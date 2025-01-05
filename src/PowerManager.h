#ifndef POWERMANAGER_H
#define POWERMANAGER_H

#include <Arduino.h>
#include <UMS3.h>
#include "Config.h"
#include "WiFiManager.h"
#include "MorseCode.h"

class PowerManager
{
public:
    static PowerManager &getInstance()
    {
        static PowerManager instance;
        return instance;
    }

    // Sleep reason enum
    enum class SleepReason
    {
        POWER_OFF,
        INACTIVITY,
        BATTERY_CRITICAL
    };

    // Constants
    static constexpr float LOW_BATTERY_THRESHOLD = 3.3;         // LiPo should not go below 3.3V
    static constexpr float MAX_BATTERY_VOLTAGE = 4.2;           // Fully charged LiPo
    static constexpr float FULLY_CHARGED_THRESHOLD = 4.15;      // Very close to full charge
    static constexpr float MIN_BATTERY_VOLTAGE = 3.2;           // Absolute minimum - protect battery
    static constexpr unsigned long INACTIVITY_TIMEOUT = 300000; // 5 minutes
    static constexpr int POTENTIOMETER_THRESHOLD = 100;

    // Public methods
    void begin();
    void checkPowerSwitch();
    void checkActivity();
    bool checkForInputChanges();
    float getBatteryVoltage();
    bool isLowBattery();
    bool isUSBPowered() { return ums3.getVbusPresent(); }
    void updatePowerLED();
    void enterDeepSleep(SleepReason reason = SleepReason::POWER_OFF);

private:
    PowerManager() = default;
    PowerManager(const PowerManager &) = delete;
    PowerManager &operator=(const PowerManager &) = delete;

    void configurePins();
    void configureADC();
    int readADC(int pin);
    void startLEDTask();
    void stopLEDTask();
    static void LEDTaskCode(void *parameter);
    void displayBatteryStatus();
    void updateLEDBrightness(float batteryVoltage);
    void shutdownAllPins();
    void updatePinStates();
    void updatePowerIndicators(bool powerOn);

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

    // Hardware instance
    UMS3 ums3;
};

#endif
