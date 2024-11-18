#include "PowerManager.h"

void PowerManager::begin()
{
    btStop();

    // Initialize FeatherS3 specific features
    ums3.begin();

    // Enable the 3.3V 1 power supply
    ums3.setLDO2Power(true);

    // Configure pins with explicit pull-ups and set pin modes
    configurePins();

    // Check power switch state immediately
    checkPowerSwitch();

    // Configure ADC for power efficiency
    configureADC();

    // Initialize last known values
    lastTuningValue = analogRead(Pins::TUNING_POT);
    lastVolumeValue = analogRead(Pins::VOLUME_POT);

    // Initialize digital pin states with debounce
    updatePinStates();
}

void PowerManager::checkPowerSwitch()
{
    // If power switch is pulled high (switched on)
    if (digitalRead(Pins::POWER_SWITCH) == HIGH)
    {
        // Disable the 3.3V 1 power supply
        ums3.setLDO2Power(false);

        // Enter deep sleep
        enterDeepSleep();
    }
}

void PowerManager::enterDeepSleep()
{
    Log::println("Entering deep sleep mode...");

    // Configure wake-up on power switch high (when switch is turned back on)
    esp_sleep_enable_ext0_wakeup(static_cast<gpio_num_t>(Pins::POWER_SWITCH), LOW);

    // Enter deep sleep
    esp_deep_sleep_start();
}

void PowerManager::configureADC()
{
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);
    analogSetPinAttenuation(Pins::TUNING_POT, ADC_11db);
    analogSetPinAttenuation(Pins::VOLUME_POT, ADC_11db);
}

void PowerManager::updatePinStates()
{
    lastLWState = digitalRead(Pins::LW_BAND_SWITCH) == LOW;
    lastMWState = digitalRead(Pins::MW_BAND_SWITCH) == LOW;
    lastSlowState = digitalRead(Pins::SLOW_DECODE) == LOW;
    lastMedState = digitalRead(Pins::MED_DECODE) == LOW;
    lastWiFiState = digitalRead(Pins::WIFI_BUTTON) == LOW;
}

bool PowerManager::checkForInputChanges()
{
    // First check power switch state
    checkPowerSwitch();

    bool activity = false;

    // Check potentiometers
    int currentTuning = analogRead(Pins::TUNING_POT);
    int currentVolume = analogRead(Pins::VOLUME_POT);

    if (abs(currentTuning - lastTuningValue) > POTENTIOMETER_THRESHOLD)
    {
        activity = true;
        Log::println("Tuning change: ", lastTuningValue, " -> ", currentTuning);
    }

    if (abs(currentVolume - lastVolumeValue) > POTENTIOMETER_THRESHOLD)
    {
        activity = true;
        Log::println("Volume change: ", lastVolumeValue, " -> ", currentVolume);
    }

    // Check digital inputs for state changes
    bool currentLWState = digitalRead(Pins::LW_BAND_SWITCH) == LOW;
    bool currentMWState = digitalRead(Pins::MW_BAND_SWITCH) == LOW;
    bool currentSlowState = digitalRead(Pins::SLOW_DECODE) == LOW;
    bool currentMedState = digitalRead(Pins::MED_DECODE) == LOW;
    bool currentWiFiState = digitalRead(Pins::WIFI_BUTTON) == LOW;

    if (currentLWState != lastLWState)
    {
        activity = true;
        Log::println("LW switch changed");
    }

    if (currentMWState != lastMWState)
    {
        activity = true;
        Log::println("MW switch changed");
    }

    if (currentSlowState != lastSlowState)
    {
        activity = true;
        Log::println("Slow decode changed");
    }

    if (currentMedState != lastMedState)
    {
        activity = true;
        Log::println("Med decode changed");
    }

    if (currentWiFiState != lastWiFiState)
    {
        activity = true;
        Log::println("WiFi button changed");
    }

    // Update stored states
    lastTuningValue = currentTuning;
    lastVolumeValue = currentVolume;
    lastLWState = currentLWState;
    lastMWState = currentMWState;
    lastSlowState = currentSlowState;
    lastMedState = currentMedState;
    lastWiFiState = currentWiFiState;

    return activity;
}

void PowerManager::checkActivity()
{
    // First check power switch state
    checkPowerSwitch();

    static unsigned long lastDebounceTime = 0;
    const unsigned long debounceDelay = 100; // 100ms debounce

    static unsigned long lastDisplayTime = 0;   // Track last display time
    const unsigned long displayInterval = 5000; // 5 seconds

    unsigned long currentTime = millis();

    // Only display battery status every 5 seconds
    if (currentTime - lastDisplayTime >= displayInterval)
    {
        displayBatteryStatus();
        lastDisplayTime = currentTime;
    }

    if (WiFiManager::getInstance().isEnabled())
    {
        lastActivityTime = millis();
        return;
    }

    // Add debounce for input checks
    if (currentTime - lastDebounceTime > debounceDelay)
    {
        if (checkForInputChanges())
        {
            lastActivityTime = millis();
            lastDebounceTime = millis();
            return;
        }
    }

    // Check if we've been inactive for too long
    unsigned long inactiveTime = (currentTime - lastActivityTime) / 1000;

    // Print remaining time every minute, but only print it once, not every time it loops in a single second
    static unsigned long lastPrintTime = 0;
    if (inactiveTime > 0 && inactiveTime % 60 == 0 && currentTime - lastPrintTime >= 50000)
    {
        unsigned long timeToSleep = (INACTIVITY_TIMEOUT - (currentTime - lastActivityTime)) / 1000;
        Log::println("Inactive for ", inactiveTime, " seconds. Sleep in ", timeToSleep, " seconds...");
        lastPrintTime = currentTime;
    }

    if (currentTime - lastActivityTime >= INACTIVITY_TIMEOUT)
    {
        Log::println("No activity detected for some time, entering light sleep...");
        enterLightSleep();
    }
}

