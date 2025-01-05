#include <Arduino.h>
#include <TaskScheduler.h>
#include <UMS3.h>
#include <esp_sleep.h>

#include "AudioManager.h"
#include "Config.h"
#include "MorseCode.h"
#include "PowerManager.h"
#include "SpeedManager.h"
#include "StationManager.h"
#include "WaveBandManager.h"
#include "WiFiManager.h"

// Task Scheduler
Scheduler ts;

// Task forward declarations
void batteryCheckCallback();
void systemUpdateCallback();
void managerUpdateCallback();

// Tasks
Task tBatteryCheck(60000, TASK_FOREVER, &batteryCheckCallback);  // Check battery every minute
Task tSystemUpdate(20, TASK_FOREVER, &systemUpdateCallback);     // System update every 20ms
Task tManagerUpdate(20, TASK_FOREVER, &managerUpdateCallback);   // Manager updates every 20ms

// Main system manager class
class RadioSystem {
 public:
  void begin() {
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.begin(115200);
    Serial.println("\nRadio Starting...");
#endif
    setCpuFrequencyMhz(80);

#ifdef DEBUG_SERIAL_OUTPUT
    Serial.print("CPU Frequency set to ");
    Serial.print(getCpuFrequencyMhz());
    Serial.println(" MHz");
#endif
    initializeSubsystems();
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
    delay(1);  // Small delay to prevent overwhelming the CPU
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
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.println("All subsystems initialized");
#endif
  }

  void initializeTasks() {
    ts.addTask(tBatteryCheck);
    ts.addTask(tSystemUpdate);
    ts.addTask(tManagerUpdate);

    tBatteryCheck.enable();
    tSystemUpdate.enable();
    tManagerUpdate.enable();
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
  Serial.printf("Battery Voltage: %.2fV\n", voltage);
#endif
}

void systemUpdateCallback() {
  static unsigned long lastButtonPress = 0;
  const unsigned long debounceTime = Timing::DEBOUNCE_DELAY;

  // Check WiFi toggle button with debounce
  if (digitalRead(Pins::WIFI_BUTTON) == LOW) {
    unsigned long currentTime = millis();
    if (currentTime - lastButtonPress > debounceTime) {
      WiFiManager::getInstance().toggle();
      lastButtonPress = currentTime;
    }
  }

  // Update tuning
  WaveBandManager::getInstance().update();
  int tuningValue = analogRead(Pins::TUNING_POT);
  int signalStrength = 0;

  auto& config = ConfigManager::getInstance();
  auto& stations = StationManager::getInstance();
  Station* closestStation =
      stations.findClosestStation(tuningValue, config.getWaveBand(), signalStrength);

  RadioSystem::handleStationTuning(closestStation, signalStrength);
}

void managerUpdateCallback() {
  AudioManager::getInstance().handlePlayback();
  WiFiManager::getInstance().handle();
  WaveBandManager::getInstance().updateLEDs();
  SpeedManager::getInstance().update();
}

// Global system instance
RadioSystem radioSystem;

void setup() { radioSystem.begin(); }

void loop() { radioSystem.loop(); }