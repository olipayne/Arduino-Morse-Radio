#include <Arduino.h>
#include "Config.h"
#include "Stations.h"
#include "MorseCode.h"
#include "WiFiManager.h"
#include "AudioManager.h"
#include "WaveBandManager.h"
#include "SpeedManager.h"
#include <esp_sleep.h>
#include <UMS3.h>

Preferences preferences;
unsigned long lastDebugTime = 0;
unsigned long lastButtonPress = 0;
unsigned long stationLockTime = 0; // Track when the station was locked
const int WIFI_BUTTON_PIN = 0;     // GPIO0 for Wi-Fi toggle button
const int DEBOUNCE_DELAY = 500;    // 500 ms debounce delay
const int SPEAKER_PIN = 1;         // GPIO1 for speaker
const int SPEAKER_CHANNEL = 0;     // PWM channel for speaker output

void setup()
{
  Serial.begin(115200);

  // Set up PWM on SPEAKER_PIN
  ledcSetup(SPEAKER_CHANNEL, 5000, 8); // 5 kHz frequency, 8-bit resolution
  ledcAttachPin(SPEAKER_PIN, SPEAKER_CHANNEL);

  pinMode(WIFI_BUTTON_PIN, INPUT_PULLUP); // Set GPIO0 as input with pull-up resistor
  initWiFiManager();
}

void loop()
{
  // Check the Wi-Fi toggle button (GPIO0)
  int buttonState = digitalRead(WIFI_BUTTON_PIN); // Read GPIO0 state
  if (buttonState == LOW)
  {
    unsigned long currentTime = millis();
    if (currentTime - lastButtonPress > DEBOUNCE_DELAY)
    { // Debounce check
      Serial.println("Button pressed, toggling Wi-Fi");
      toggleWiFi();
      lastButtonPress = currentTime;
    }
  }

  // Tuning logic for Morse code playback
  int tuningValue = analogRead(TUNING_ADC); // Read the tuning potentiometer value
  int volumeValue = analogRead(VOLUME_ADC); // Read the volume potentiometer value

  // Determine the closest station on the current wave band
  Station *closestStation = nullptr;
  int closestSignalStrength = 0;
  int distanceToClosestStation = -1; // Distance to the closest station if outside lock range

  for (int i = 0; i < numStations; i++)
  {
    if (stations[i].band == currentWave)
    { // Only consider stations on the current wave band
      int strength = calculateSignalStrength(tuningValue, stations[i].frequency);
      int distance = abs(tuningValue - stations[i].frequency);

      if (strength > closestSignalStrength)
      {
        closestSignalStrength = strength;
        closestStation = &stations[i];
        distanceToClosestStation = distance;
      }
    }
  }

  bool stationLocked = (closestSignalStrength > 0);
  if (stationLocked && !morsePlaying)
  {
    // Start Morse code playback
    Serial.print("Station locked: ");
    Serial.print(closestStation->name);
    Serial.print(" | Frequency: ");
    Serial.print(closestStation->frequency);
    Serial.print(" | Signal Strength: ");
    Serial.println(closestSignalStrength);

    playMorseMessage(closestStation->message);
    morsePlaying = true;
    stationLockTime = millis();
  }
  else if (!stationLocked && morsePlaying)
  {
    // Stop Morse code playback if lock is lost
    Serial.println("Lost station lock. Stopping Morse code.");
    stopMorse();
    morsePlaying = false;
  }

  handleWiFi();
  updateWiFiLED();

  // Debug output every second
  unsigned long currentTime = millis();
  if (currentTime - lastDebugTime >= 1000)
  {
    Serial.println("---- Debug Data ----");

    // Volume Potentiometer
    Serial.print("Volume Potentiometer Value: ");
    Serial.println(volumeValue);

    // Current Tuning Value and Wave Band
    Serial.print("Current Tuning Value: ");
    Serial.println(tuningValue);
    Serial.print("Current Wave: ");
    Serial.println(currentWave == LONG_WAVE ? "Long Wave" : (currentWave == MEDIUM_WAVE ? "Medium Wave" : "Short Wave"));

    // Closest Station Information
    if (closestStation != nullptr)
    {
      Serial.print("Closest Station: ");
      Serial.print(closestStation->name);
      Serial.print(" | Frequency: ");
      Serial.print(closestStation->frequency);
      Serial.print(" | Distance: ");
      Serial.print(distanceToClosestStation);
      Serial.print(" | Signal Strength: ");
      Serial.println(closestSignalStrength);
    }
    else
    {
      Serial.println("No station detected on current wave band.");
    }

    // Decode Speed
    Serial.print("Decode Speed: ");
    Serial.println(morseSpeed == LOW_SPEED ? "Slow" : (morseSpeed == MEDIUM_SPEED ? "Medium" : "Fast"));

    // Station Lock and Morse Code Status
    Serial.println(morsePlaying ? "Morse Code Playing" : "No Morse Code Playing");

    Serial.println("--------------------");
    lastDebugTime = currentTime;
  }

  delay(10);
}
