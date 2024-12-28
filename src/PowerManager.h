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
    void updatePinStates();
    void updatePowerIndicators(bool powerOn);
    int readADC(int pin);
    void pulseLED();
    void startLEDTask();
    void stopLEDTask();
    static void LEDTaskCode(void *parameter);
    void updateLEDBrightness(float batteryVoltage);
    void shutdownAllPins(); // New function to handle all pin shutdown
    void displayBatteryStatus();

    // RTC data structure that persists during deep sleep
    RTC_DATA_ATTR static bool inactivitySleep;

    // LED control constants
    static constexpr uint8_t LED_CHANNEL = 4;    // PWM channel for power LED
    static constexpr uint32_t LED_FREQ = 5000;   // PWM frequency
    static constexpr uint8_t LED_RESOLUTION = 8; // 8-bit resolution (0-255)
    static constexpr uint8_t MAX_BRIGHTNESS = 255;
    static constexpr uint8_t MIN_BRIGHTNESS = 10;
    static constexpr uint8_t CRITICAL_BRIGHTNESS = 5;
    static constexpr unsigned long PULSE_INTERVAL = 8;    // Reduced from 15ms to 8ms
    static constexpr uint8_t BRIGHTNESS_STEP = 2;         // Add step size for faster changes
    static constexpr unsigned long FLASH_INTERVAL = 1000; // ms between flashes for low battery

    // State tracking
    unsigned long lastActivityTime = 0;
    int lastTuningValue = 0;
    int lastVolumeValue = 0;
    bool lastLWState = false;
    bool lastMWState = false;
    bool lastSlowState = false;
    bool lastMedState = false;
    bool lastWiFiState = false;
    uint8_t currentBrightness = MAX_BRIGHTNESS;
    bool pulseIncreasing = true;
    unsigned long lastPulseUpdate = 0;
    unsigned long lastFlashUpdate = 0;
    TaskHandle_t ledTaskHandle = nullptr;
    volatile bool shouldPulse = false;

    // UMS3 instance
    UMS3 ums3;
};

#endif
