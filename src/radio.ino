#include <Arduino.h>
#include <Preferences.h>
#include "Config.h"
#include "Stations.h"
#include "MorseCode.h"
#include "WiFiManager.h"

// Hardware pin definitions
#define POTENTIOMETER_PIN A0     // Potentiometer pin (ADC input)
#define LOCK_LED_PIN LED_BUILTIN // Lock LED pin
#define SPEAKER_PIN D7           // Speaker pin for Morse code output
#define BLUE_LED_PIN LED_BLUE    // Blue LED pin for Wi-Fi status
#define WIFI_BUTTON_PIN D2       // Wi-Fi toggle button pin

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

void setup()
{
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize hardware pins
  pinMode(POTENTIOMETER_PIN, INPUT);
  pinMode(LOCK_LED_PIN, OUTPUT);
  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(blueLEDPin, OUTPUT);
  pinMode(WIFI_BUTTON_PIN, INPUT_PULLUP); // Enable internal pull-up resistor

  // Turn off the blue LED initially (Wi-Fi is off)
  digitalWrite(blueLEDPin, HIGH); // Active-low configuration

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
}

void loop()
{
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

    // Check if 2 minutes have passed since Wi-Fi was started
    if (currentTime - wifiStartTime >= 120000) // 120000 ms = 2 minutes
    {
      stopWiFi();                     // Stop Wi-Fi after 2 minutes
      wifiEnabled = false;            // Update the state
      digitalWrite(blueLEDPin, HIGH); // Turn off the LED when Wi-Fi stops
    }
  }
  else
  {
    // If Wi-Fi is not enabled, turn off the LED
    digitalWrite(blueLEDPin, HIGH); // Ensure the LED is off
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
    // Adjust speaker volume based on signal strength
    unsigned int adjustedVolume = map(signalStrength, 0, 255, 0, speakerDutyCycle);

    if (!morsePlaying)
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
    }

    // Process Morse code playback
    if (morsePlaying && currentTime >= morseTimer)
    {
      if (morseToneOn)
      {
        // Turn off the tone
        ledcWrite(SPEAKER_CHANNEL, 0);
        morseToneOn = false;

        // Determine the appropriate gap
        if (currentMorseCodeIndex < currentMorseCode.length())
        {
          // Gap between parts of the same character
          morseTimer = currentTime + partGap;
        }
        else
        {
          // Gap between characters
          morseTimer = currentTime + characterGap - partGap;
        }
      }
      else
      {
        if (currentMorseCodeIndex < currentMorseCode.length())
        {
          // Play the next dot or dash
          char symbol = currentMorseCode[currentMorseCodeIndex];
          if (symbol == '.' || symbol == '-')
          {
            // Play dot or dash
            ledcWriteTone(SPEAKER_CHANNEL, morseFrequency);
            // Adjust volume based on signal strength
            ledcWrite(SPEAKER_CHANNEL, adjustedVolume);

            morseToneOn = true;
            if (symbol == '.')
            {
              morseTimer = currentTime + dotDuration;
            }
            else
            {
              morseTimer = currentTime + dashDuration;
            }
          }
          currentMorseCodeIndex++;
        }
        else
        {
          // Move to the next character
          currentMorseIndex++;
          currentMorseCodeIndex = 0;

          if (currentMorseIndex < currentMorseMessage.length())
          {
            currentMorseCode = getMorseCode(currentMorseMessage[currentMorseIndex]);
            // No need to set morseTimer here
          }
          else
          {
            // Finished the message
            morsePlaying = false;
            ledcWrite(SPEAKER_CHANNEL, 0);
          }
        }
      }
    }
  }
  else
  {
    // No station locked; play static noise
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
    if (currentTime - lastButtonPress > 500)
    { // 500 ms debounce
      toggleWiFi();
      lastButtonPress = currentTime;
    }
  }

  // Small delay for stability
  delay(10);
}
