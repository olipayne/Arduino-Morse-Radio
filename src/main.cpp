#include <Arduino.h>
#include <esp_sleep.h>
#include <UMS3.h>

#include "Config.h"
#include "AudioManager.h"
#include "MorseCode.h"
#include "StationManager.h"
#include "WaveBandManager.h"
#include "WiFiManager.h"
#include "SpeedManager.h"
// Main system manager class
class RadioSystem
{
public:
  void begin()
  {
    Serial.begin(115200);
    Serial.println("\nTeam Building Radio Starting...");

    // Initialize all subsystems
    initializeSubsystems();
    setupDebugOutput();
  }

  void loop()
  {
    handleWiFiButton();
    handleTuning();
    updateManagers();
    outputDebugInfo();

    // Small delay to prevent overwhelming the CPU
    delay(10);
  }

private:
  void initializeSubsystems()
  {
    // Initialize ConfigManager first as other systems depend on it
    ConfigManager::getInstance().begin();

    // Initialize other managers
    AudioManager::getInstance().begin();
    StationManager::getInstance().begin();
    WaveBandManager::getInstance().begin();
    WiFiManager::getInstance().begin();
    MorseCode::getInstance().begin();
    SpeedManager::getInstance().begin();

    Serial.println("All subsystems initialized");
  }

  void handleWiFiButton()
  {
    static unsigned long lastButtonPress = 0;
    const unsigned long debounceTime = Timing::DEBOUNCE_DELAY;

    // Check WiFi toggle button with debounce
    if (digitalRead(Pins::WIFI_BUTTON) == LOW)
    {
      unsigned long currentTime = millis();
      if (currentTime - lastButtonPress > debounceTime)
      {
        WiFiManager::getInstance().toggle();
        lastButtonPress = currentTime;
      }
    }
  }

  void handleTuning()
  {
    // Update the current wave band
    WaveBandManager::getInstance().update();

    // Read tuning and get signal strength
    int tuningValue = analogRead(Pins::TUNING_POT);
    int signalStrength = 0;

    // Find closest station on current band
    auto &config = ConfigManager::getInstance();
    auto &stations = StationManager::getInstance();
    Station *closestStation = stations.findClosestStation(
        tuningValue,
        config.getWaveBand(),
        signalStrength);

    // Handle station tuning
    handleStationTuning(closestStation, signalStrength);
  }

  void handleStationTuning(Station *station, int signalStrength)
  {
    auto &config = ConfigManager::getInstance();
    auto &audio = AudioManager::getInstance();
    auto &morse = MorseCode::getInstance();

    static Station *lastStation = nullptr;
    static unsigned long stationLockTime = 0;

    bool stationLocked = (signalStrength > 0);

    if (stationLocked)
    {
      if (!config.isMorsePlaying() || station != lastStation)
      {
        // New station found or changed
        if (station != lastStation)
        {
          Serial.printf("Station locked: %s (Signal: %d)\n",
                        station->getName(), signalStrength);
        }

        morse.startMessage(station->getMessage());
        lastStation = station;
      }
    }
    else
    {
      if (config.isMorsePlaying())
      {
        Serial.println("Lost station lock");
        morse.stop();
        lastStation = nullptr;
      }
      audio.playStaticNoise(signalStrength);
    }

    // Update morse code state
    morse.update();
  }

  void updateManagers()
  {
    // Update all managers that need regular processing
    AudioManager::getInstance().handlePlayback();
    WiFiManager::getInstance().handle();
    WaveBandManager::getInstance().updateLEDs();
    SpeedManager::getInstance().update();
  }

  void setupDebugOutput()
  {
    lastDebugOutput = 0;
  }

  void outputDebugInfo()
  {
    unsigned long currentTime = millis();
    if (currentTime - lastDebugOutput >= Timing::DEBUG_INTERVAL)
    {
      printSystemStatus();
      lastDebugOutput = currentTime;
    }
  }

  void printSystemStatus()
  {
    auto &config = ConfigManager::getInstance();

    Serial.println("\n=== System Status ===");

    // Wave Band Status
    Serial.printf("Wave Band: %s\n", toString(config.getWaveBand()));

    // Tuning Status
    int tuningValue = analogRead(Pins::TUNING_POT);
    int volumeValue = analogRead(Pins::VOLUME_POT);
    Serial.printf("Tuning: %d, Volume: %d\n", tuningValue, volumeValue);

    // Morse Status
    Serial.printf("Morse Playing: %s, Speed: %s\n",
                  config.isMorsePlaying() ? "Yes" : "No",
                  toString(config.getMorseSpeed()));

    // WiFi Status
    Serial.printf("WiFi Enabled: %s\n",
                  WiFiManager::getInstance().isEnabled() ? "Yes" : "No");

    Serial.println("==================\n");
  }

  unsigned long lastDebugOutput;
};

// Global system instance
RadioSystem radioSystem;

void setup()
{
  radioSystem.begin();
}

void loop()
{
  radioSystem.loop();
}