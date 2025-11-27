#ifndef POWERMANAGER_H
#define POWERMANAGER_H

#include <Arduino.h>
#include <UMS3.h>
#include "Config.h"
#include "MorseCode.h"
#include "PotentiometerReader.h"

class PowerManager {
 public:
  static PowerManager& getInstance() {
    static PowerManager instance;
    return instance;
  }

  // Sleep reason enum
  enum class SleepReason { POWER_OFF, INACTIVITY, BATTERY_CRITICAL };

  // Constants
  static constexpr int POTENTIOMETER_THRESHOLD = 100;

  // Public methods
  void begin();
  void checkPowerSwitch();
  void checkActivity();
  bool checkForInputChanges();
  void resetActivityTimer(const char* reason = nullptr);
  float getBatteryVoltage();
  float getBatteryPercent();  // Returns battery percentage using LiPo discharge curve
  bool isLowBattery();
  bool isUSBPowered() { return ums3.getVbusPresent(); }
  void updatePowerLED();
  void displayBatteryLevel();
  void enterDeepSleep(SleepReason reason = SleepReason::POWER_OFF);
  int readADC(int pin);
  int readADCRaw(int pin);

  // LED task control methods (moved from private)
  void startLEDTask();
  void stopLEDTask();

  // OTA update functionality
  void checkOTABootSequence();
  bool isInOTABootWindow() const;

 private:
  PowerManager() : tuningPot(Pins::TUNING_POT), volumePot(Pins::VOLUME_POT) {}
  PowerManager(const PowerManager&) = delete;
  PowerManager& operator=(const PowerManager&) = delete;

  void configurePins();
  void configureADC();
  static void LEDTaskCode(void* parameter);
  void displayBatteryStatus();
  void updateLEDBrightness(float batteryVoltage);
  void shutdownAllPins();
  void updatePinStates();
  void updatePowerIndicators(bool powerOn);
  
  // LiPo battery percentage calculation using discharge curve lookup table
  static float voltageToPercent(float voltage);
  
  // LiPo discharge curve lookup table (voltage -> percentage)
  // Based on typical single-cell LiPo discharge characteristics
  static constexpr int LIPO_CURVE_POINTS = 12;
  static constexpr float LIPO_VOLTAGE_TABLE[LIPO_CURVE_POINTS] = {
    4.20f, 4.15f, 4.10f, 4.00f, 3.90f, 3.80f,
    3.70f, 3.60f, 3.50f, 3.40f, 3.30f, 3.20f
  };
  static constexpr float LIPO_PERCENT_TABLE[LIPO_CURVE_POINTS] = {
    100.0f, 95.0f, 90.0f, 80.0f, 65.0f, 50.0f,
    35.0f, 20.0f, 12.0f, 6.0f, 2.0f, 0.0f
  };

  // Task handle for LED control
  TaskHandle_t ledTaskHandle = nullptr;

  // State variables
  bool shouldPulse = false;
  static bool inactivitySleep;
  unsigned long lastActivityTime = 0;
  int lastTuningValue = 0;
  int lastVolumeValue = 0;
  uint8_t currentBrightness = 0;
  unsigned long lastFlashUpdate = 0;

  // Button states
  bool lastLWState = false;
  bool lastMWState = false;
  bool lastSlowState = false;
  bool lastMedState = false;
  bool lastWiFiState = false;

  // Potentiometer readers
  PotentiometerReader tuningPot;
  PotentiometerReader volumePot;

  // Hardware instance
  UMS3 ums3;

  // OTA boot sequence detection
  unsigned long bootTime = 0;
  int wifiButtonPressCount = 0;
  unsigned long lastWiFiButtonPress = 0;
  static constexpr unsigned long OTA_BOOT_WINDOW = 5000;  // 5 seconds
  static constexpr unsigned long BUTTON_DEBOUNCE = 200;   // 200ms debounce
};

#endif
