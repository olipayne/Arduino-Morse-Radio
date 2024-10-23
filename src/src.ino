#include <Arduino.h>
#include <Preferences.h>
#include "Config.h"
#include "Stations.h"
#include "MorseCode.h"
#include "WiFiManager.h"
#include <esp_sleep.h>
#include <UMS3.h>

// Hardware pins
#define POTENTIOMETER_PIN 17     // Potentiometer pin (ADC input)
#define LOCK_LED_PIN 7           // Lock LED pin
#define SPEAKER_PIN 1            // Speaker pin for Morse code output
#define BLUE_LED_PIN LED_BUILTIN // Blue LED pin for Wi-Fi status
#define WIFI_BUTTON_PIN 33       // Wi-Fi toggle button pin
#define WAKEUP_PIN 9             // Wake-up pin for deep sleep

// PWM configurations
const int PWM_FREQUENCY = 5000; // PWM frequency in Hz
const int PWM_RESOLUTION = 8;   // PWM resolution (8-bit)
const int LOCK_LED_CHANNEL = 0; // PWM channel for lock LED
const int SPEAKER_CHANNEL = 1;  // PWM channel for speaker

// Leeway for station locking
const int LEEWAY = 100; // Range around each station frequency

// Variables for speaker settings
unsigned int speakerDutyCycle = 64; // Speaker volume (duty cycle)
unsigned int morseFrequency = 800;  // Speaker frequency (Hz)

// Morse code playback variables
String currentMorseMessage = "";
int currentMorseIndex = 0;
String currentMorseCode = "";
int currentMorseCodeIndex = 0;
unsigned long morseTimer = 0;
bool morsePlaying = false;
bool morseToneOn = false;

// Morse code speed settings
MorseSpeed morseSpeed = MEDIUM_SPEED; // Default Morse code speed

// Timing variables (milliseconds)
unsigned int dotDuration = 100;
unsigned int dashDuration = 300;
unsigned int partGap = 100;
unsigned int characterGap = 500; // Default value for character gap

Preferences preferences;

// Configuration variables
String londonMessage = preferences.getString("londonMsg", "L");
String hilversumMessage = preferences.getString("hilversumMsg", "H");
String barcelonaMessage = preferences.getString("barcelonaMsg", "B");

// Preferences for storing configurations

// Wi-Fi control variables
bool wifiEnabled = false;          // Wi-Fi is off by default
unsigned long lastButtonPress = 0; // For debouncing the Wi-Fi button

// Externally declared hardware pins
const int blueLEDPin = BLUE_LED_PIN;

// Global variable to track Wi-Fi start time
unsigned long wifiStartTime = 0;

// Global variable to track when the station was locked
unsigned long stationLockTime = 0;

// Define the deep sleep timeout (e.g., 5 minutes)
const unsigned long DEEP_SLEEP_TIMEOUT = 30000; // 30000 ms = 0.5 minutes

// Global variable to track the last activity time
unsigned long lastActivityTime = 0;
unsigned long lastBatteryCheck = 0;

#define BATTERY_CHECK_INTERVAL 1000

