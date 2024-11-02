#include <Arduino.h>
#include <vector>
#include "Config.h"
#include "Station.h"
#include "MorseCode.h"
#include "WiFiManager.h"
#include <esp_sleep.h>
#include <UMS3.h>

using namespace Config;
using namespace MorseCode;
using namespace WiFiManager;

// Hardware pin definitions
constexpr int POTENTIOMETER_PIN = 17;
constexpr int LOCK_LED_PIN = 5; // Use a safe GPIO pin for the lock LED
constexpr int SPEAKER_PIN = 1;
constexpr int WIFI_BUTTON_PIN = 33;
constexpr int WAKEUP_PIN = 9;

// PWM configurations
constexpr int PWM_FREQUENCY = 5000;
constexpr int PWM_RESOLUTION = 8;

// Define separate timers and channels for PWM
constexpr int LOCK_LED_CHANNEL = 0;
constexpr int LOCK_LED_TIMER = 0; // Timer 0

constexpr int SPEAKER_TIMER = 1; // Timer 1

// Morse code playback variables
String currentMorseMessage = "";
size_t currentMorseIndex = 0;
String currentMorseCode = "";
size_t currentMorseCodeIndex = 0;
unsigned long morseTimer = 0;
bool morsePlaying = false;
bool morseToneOn = false;

// Wi-Fi control variables
unsigned long lastButtonPress = 0;

// Global variable to track when the station was locked
unsigned long stationLockTime = 0;

// Define the deep sleep timeout (e.g., 5 minutes)
constexpr unsigned long DEEP_SLEEP_TIMEOUT = 300000;

// Global variable to track the last activity time
unsigned long lastActivityTime = 0;
unsigned long lastBatteryCheck = 0;

UMS3 ums3;

// Variables for LED flashing
bool ledState = LOW;                // Current state of the blue LED
unsigned long previousMillis = 0;   // Stores the last time the LED was updated
const unsigned long interval = 500; // Interval at which to blink (milliseconds)

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
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(WIFI_BUTTON_PIN, INPUT_PULLUP);
  pinMode(WAKEUP_PIN, INPUT_PULLUP);

  // Turn off the blue LED initially (Wi-Fi is off)
  digitalWrite(BLUE_LED_PIN, LOW);

  // Configure PWM channels with specified timers
  ledcSetup(LOCK_LED_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(LOCK_LED_PIN, LOCK_LED_CHANNEL);

  ledcSetup(SPEAKER_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(SPEAKER_PIN, SPEAKER_CHANNEL);

  // Initially set the speaker to silent
  ledcWrite(SPEAKER_CHANNEL, 0);

  // Load saved configurations
  loadConfigurations();

  // Initialize Wi-Fi manager
  initWiFiManager();

  // Record the initial activity time
  lastActivityTime = millis();
}

void loop()
{
  unsigned long currentTime = millis();

  // Check battery every 200 ms
  if (currentTime - lastBatteryCheck >= 200)
  {
    checkBattery();
    lastBatteryCheck = currentTime;
  }

  // Read potentiometer value
  int potValue = analogRead(POTENTIOMETER_PIN);

  // Handle Wi-Fi tasks if enabled
  if (wifiEnabled)
  {
    handleWiFi();

    // Flash the blue LED while Wi-Fi is active
    if (currentTime - previousMillis >= interval)
    {
      previousMillis = currentTime;
      ledState = !ledState; // Toggle the LED state
      digitalWrite(BLUE_LED_PIN, ledState);
    }

    // Check if 2 minutes have passed since the last activity
    if ((long)(currentTime - wifiStartTime) >= 120000)
    {
      Serial.println("2 minutes passed, stopping Wi-Fi");
      stopWiFi();
      wifiEnabled = false;
      digitalWrite(BLUE_LED_PIN, LOW); // Ensure the LED is off
    }
  }
  else
  {
    // Ensure the LED is off when Wi-Fi is not enabled
    digitalWrite(BLUE_LED_PIN, LOW);
  }

  bool stationLocked = false;
  int lockBrightness = 0;
  Station *lockedStation = nullptr;
  int signalStrength = 0;

  // Determine the closest station
  for (auto &station : stations)
  {
    int strength = calculateSignalStrength(potValue, station.getFrequency());

    if (strength > 0)
    {
      if (strength > signalStrength)
      {
        signalStrength = strength;
        lockBrightness = strength;
        stationLocked = true;
        lockedStation = &station;
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
      Serial.println(lockedStation->getName());

      // Update last activity time when a station lock is acquired
      lastActivityTime = currentTime;

      // Turn off the speaker output for 2 seconds
      ledcWrite(SPEAKER_CHANNEL, 0);
    }

    // Check if the station has been locked for more than 2 seconds
    if (!morsePlaying && (currentTime - stationLockTime >= 2000))
    {
      // Set the Morse message based on the locked station
      currentMorseMessage = lockedStation->getMessage();

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
        unsigned int duration = (currentMorseCode[currentMorseCodeIndex] == '-') ? dashDuration : dotDuration;

        if (currentTime - morseTimer >= duration)
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

          // Add a gap between characters
          morseTimer = currentTime + characterGap;
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
    for (auto &station : stations)
    {
      int strength = calculateSignalStrength(potValue, station.getFrequency());
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
    {
      toggleWiFi();
      lastButtonPress = currentTime;
    }
  }

  // Check if the device is plugged in (Vbus present)
  if (ums3.getVbusPresent())
  {
    // Reset the inactivity timer
    lastActivityTime = currentTime;
  }

  // Check if the deep sleep timeout has been reached
  if (currentTime - lastActivityTime >= DEEP_SLEEP_TIMEOUT)
  {
    Serial.println("Inactivity timeout reached, entering deep sleep");
    stopWiFi();
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_6, 0); // Adjust GPIO as needed
    esp_deep_sleep_start();
  }

  delay(10);
}

void checkBattery()
{
  static float batterySum = 0.0f;
  static int batteryCount = 0;

  float batteryVoltage = ums3.getBatteryVoltage();
  bool charging = ums3.getVbusPresent();

  batterySum += batteryVoltage;
  batteryCount++;

  if (batteryCount >= 25)
  {
    float averageBattery = batterySum / batteryCount;
    float averagePercentage = ((averageBattery - 3.1f) / (4.2f - 3.1f)) * 100.0f;
    averagePercentage = constrain(averagePercentage, 0.0f, 100.0f);

    Serial.printf("Battery: %.2f%% (%.2fV) Charging: %s\n", averagePercentage, averageBattery, charging ? "Yes" : "No");

    batterySum = 0.0f;
    batteryCount = 0;
  }
}