void PowerManager::enterLightSleep()
{
    Log::println("Preparing for light sleep...");

    // Enable GPIO wakeup
    esp_sleep_enable_gpio_wakeup();

    // List of pins to monitor
    const gpio_num_t monitoredPins[] = {
        static_cast<gpio_num_t>(Pins::LW_BAND_SWITCH),
        static_cast<gpio_num_t>(Pins::MW_BAND_SWITCH),
        static_cast<gpio_num_t>(Pins::SLOW_DECODE),
        static_cast<gpio_num_t>(Pins::MED_DECODE),
        static_cast<gpio_num_t>(Pins::WIFI_BUTTON)};

    // Configure per-pin wakeup triggers based on opposite of current state
    for (const auto &pin : monitoredPins)
    {
        // Read current level
        int pinLevel = gpio_get_level(pin);

        // Set wakeup trigger to opposite level
        gpio_int_type_t wakeupMode = (pinLevel == 1) ? GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL;
        esp_err_t result = gpio_wakeup_enable(pin, wakeupMode);
        if (result != ESP_OK)
        {
            Log::println("Failed to configure wakeup on pin ", static_cast<int>(pin), ": ", esp_err_to_name(result));
        }
    }

    // Prepare for light sleep
    Serial.flush(); // Ensure all serial output is sent

    // Disable the 3.3V 1 power supply
    ums3.setLDO2Power(false);

    // Enter light sleep
    esp_light_sleep_start();

    // Execution resumes here after wake-up
    Log::println("Woke up from light sleep");

    // Re-enable the 3.3V 1 power supply
    ums3.setLDO2Power(true);

    // Reinitialize any peripherals if necessary
    lastActivityTime = millis();
    setCpuFrequencyMhz(80);
}

float PowerManager::getBatteryVoltage()
{
    return ums3.getBatteryVoltage();
}

bool PowerManager::isLowBattery()
{
    float voltage = getBatteryVoltage();
    return voltage < LOW_BATTERY_THRESHOLD;
}

void PowerManager::configurePins()
{
    // Configure power switch pin
    pinMode(Pins::POWER_SWITCH, INPUT_PULLUP);
    gpio_set_direction(static_cast<gpio_num_t>(Pins::POWER_SWITCH), GPIO_MODE_INPUT);
    gpio_pullup_en(static_cast<gpio_num_t>(Pins::POWER_SWITCH));
    gpio_pulldown_dis(static_cast<gpio_num_t>(Pins::POWER_SWITCH));

    // Configure input pins
    pinMode(Pins::LW_BAND_SWITCH, INPUT_PULLUP);
    pinMode(Pins::MW_BAND_SWITCH, INPUT_PULLUP);
    pinMode(Pins::SLOW_DECODE, INPUT_PULLUP);
    pinMode(Pins::MED_DECODE, INPUT_PULLUP);
    pinMode(Pins::WIFI_BUTTON, INPUT_PULLUP);

    // Small delay to let pins settle
    delay(10);

    // Configure RTC GPIO for wake capabilities
    gpio_set_direction(static_cast<gpio_num_t>(Pins::LW_BAND_SWITCH), GPIO_MODE_INPUT);
    gpio_set_direction(static_cast<gpio_num_t>(Pins::MW_BAND_SWITCH), GPIO_MODE_INPUT);
    gpio_set_direction(static_cast<gpio_num_t>(Pins::SLOW_DECODE), GPIO_MODE_INPUT);
    gpio_set_direction(static_cast<gpio_num_t>(Pins::MED_DECODE), GPIO_MODE_INPUT);
    gpio_set_direction(static_cast<gpio_num_t>(Pins::WIFI_BUTTON), GPIO_MODE_INPUT);

    // Enable pull-ups on RTC GPIO
    gpio_pullup_en(static_cast<gpio_num_t>(Pins::LW_BAND_SWITCH));
    gpio_pullup_en(static_cast<gpio_num_t>(Pins::MW_BAND_SWITCH));
    gpio_pullup_en(static_cast<gpio_num_t>(Pins::SLOW_DECODE));
    gpio_pullup_en(static_cast<gpio_num_t>(Pins::MED_DECODE));
    gpio_pullup_en(static_cast<gpio_num_t>(Pins::WIFI_BUTTON));

    // Disable pull-downs to prevent conflicts
    gpio_pulldown_dis(static_cast<gpio_num_t>(Pins::LW_BAND_SWITCH));
    gpio_pulldown_dis(static_cast<gpio_num_t>(Pins::MW_BAND_SWITCH));
    gpio_pulldown_dis(static_cast<gpio_num_t>(Pins::SLOW_DECODE));
    gpio_pulldown_dis(static_cast<gpio_num_t>(Pins::MED_DECODE));
    gpio_pulldown_dis(static_cast<gpio_num_t>(Pins::WIFI_BUTTON));
}

void PowerManager::displayBatteryStatus()
{
    float voltage = getBatteryVoltage();
    bool isPluggedIn = ums3.getVbusPresent();

    Log::println("Battery voltage: ", voltage, "V");

    ums3.setPixelBrightness(10);

    if (isPluggedIn)
    {
        ums3.setPixelBrightness(50);
    }
    else
    {
        ums3.setPixelBrightness(10);
    }
    // Normal battery status display
    if (voltage >= 4.0)
    {
        ums3.setPixelColor(0, 255, 0); // Fully Charged - Green
    }
    else if (voltage >= 3.7)
    {
        ums3.setPixelColor(255, 255, 0); // Moderately Charged - Yellow
    }
    else
    {
        ums3.setPixelColor(255, 0, 0); // Low Battery - Red
    }
}
