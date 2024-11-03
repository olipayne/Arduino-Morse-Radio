#include "PowerManager.h"

void PowerManager::begin()
{
    btStop();

    // Initialize FeatherS3 specific features
    ums3.begin();

    // Check if we're waking from deep sleep
    if (checkWakeupCause())
    {
        // We just woke up, reset all our states
        lastActivityTime = millis();
    }

    // Configure pins with explicit pull-ups and set pin modes
    configurePins();

    // Configure ADC for power efficiency
    configureADC();

    // Initialize last known values
    lastTuningValue = analogRead(Pins::TUNING_POT);
    lastVolumeValue = analogRead(Pins::VOLUME_POT);

    // Initialize digital pin states with debounce
    updatePinStates();
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
    bool activity = false;

    // Check potentiometers
    int currentTuning = analogRead(Pins::TUNING_POT);
    int currentVolume = analogRead(Pins::VOLUME_POT);

    if (abs(currentTuning - lastTuningValue) > POTENTIOMETER_THRESHOLD)
    {
        activity = true;
        Serial.printf("Tuning change: %d -> %d\n", lastTuningValue, currentTuning);
    }

    if (abs(currentVolume - lastVolumeValue) > POTENTIOMETER_THRESHOLD)
    {
        activity = true;
        Serial.printf("Volume change: %d -> %d\n", lastVolumeValue, currentVolume);
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
        Serial.println("LW switch changed");
    }

    if (currentMWState != lastMWState)
    {
        activity = true;
        Serial.println("MW switch changed");
    }

    if (currentSlowState != lastSlowState)
    {
        activity = true;
        Serial.println("Slow decode changed");
    }

    if (currentMedState != lastMedState)
    {
        activity = true;
        Serial.println("Med decode changed");
    }

    if (currentWiFiState != lastWiFiState)
    {
        activity = true;
        Serial.println("WiFi button changed");
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
    static unsigned long lastDebounceTime = 0;
    const unsigned long debounceDelay = 100; // 100ms debounce

    if (WiFiManager::getInstance().isEnabled())
    {
        lastActivityTime = millis();
        return;
    }

    // Add debounce for input checks
    if (millis() - lastDebounceTime > debounceDelay)
    {
        if (checkForInputChanges())
        {
            lastActivityTime = millis();
            lastDebounceTime = millis();
            return;
        }
    }

    // Check if we've been inactive for too long
    unsigned long currentTime = millis();
    unsigned long inactiveTime = (currentTime - lastActivityTime) / 1000;

    // Print remaining time every minute
    if (inactiveTime > 0 && inactiveTime % 10 == 0)
    {
        unsigned long timeToSleep = (INACTIVITY_TIMEOUT - (currentTime - lastActivityTime)) / 1000;
        Serial.printf("Inactive for %lu seconds. Sleep in %lu seconds...\n",
                      inactiveTime, timeToSleep);
    }

    if (currentTime - lastActivityTime >= INACTIVITY_TIMEOUT)
    {
        Serial.println("No activity detected for some time, entering light sleep...");
        delay(100);
        enterLightSleep();
    }
}

void PowerManager::enterLightSleep()
{
    Serial.println("Preparing for light sleep...");
    delay(100);

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
            Serial.printf("Failed to configure wakeup on pin %d: %s\n", pin, esp_err_to_name(result));
        }
    }

    // Prepare for light sleep
    Serial.flush(); // Ensure all serial output is sent
    esp_light_sleep_start();

    // Execution resumes here after wake-up
    Serial.println("Woke up from light sleep");

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

bool PowerManager::checkWakeupCause()
{
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

    if (wakeup_reason != ESP_SLEEP_WAKEUP_UNDEFINED)
    {
        printWakeupCause();
        return true;
    }
    return false;
}

void PowerManager::printWakeupCause()
{
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

    Serial.print("Wakeup cause: ");
    switch (wakeup_reason)
    {
    case ESP_SLEEP_WAKEUP_EXT0:
        Serial.println("External signal using RTC_IO");
        break;
    case ESP_SLEEP_WAKEUP_EXT1:
    {
        uint64_t wakeup_pin_mask = esp_sleep_get_ext1_wakeup_status();
        if (wakeup_pin_mask != 0)
        {
            int pin = __builtin_ffsll(wakeup_pin_mask) - 1;
            Serial.printf("External signal using RTC_CNTL on pin %d\n", pin);
        }
        else
        {
            Serial.println("External signal using RTC_CNTL (pin not identified)");
        }
    }
    break;
    case ESP_SLEEP_WAKEUP_TIMER:
        Serial.println("Timer");
        break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
        Serial.println("Touchpad");
        break;
    case ESP_SLEEP_WAKEUP_ULP:
        Serial.println("ULP program");
        break;
    default:
        Serial.printf("Unknown reason %d\n", wakeup_reason);
        break;
    }
}

void PowerManager::configurePins()
{
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