UMS3 ums3;

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
  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(blueLEDPin, OUTPUT);
  pinMode(WIFI_BUTTON_PIN, INPUT_PULLUP); // Enable internal pull-up resistor
  pinMode(WAKEUP_PIN, INPUT_PULLUP);      // Configure the wake-up pin

  // Turn off the blue LED initially (Wi-Fi is off)
  digitalWrite(blueLEDPin, LOW); // Active-low configuration

  // Configure PWM channels
  ledcSetup(LOCK_LED_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(LOCK_LED_PIN, LOCK_LED_CHANNEL);
  ledcSetup(SPEAKER_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(SPEAKER_PIN, SPEAKER_CHANNEL);

  // Initially set the speaker to silent
  ledcWrite(SPEAKER_CHANNEL, 0);

  // Load saved configurations
  loadConfigurations();

  // Set initial Morse code speed
  setMorseSpeed(morseSpeed);

  // Initialize Wi-Fi manager
  initWiFiManager();

  // Record the initial activity time
  lastActivityTime = millis();
}

void loop()
{
  if (lastBatteryCheck == 0 || millis() - lastBatteryCheck > BATTERY_CHECK_INTERVAL)
  {
    checkBattery();
    lastBatteryCheck = millis();
  }

  // Read potentiometer value (0-4095 for ESP32 ADC)
  int potValue = analogRead(POTENTIOMETER_PIN);

  unsigned long currentTime = millis();

  // Check if Wi-Fi is enabled and handle Wi-Fi tasks
  if (wifiEnabled)
  {
    handleWiFi();

    // Flash the blue LED while Wi-Fi is active
    static unsigned long ledFlashStartTime = 0;  // Store the start time for LED flashing
    static unsigned long ledFlashInterval = 500; // Flash every 500 ms
    static bool ledState = LOW;                  // Current state of the LED

    // Check if it's time to toggle the LED
    if (currentTime - ledFlashStartTime >= ledFlashInterval)
    {
      ledState = !ledState;                  // Toggle LED state
      digitalWrite(blueLEDPin, ledState);    // Set LED state
      ledFlashStartTime += ledFlashInterval; // Update start time for next flash
    }

    // Check if 2 minutes have passed since the last request
    if (currentTime >= wifiStartTime && (currentTime - wifiStartTime >= 120000)) // 120000 ms = 2 minutes
    {
      Serial.println("2 minutes passed, stopping Wi-Fi");
      Serial.print("Current time: ");
      Serial.println(currentTime);
      Serial.print("WiFi start time: ");
      Serial.println(wifiStartTime);
      stopWiFi();                    // Stop Wi-Fi after 2 minutes
      wifiEnabled = false;           // Update the state
      digitalWrite(blueLEDPin, LOW); // Turn off the LED when Wi-Fi stops
    }
  }
  else
  {
    // If Wi-Fi is not enabled, turn off the LED
    digitalWrite(blueLEDPin, LOW); // Ensure the LED is off
  }

  bool stationLocked = false;
  int lockBrightness = 0;
  Station *lockedStation = nullptr;
  int signalStrength = 0;

  // Determine the closest station
  for (int i = 0; i < numStations; i++)
  {
    int strength = calculateSignalStrength(potValue, stations[i].frequency);

    if (strength > 0)
    {
      // Signal detected for this station
      if (strength > signalStrength)
      {
        signalStrength = strength;
        lockBrightness = strength;
        stationLocked = true;
        lockedStation = &stations[i];
      }
    }
  }

  // Set the lock LED brightness
  ledcWrite(LOCK_LED_CHANNEL, lockBrightness);

  if (stationLocked)
  {
    if (stationLockTime == 0)
    {
      // Record the time when the station was locked
      stationLockTime = currentTime;
      Serial.print("Station lock acquired: ");
      Serial.println(lockedStation->name);

      // Update last activity time when a station lock is acquired
      lastActivityTime = currentTime;

      // Turn off the speaker output for 2 seconds
      ledcWrite(SPEAKER_CHANNEL, 0);
    }

    // Check if the station has been locked for more than 2 seconds
    if (!morsePlaying && (currentTime - stationLockTime >= 2000))
    {
      // Set the Morse message based on the locked station
      if (strcmp(lockedStation->name, "London") == 0)
      {
        currentMorseMessage = londonMessage;
      }
      else if (strcmp(lockedStation->name, "Hilversum") == 0)
      {
        currentMorseMessage = hilversumMessage;
      }
      else if (strcmp(lockedStation->name, "Barcelona") == 0)
      {
        currentMorseMessage = barcelonaMessage;
      }
      else
      {
        currentMorseMessage = "S"; // Default message
      }

      // Initialize Morse code playback variables
      currentMorseIndex = 0;
      currentMorseCodeIndex = 0;
      currentMorseCode = getMorseCode(currentMorseMessage[currentMorseIndex]);
      morsePlaying = true;
      morseTimer = currentTime;
      morseToneOn = false;

      Serial.println("Starting Morse code playback.");
    }

    // Morse code playback logic
    if (morsePlaying)
    {
      if (currentMorseCodeIndex < currentMorseCode.length())
      {
        if (currentTime - morseTimer >= (morseToneOn ? dashDuration : dotDuration))
        {
          morseToneOn = !morseToneOn;
          morseTimer = currentTime;

          if (morseToneOn)
          {
            // Turn on the speaker for the current Morse code element
            ledcWriteTone(SPEAKER_CHANNEL, morseFrequency);
            ledcWrite(SPEAKER_CHANNEL, speakerDutyCycle);
          }
          else
          {
            // Turn off the speaker
            ledcWrite(SPEAKER_CHANNEL, 0);
            currentMorseCodeIndex++;
          }
        }
      }
      else
      {
        // Move to the next character in the message
        currentMorseIndex++;
        if (currentMorseIndex < currentMorseMessage.length())
        {
          currentMorseCode = getMorseCode(currentMorseMessage[currentMorseIndex]);
          currentMorseCodeIndex = 0;
        }
        else
        {
          // End of message
          morsePlaying = false;
          Serial.println("Morse code playback finished.");
        }
      }
    }
  }
  else
  {
    if (stationLockTime != 0)
    {
      Serial.println("Station lock lost.");
    }

    // Reset the station lock time and morsePlaying if no station is locked
    stationLockTime = 0;
    morsePlaying = false;

    // Calculate overall signal strength (proximity to any station)
    int overallSignalStrength = 0;
    for (int i = 0; i < numStations; i++)
    {
      int strength = calculateSignalStrength(potValue, stations[i].frequency);
      if (strength > overallSignalStrength)
      {
        overallSignalStrength = strength;
      }
    }

    playStaticNoise(overallSignalStrength);
  }

  // Handle Wi-Fi button press (debounced)
  if (digitalRead(WIFI_BUTTON_PIN) == LOW)
  {
    unsigned long currentTime = millis();
    if (currentTime - lastButtonPress > 500) // 500 ms debounce
    {
      toggleWiFi();
      lastButtonPress = currentTime;
    }
  }

  // Check if the device is plugged in (GPIO pin 34 is high)
  if (ums3.getVbusPresent()) // Assuming HIGH means plugged in
  {
    // Reset the inactivity timer
    // Serial.println("Device is plugged in, resetting inactivity timer");
    lastActivityTime = currentTime;
  }

  // Check if the deep sleep timeout has been reached
  if (currentTime - lastActivityTime >= DEEP_SLEEP_TIMEOUT)
  {
    Serial.println("5 minutes of inactivity, entering deep sleep");
    // Stop all activities
    stopWiFi();
    // Add any other cleanup code here if necessary
    // Configure the wake-up source
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_6, 0); // Wake up when the pin is LOW
    // Enter deep sleep
    esp_deep_sleep_start();
  }

  // Small delay for stability
  delay(10);
}

void checkBattery()
{
  // Get the battery voltage, corrected for the on-board voltage divider
  // Full should be around 4.2v and empty should be around 3v
  float battery = ums3.getBatteryVoltage();
  Serial.println(String("Battery: ") + battery);

  if (ums3.getVbusPresent())
  {
    // If USB power is present
    if (battery < 4.0)
    {
      // Charging - blue
      ums3.setPixelColor(0x0000FF);
    }
    else
    {
      // Close to full - off
      ums3.setPixelColor(0x000000);
    }
  }
  else
  {
    // Else, USB power is not present (running from battery)
    if (battery < 3.1)
    {
      // Uncomment the following line to sleep when the battery is critically low
      // esp_deep_sleep_start();
    }
    else if (battery < 3.3)
    {
      // Below 3.3v - red
      ums3.setPixelColor(0xFF0000);
    }
    else if (battery < 3.6)
    {
      // Below 3.6v (around 50%) - orange
      ums3.setPixelColor(0xFF8800);
    }
    else
    {
      // Above 3.6v - green
      ums3.setPixelColor(0x00FF00);
    }
  }
}
