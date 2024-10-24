// src.ino
#include <Arduino.h>
#include <Preferences.h>
#include <esp_sleep.h>
#include <UMS3.h>
#include "MorseCodePlayer.h"
#include "AudioManager.h"
#include "StationManager.h"
#include "WiFiManager.h"
#include "PowerManager.h"

// Hardware pin definitions
#define POTENTIOMETER_PIN 17     // Potentiometer pin (ADC input)
#define LOCK_LED_PIN 7           // Lock LED pin
#define SPEAKER_PIN 1            // Speaker pin for Morse code output
#define BLUE_LED_PIN LED_BUILTIN // Blue LED pin for Wi-Fi status
#define WIFI_BUTTON_PIN 33       // Wi-Fi toggle button pin
#define WAKEUP_PIN 9             // Wake-up pin for deep sleep

// Global objects
UMS3 ums3;
Preferences preferences;

AudioManager audioManager(SPEAKER_PIN, 1);
StationManager stationManager(100); // Leeway of 100
MorseCodePlayer morseCodePlayer;
PowerManager powerManager(ums3, 300000); // 5 minutes timeout

WiFiManager wifiManager(BLUE_LED_PIN, WIFI_BUTTON_PIN);

// Configuration variables
String londonMessage;
String hilversumMessage;
String barcelonaMessage;
unsigned int speakerDutyCycle;
unsigned int morseFrequency;
unsigned int morseSpeed;

void loadConfigurations()
{
  preferences.begin("config", false);

  londonMessage = preferences.getString("londonMsg", "L");
  hilversumMessage = preferences.getString("hilversumMsg", "H");
  barcelonaMessage = preferences.getString("barcelonaMsg", "B");
  speakerDutyCycle = preferences.getUInt("volume", 64);
  morseFrequency = preferences.getUInt("frequency", 800);
  morseSpeed = preferences.getUInt("morseSpeed", 1);

  preferences.end();

  // Update Morse code timing
  unsigned int dotDuration;
  switch (morseSpeed)
  {
  case 0:
    dotDuration = 500;
    break;
  case 1:
    dotDuration = 300;
    break;
  case 2:
    dotDuration = 100;
    break;
  default:
    dotDuration = 300;
    break;
  }
  morseCodePlayer.setSpeed(dotDuration);
  morseCodePlayer.setFrequency(morseFrequency);
  morseCodePlayer.setVolume(speakerDutyCycle);
}

void saveConfigurations()
{
  preferences.begin("config", false);

  preferences.putString("londonMsg", londonMessage);
  preferences.putString("hilversumMsg", hilversumMessage);
  preferences.putString("barcelonaMsg", barcelonaMessage);
  preferences.putUInt("volume", speakerDutyCycle);
  preferences.putUInt("frequency", morseFrequency);
  preferences.putUInt("morseSpeed", morseSpeed);

  preferences.end();
}

void setMorseSpeed(unsigned int dotDuration)
{
  morseCodePlayer.setSpeed(dotDuration);
}

void setup()
{
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize UMS3
  ums3.begin();
  ums3.setPixelBrightness(5);

  // Initialize hardware pins
  pinMode(POTENTIOMETER_PIN, INPUT);
  pinMode(LOCK_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(WIFI_BUTTON_PIN, INPUT_PULLUP); // Enable internal pull-up resistor
  pinMode(WAKEUP_PIN, INPUT_PULLUP);      // Configure the wake-up pin

  // Turn off the blue LED initially (Wi-Fi is off)
  digitalWrite(BLUE_LED_PIN, LOW); // Active-low configuration

  // Initialize audio manager
  audioManager.init();

  // Initialize lock LED PWM
  const int LOCK_LED_CHANNEL = 0;
  const int PWM_FREQUENCY = 5000; // PWM frequency in Hz
  const int PWM_RESOLUTION = 8;   // PWM resolution (8-bit)

  ledcSetup(LOCK_LED_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(LOCK_LED_PIN, LOCK_LED_CHANNEL);

  // Load saved configurations
  loadConfigurations();

  // Initialize Wi-Fi manager
  wifiManager.init();

  // Initialize station manager
  stationManager.addStation("London", 1000);
  stationManager.addStation("Hilversum", 2000);
  stationManager.addStation("Barcelona", 3000);

  // Reset inactivity timer
  powerManager.resetInactivityTimer();
}

void loop()
{
  // Check battery status
  static unsigned long lastCheckTime = 0;
  unsigned long currentTime = millis();
  if (currentTime - lastCheckTime >= 100)
  {
    powerManager.checkBattery();
    lastCheckTime = currentTime;
  }
  // Update inactivity timer
  powerManager.updateInactivity(wifiManager.isWiFiEnabled());

  // Handle Wi-Fi tasks
  wifiManager.handle();

  // Read potentiometer value (0-4095 for ESP32 ADC)
  int potValue = analogRead(POTENTIOMETER_PIN);

  // Determine the strongest station
  int signalStrength = 0;
  const Station *lockedStation = stationManager.getStrongestStation(potValue, signalStrength);

  // Set lock LED brightness
  ledcWrite(0, signalStrength); // Assuming lock LED is on PWM channel 0

  if (lockedStation && signalStrength > 0)
  {
    // Station is locked
    // Start Morse code playback if not already playing
    if (!morseCodePlayer.isPlaying())
    {
      // Set the Morse message based on the locked station
      if (lockedStation->name == "London")
      {
        morseCodePlayer.setMessage(londonMessage);
      }
      else if (lockedStation->name == "Hilversum")
      {
        morseCodePlayer.setMessage(hilversumMessage);
      }
      else if (lockedStation->name == "Barcelona")
      {
        morseCodePlayer.setMessage(barcelonaMessage);
      }
      else
      {
        morseCodePlayer.setMessage("S"); // Default message
      }

      Serial.print("Station locked: ");
      Serial.println(lockedStation->name);

      // Reset inactivity timer
      powerManager.resetInactivityTimer();
    }

    // Update Morse code playback
    morseCodePlayer.update();

    if (morseCodePlayer.isPlaying())
    {
      if (morseCodePlayer.isToneOn())
      {
        // Play tone
        audioManager.playTone(morseCodePlayer.getFrequency(), morseCodePlayer.getVolume());
      }
      else
      {
        // Stop tone
        audioManager.stopTone();
      }
    }
    else
    {
      // Morse code playback finished
      audioManager.stopTone();
    }
  }
  else
  {
    // No station locked
    if (morseCodePlayer.isPlaying())
    {
      morseCodePlayer.stop(); // Stop playback
      audioManager.stopTone();
    }

    // Play static noise at configured volume
    audioManager.playStaticNoise(speakerDutyCycle);
  }

  // Handle Wi-Fi button press
  wifiManager.updateButton();

  // Check if should enter deep sleep
  if (powerManager.shouldSleep())
  {
    Serial.println("Inactivity timeout reached, entering deep sleep");
    // Clean up and enter deep sleep
    wifiManager.stop();
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_6, 0); // Wake up when the pin is LOW
    esp_deep_sleep_start();
  }

  // Small delay
  delay(10);
}
