#include <Arduino.h>
#include <TaskScheduler.h>
#include <UMS3.h>
#include <esp_ota_ops.h>
#include <esp_sleep.h>

#include "AudioManager.h"
#include "Config.h"
#include "MorseCode.h"
#include "PowerManager.h"
#include "SignalManager.h"
#include "SpeedManager.h"
#include "StationManager.h"
#include "Version.h"  // Include the auto-generated version header
#include "WaveBandManager.h"
#include "WiFiManager.h"

// Task Scheduler
Scheduler ts;

// Task forward declarations
void batteryCheckCallback();
void systemUpdateCallback();
void handleWiFiButton(unsigned long& lastButtonPress, const unsigned long debounceTime);
void updateTuningAndStation();

// Tasks
Task tBatteryCheck(60000, TASK_FOREVER, &batteryCheckCallback);  // Check battery every minute
Task tSystemUpdate(20, TASK_FOREVER, &systemUpdateCallback);     // System update every 20ms

// Main system manager class
class RadioSystem {
 public:
  void begin() {
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.begin(115200);
    Serial.println(F("\nRadio Starting..."));
    Serial.print(F("Firmware version: "));
    Serial.println(FIRMWARE_VERSION);
#endif
    // Defer CPU frequency setting until after critical initialization
    // This allows boot to complete faster at default speed, then scale up
    
    initializeSubsystems();
    
    // Set CPU frequency after subsystems are initialized for better boot performance
    setCpuFrequencyMhz(240);

#ifdef DEBUG_SERIAL_OUTPUT
    Serial.print(F("CPU Frequency set to "));
    Serial.print(getCpuFrequencyMhz());
    Serial.println(F(" MHz"));

    // Simple system information
    const esp_partition_t* running = esp_ota_get_running_partition();
    if (running != NULL) {
      Serial.printf_P(PSTR("Running from partition: %s\n"), running->label);
    }
#endif
    initializeTasks();
  }

  void loop() {
    // Check power switch state
    PowerManager::getInstance().checkPowerSwitch();

    // If power switch is off, don't process anything else
    if (digitalRead(Pins::POWER_SWITCH) == LOW) {
      delay(10);  // Debounce delay
      return;
    }

    // Update power LED state
    PowerManager::getInstance().updatePowerLED();

    // Check for inactivity and manage power state
    PowerManager::getInstance().checkActivity();

    ts.execute();
    // Use yield() instead of delay(1) for better responsiveness and lower latency
    yield();
  }

  static void handleStationTuning(Station* station, int signalStrength) {
    auto& config = ConfigManager::getInstance();
    auto& audio = AudioManager::getInstance();
    auto& morse = MorseCode::getInstance();

    static Station* lastStation = nullptr;
    bool stationLocked = (signalStrength > 0);

    if (stationLocked) {
      if (!config.isMorsePlaying() || station != lastStation) {
        if (station != lastStation) {
          audio.stop();
        }
        morse.startMessage(station->getMessage());
        lastStation = station;
      }
    } else {
      if (config.isMorsePlaying()) {
        morse.stop();
        lastStation = nullptr;
      }
      audio.playStaticNoise(signalStrength);
    }

    morse.update();
  }

 private:
  void initializeSubsystems() {
    PowerManager::getInstance().begin();
    ConfigManager::getInstance().begin();
    AudioManager::getInstance().begin();
    StationManager::getInstance().begin();
    WaveBandManager::getInstance().begin();
    WiFiManager::getInstance().begin();
    MorseCode::getInstance().begin();
    SpeedManager::getInstance().begin();
    SignalManager::getInstance().begin();
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.println(F("All subsystems initialized"));
#endif
  }

  void initializeTasks() {
    ts.addTask(tBatteryCheck);
    ts.addTask(tSystemUpdate);

    tBatteryCheck.enable();
    tSystemUpdate.enable();
  }
};

// Task callback implementations
void batteryCheckCallback() {
  auto& power = PowerManager::getInstance();
  float voltage = power.getBatteryVoltage();

  // Force deep sleep if battery is critically low
  if (voltage <= PowerManager::MIN_BATTERY_VOLTAGE) {
    power.enterDeepSleep(PowerManager::SleepReason::BATTERY_CRITICAL);
    return;
  }

  // Let the LED task handle low battery indication
  power.updatePowerLED();

#ifdef DEBUG_SERIAL_OUTPUT
  Serial.printf_P(PSTR("Battery Voltage: %.2fV\n"), voltage);
#endif
}

void systemUpdateCallback() {
  static unsigned long lastButtonPress = 0;
  const unsigned long debounceTime = Timing::DEBOUNCE_DELAY;

  // Check for OTA boot sequence (3 WiFi button presses within 5 seconds of boot)
  PowerManager::getInstance().checkOTABootSequence();

  // Check WiFi toggle button with debounce
  handleWiFiButton(lastButtonPress, debounceTime);

  // Handle WiFi operations
  WiFiManager::getInstance().handle();

  // Skip morse radio functionality when WiFi is enabled
  if (!WiFiManager::getInstance().isEnabled()) {
    // Update tuning and find the closest station
    updateTuningAndStation();

    // Handle audio playback
    AudioManager::getInstance().handlePlayback();

    // Update wave band LEDs
    WaveBandManager::getInstance().updateLEDs();

    // Update morse speed
    SpeedManager::getInstance().update();
  }
}

// Helper functions to break down the systemUpdateCallback
void handleWiFiButton(unsigned long& lastButtonPress, const unsigned long debounceTime) {
  // Skip normal WiFi button handling during OTA boot window
  if (PowerManager::getInstance().isInOTABootWindow()) {
    return;
  }
  
  if (digitalRead(Pins::WIFI_BUTTON) == LOW) {
    unsigned long currentTime = millis();
    if (currentTime - lastButtonPress > debounceTime) {
      WiFiManager::getInstance().toggle();
      lastButtonPress = currentTime;
    }
  }
}

void updateTuningAndStation() {
  // Update wave band selection
  WaveBandManager::getInstance().update();

  // Read the tuning potentiometer
  int tuningValue = PowerManager::getInstance().readADC(Pins::TUNING_POT);
  int signalStrength = 0;

  // Get configuration and station managers
  auto& config = ConfigManager::getInstance();
  auto& stations = StationManager::getInstance();
  auto& signalMgr = SignalManager::getInstance();

  // Find the closest station based on tuning value and current wave band
  Station* closestStation =
      stations.findClosestStation(tuningValue, config.getWaveBand(), signalStrength);

  // Update signal indicators
  bool stationLocked = (signalStrength > 0);
  signalMgr.updateLockStatus(stationLocked);
  signalMgr.updateSignalStrength(signalStrength);

  // Debug output if enabled
#ifdef DEBUG_SERIAL_OUTPUT
  signalMgr.debugPrint(stationLocked, closestStation ? closestStation->getName() : nullptr,
                       signalStrength);
#endif

  // Handle station tuning and audio playback
  RadioSystem::handleStationTuning(closestStation, signalStrength);
}

// Global system instance
RadioSystem radioSystem;

void setup() { radioSystem.begin(); }

void loop() { radioSystem.loop(); }