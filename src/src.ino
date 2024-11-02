#include <Arduino.h>
#include <Preferences.h>
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
UMS3 ums3;
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
  ums3.begin();
  ums3.setPixelBrightness(5);

  pinMode(WIFI_BUTTON_PIN, INPUT_PULLUP); // Set GPIO0 as input with pull-up resistor
  initWiFiManager();

  ledcSetup(SPEAKER_CHANNEL, 5000, 8); // 5 kHz frequency, 8-bit resolution
  ledcAttachPin(SPEAKER_PIN, SPEAKER_CHANNEL);
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
  Station *lockedStation = nullptr;
  int signalStrength = 0;

  // Determine the closest station on the current wave band
  for (int i = 0; i < numStations; i++)
  {
    if (stations[i].band == currentWave)
    { // Only consider stations on the current wave band
      int strength = calculateSignalStrength(tuningValue, stations[i].frequency);
      if (strength > signalStrength)
      {
        signalStrength = strength;
        lockedStation = &stations[i];
      }
    }
  }

  if (signalStrength > 0)
  {
    // Station is locked
    if (lockedStation != nullptr && !morsePlaying)
    {
      Serial.print("Station locked: ");
      Serial.print(lockedStation->name);
      Serial.print(" | Frequency: ");
      Serial.print(lockedStation->frequency);
      Serial.print(" | Signal Strength: ");
      Serial.println(signalStrength);

      // Start Morse code playback
      playMorseMessage(lockedStation->message);
      morsePlaying = true;
      stationLockTime = millis();
    }
  }
  else
  {
    // No station locked
    if (morsePlaying)
    {
      Serial.println("Lost station lock. Stopping Morse code.");
      stopMorse();
      morsePlaying = false;
    }
  }

  handleWiFi();
  updateWiFiLED();

  // Debug output every second for current tuning value and status
  unsigned long currentTime = millis();
  if (currentTime - lastDebugTime >= 1000)
  {
    Serial.println("---- Debug Data ----");
    Serial.print("Current Tuning Value: ");
    Serial.println(tuningValue);
    Serial.print("Current Wave: ");
    Serial.println(currentWave == LONG_WAVE ? "Long Wave" : (currentWave == MEDIUM_WAVE ? "Medium Wave" : "Short Wave"));
    Serial.println(morsePlaying ? "Morse Code Playing" : "No Morse Code Playing");
    Serial.println("--------------------");
    lastDebugTime = currentTime;
  }

  delay(10);
}
