#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include <Arduino.h>
#include <esp_sleep.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <WiFi.h>
#include "Config.h"
#include "WiFiManager.h"
#include <UMS3.h>

class PowerManager
{
public:
    static PowerManager &getInstance()
    {
        static PowerManager instance;
        return instance;
    }

    void begin();
    float getBatteryVoltage();
    bool isLowBattery();
    void checkActivity();

    int getCurrentCpuFreq() const { return getCpuFrequencyMhz(); }

private:
    PowerManager() : ums3(UMS3()) {}
    PowerManager(const PowerManager &) = delete;
    PowerManager &operator=(const PowerManager &) = delete;

    void configureADC();
    void enterLightSleep();
    bool checkForInputChanges();
    void updatePinStates();

    UMS3 ums3;                                            // FeatherS3 helper
    static constexpr float LOW_BATTERY_THRESHOLD = 3.4f;  // Volts
    static constexpr uint32_t INACTIVITY_TIMEOUT = 30000; // 30 seconds
    static constexpr int POTENTIOMETER_THRESHOLD = 100;   // Minimum change to count as activity

    // Last known values for activity detection
    int lastTuningValue = 0;
    int lastVolumeValue = 0;

    // Last known states of digital inputs
    bool lastLWState = false;
    bool lastMWState = false;
    bool lastSlowState = false;
    bool lastMedState = false;
    bool lastWiFiState = false;

    unsigned long lastActivityTime = 0;

    // Check if we just woke from deep sleep
    bool checkWakeupCause();
    void printWakeupCause();
    void configurePins(); // New pin configuration function
};

#endif