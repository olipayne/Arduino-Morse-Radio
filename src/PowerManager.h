#ifndef POWERMANAGER_H
#define POWERMANAGER_H

#include <Arduino.h>
#include <UMS3.h>
#include "Config.h"
#include "WiFiManager.h"

class PowerManager
{
public:
    static PowerManager &getInstance()
    {
        static PowerManager instance;
        return instance;
    }

    void begin();
    void checkPowerSwitch();
    void checkActivity();
    float getBatteryVoltage();
    bool isLowBattery();
    bool isUSBPowered() { return ums3.getVbusPresent(); } // Added method to check USB power

private:
    PowerManager() = default;
    PowerManager(const PowerManager &) = delete;
    PowerManager &operator=(const PowerManager &) = delete;

    void configurePins();
    void configureADC();
    void updatePinStates();
    bool checkForInputChanges();
    void enterDeepSleep();
    void enterLightSleep();
    void displayBatteryStatus();
    void updatePowerIndicators(bool powerOn);

    // Constants
    static constexpr float LOW_BATTERY_THRESHOLD = 3.5;
    static constexpr unsigned long INACTIVITY_TIMEOUT = 300000; // 5 minutes
    static constexpr int POTENTIOMETER_THRESHOLD = 100;
    static constexpr uint8_t LED_PWM_RESOLUTION = 8;
    static constexpr uint32_t LED_PWM_FREQ = 5000;
    static constexpr uint8_t LED_BRIGHTNESS = 255;

    // State tracking
    unsigned long lastActivityTime = 0;
    int lastTuningValue = 0;
    int lastVolumeValue = 0;
    bool lastLWState = false;
    bool lastMWState = false;
    bool lastSlowState = false;
    bool lastMedState = false;
    bool lastWiFiState = false;

    // UMS3 instance
    UMS3 ums3;
};

#endif
