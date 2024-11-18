#include <Arduino.h>
#include <esp_sleep.h>
#include <UMS3.h>

#include "Config.h"
#include "PowerManager.h"
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
    if constexpr (Log::enabled)
    {
      Serial.begin(115200);
    }
    Log::println("Radio Starting...");
    setCpuFrequencyMhz(80);

    // Verify the CPU frequency
    Log::println("CPU Frequency set to ", getCpuFrequencyMhz(), " MHz");
    initializeSubsystems();
  }

  void loop()
  {
    static unsigned long lastSystemUpdate = 0;
    static unsigned long lastBatteryCheck = 0;
    const unsigned long SYSTEM_UPDATE_INTERVAL = 20;    // 20ms system update interval
    const unsigned long BATTERY_CHECK_INTERVAL = 60000; // Check battery every minute

    unsigned long currentTime = millis();

    // Battery management
    if (currentTime - lastBatteryCheck >= BATTERY_CHECK_INTERVAL)
    {
      handleBatteryStatus();
      lastBatteryCheck = currentTime;
    }

    // Main system update
    if (currentTime - lastSystemUpdate >= SYSTEM_UPDATE_INTERVAL)
    {
      handleWiFiButton();
      handleTuning();
      updateManagers();
      outputDebugInfo();
      lastSystemUpdate = currentTime;
    }

    // Check for inactivity
    PowerManager::getInstance().checkActivity();

    delay(10); // Small delay to prevent overwhelming the CPU
  }

private:
  void initializeSubsystems()
  {
    // Initialize power management first
    PowerManager::getInstance().begin();

    // Initialize ConfigManager first as other systems depend on it
    ConfigManager::getInstance().begin();

    // Initialize other managers
    AudioManager::getInstance().begin();
    StationManager::getInstance().begin();
    WaveBandManager::getInstance().begin();
    WiFiManager::getInstance().begin();
    MorseCode::getInstance().begin();
    SpeedManager::getInstance().begin();
    Log::println("All subsystems initialized");
  }

  void handleBatteryStatus()
  {
    auto &power = PowerManager::getInstance();
    float voltage = power.getBatteryVoltage();

    if (power.isLowBattery())
    {
      // Flash LED pattern to indicate low battery
      for (int i = 0; i < 3; i++)
      {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
      }
    }

    Log::println("Battery Voltage: ", voltage, "V");
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
          Log::println("Station locked: ", station->getName(), " (Signal: ", signalStrength, ")");
          // Stop the static noise
          audio.stop();
        }

        morse.startMessage(station->getMessage());
        lastStation = station;
      }
    }
    else
    {
      if (config.isMorsePlaying())
      {
        Log::println("Lost station lock");
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

    Log::println("\n\r=== System Status ===");

    // Wave Band Status
    Log::println("Wave Band: ", toString(config.getWaveBand()));

    // Tuning Status
    int tuningValue = analogRead(Pins::TUNING_POT);
    int volumeValue = analogRead(Pins::VOLUME_POT);
    Log::println("Tuning: ", tuningValue, ", Volume: ", volumeValue);

    // Morse Status
    Log::println("Morse Playing: ", config.isMorsePlaying() ? "Yes" : "No", ", Speed: ", toString(config.getMorseSpeed()));

    // WiFi Status
    Log::println("WiFi Enabled: ", WiFiManager::getInstance().isEnabled() ? "Yes" : "No");

    Log::println("==================\n\r");
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
