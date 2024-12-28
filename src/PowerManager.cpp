#include "PowerManager.h"
#include "driver/rtc_io.h"
#include "driver/gpio.h"

void PowerManager::begin()
{
    btStop();

    // Initialize FeatherS3 specific features
    ums3.begin();

    // Enable the 3.3V 1 power supply
    ums3.setLDO2Power(true);

    // Configure pins with explicit pull-ups and set pin modes
    configurePins();

    // Check power switch state immediately and update indicators
    bool powerOn = (digitalRead(Pins::POWER_SWITCH) == HIGH);
    updatePowerIndicators(powerOn);

    // Configure ADC for power efficiency
    configureADC();

    // Initialize last known values
    lastTuningValue = readADC(Pins::TUNING_POT);
    lastVolumeValue = readADC(Pins::VOLUME_POT);

    // Initialize digital pin states with debounce
    updatePinStates();
}

void PowerManager::checkPowerSwitch()
{
    // Add debouncing for power switch with longer debounce time
    static unsigned long lastDebounceTime = 0;
    static int lastPowerState = -1;
    const unsigned long debounceDelay = 100;  // Increased to 100ms for more stability

    int currentPowerState = gpio_get_level(static_cast<gpio_num_t>(Pins::POWER_SWITCH));
    unsigned long currentTime = millis();

    // If the state has changed, reset debounce timer
    if (currentPowerState != lastPowerState) {
        lastDebounceTime = currentTime;
        lastPowerState = currentPowerState;
        return;
    }

    // Only act if debounce time has passed
    if ((currentTime - lastDebounceTime) > debounceDelay) {
        bool powerOn = (currentPowerState == HIGH);
        updatePowerIndicators(powerOn);

        // If power switch is pulled low (switched off)
        if (!powerOn) {
            // Disable the 3.3V 1 power supply
            ums3.setLDO2Power(false);
            // Enter deep sleep immediately
            enterDeepSleep();
        }
    }
}

void PowerManager::updatePowerIndicators(bool powerOn)
{
    digitalWrite(Pins::BACKLIGHT, powerOn ? HIGH : LOW);
    digitalWrite(Pins::POWER_LED, powerOn ? HIGH : LOW);
}

void PowerManager::enterDeepSleep()
{
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.println("Entering deep sleep mode...");
#endif
    // Turn off power indicators
    updatePowerIndicators(false);

    // Disable WiFi and BT to prevent any interference
    WiFi.mode(WIFI_OFF);
    btStop();

    // Configure wake-up source with specific settings for stability
    gpio_num_t wakeupPin = static_cast<gpio_num_t>(Pins::POWER_SWITCH);
    
    // Configure for wake-up
    gpio_hold_dis(wakeupPin);
    gpio_deep_sleep_hold_dis();
    
    // Set up the pin for wake-up
    gpio_pad_select_gpio(static_cast<uint32_t>(wakeupPin));
    gpio_set_direction(wakeupPin, GPIO_MODE_INPUT);
    gpio_pullup_en(wakeupPin);
    gpio_pulldown_dis(wakeupPin);
    
    // Set wake-up source to detect HIGH level (when switch is turned ON)
    esp_sleep_enable_ext0_wakeup(wakeupPin, HIGH);
    
    // Keep RTC peripherals on and configure for stability
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_OFF);
    
    // Hold the GPIO state during sleep
    gpio_hold_en(wakeupPin);
    gpio_deep_sleep_hold_en();
    
    // Enter deep sleep
    esp_deep_sleep_start();
}

void PowerManager::configureADC()
{
    // Set ADC resolution to maximum
    analogReadResolution(12);
    
    // Configure ADC for better stability
    adcAttachPin(Pins::TUNING_POT);
    adcAttachPin(Pins::VOLUME_POT);
    
    // Set attenuation for the full voltage range (0-3.3V)
    analogSetPinAttenuation(Pins::TUNING_POT, ADC_11db);
    analogSetPinAttenuation(Pins::VOLUME_POT, ADC_11db);
}

int PowerManager::readADC(int pin)
{
    const int samples = 5;
    int readings[samples];
    
    // Take multiple samples with a small delay between them
    for(int i = 0; i < samples; i++) {
        readings[i] = analogRead(pin);
        delay(2); // Small delay between readings
    }
    
    // Sort readings (insertion sort for small array)
    for(int i = 1; i < samples; i++) {
        int key = readings[i];
        int j = i - 1;
        while(j >= 0 && readings[j] > key) {
            readings[j + 1] = readings[j];
            j--;
        }
        readings[j + 1] = key;
    }
    
    // Return median value (middle reading)
    return readings[samples / 2];
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

    // Check potentiometers using filtered readings
    int currentTuning = readADC(Pins::TUNING_POT);
    int currentVolume = readADC(Pins::VOLUME_POT);

    if (abs(currentTuning - lastTuningValue) > POTENTIOMETER_THRESHOLD)
    {
        activity = true;
#ifdef DEBUG_SERIAL_OUTPUT
        Serial.printf("Tuning change: %d -> %d\n\r", lastTuningValue, currentTuning);
#endif
    }

    if (abs(currentVolume - lastVolumeValue) > POTENTIOMETER_THRESHOLD)
    {
        activity = true;
#ifdef DEBUG_SERIAL_OUTPUT
        Serial.printf("Volume change: %d -> %d\n\r", lastVolumeValue, currentVolume);
#endif
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
#ifdef DEBUG_SERIAL_OUTPUT
        Serial.println("LW switch changed");
#endif
    }

    if (currentMWState != lastMWState)
    {
        activity = true;
#ifdef DEBUG_SERIAL_OUTPUT
        Serial.println("MW switch changed");
#endif
    }

    if (currentSlowState != lastSlowState)
    {
        activity = true;
#ifdef DEBUG_SERIAL_OUTPUT
        Serial.println("Slow decode changed");
#endif
    }

    if (currentMedState != lastMedState)
    {
        activity = true;
#ifdef DEBUG_SERIAL_OUTPUT
        Serial.println("Med decode changed");
#endif
    }

    if (currentWiFiState != lastWiFiState)
    {
        activity = true;
#ifdef DEBUG_SERIAL_OUTPUT
        Serial.println("WiFi button changed");
#endif
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
#ifdef DEBUG_SERIAL_OUTPUT
        displayBatteryStatus();
#endif
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
        Serial.printf("Inactive for %lu seconds. Sleep in %lu seconds...\n\r",
                      inactiveTime, timeToSleep);
        lastPrintTime = currentTime;
    }

    if (currentTime - lastActivityTime >= INACTIVITY_TIMEOUT)
    {
#ifdef DEBUG_SERIAL_OUTPUT
        Serial.println("No activity detected for some time, entering light sleep...");
#endif
        delay(100);
        enterLightSleep();
    }
}

void PowerManager::enterLightSleep()
{
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.println("Preparing for light sleep...");
#endif
    // Turn off power indicators
    updatePowerIndicators(false);

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
#ifdef DEBUG_SERIAL_OUTPUT
            Serial.printf("Failed to configure wakeup on pin %d: %s\n\r", pin, esp_err_to_name(result));
#endif
        }
    }

    // Prepare for light sleep
    Serial.flush(); // Ensure all serial output is sent

    // Disable the 3.3V 1 power supply
    ums3.setLDO2Power(false);

    // Enter light sleep
    esp_light_sleep_start();

    // Execution resumes here after wake-up
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.println("Woke up from light sleep");
#endif

    // Re-enable the 3.3V 1 power supply
    ums3.setLDO2Power(true);

    // Restore power indicators based on power switch state
    bool powerOn = (digitalRead(Pins::POWER_SWITCH) == HIGH);
    updatePowerIndicators(powerOn);

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
    // Configure power switch pin with both pull-up and input filtering
    gpio_config_t powerSwitchConfig = {
        .pin_bit_mask = (1ULL << Pins::POWER_SWITCH),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&powerSwitchConfig);

    // Set maximum drive strength for the power switch pin
    gpio_set_drive_capability(static_cast<gpio_num_t>(Pins::POWER_SWITCH), GPIO_DRIVE_CAP_3);
    
    // Enable internal input filter to reduce noise
    REG_SET_BIT(GPIO_PIN_MUX_REG[Pins::POWER_SWITCH], FUN_IE);
    
    // Configure the pin for wake-up with strong pull-up
    gpio_hold_dis(static_cast<gpio_num_t>(Pins::POWER_SWITCH));
    gpio_deep_sleep_hold_dis();
    
    // Configure for RTC wake-up
    gpio_pad_select_gpio(static_cast<uint32_t>(Pins::POWER_SWITCH));
    gpio_set_direction(static_cast<gpio_num_t>(Pins::POWER_SWITCH), GPIO_MODE_INPUT);
    gpio_pullup_en(static_cast<gpio_num_t>(Pins::POWER_SWITCH));
    gpio_pulldown_dis(static_cast<gpio_num_t>(Pins::POWER_SWITCH));
    gpio_hold_en(static_cast<gpio_num_t>(Pins::POWER_SWITCH));

    // Configure input pins
    pinMode(Pins::LW_BAND_SWITCH, INPUT_PULLUP);
    pinMode(Pins::MW_BAND_SWITCH, INPUT_PULLUP);
    pinMode(Pins::SLOW_DECODE, INPUT_PULLUP);
    pinMode(Pins::MED_DECODE, INPUT_PULLUP);
    pinMode(Pins::WIFI_BUTTON, INPUT_PULLUP);

    // Configure output pins
    pinMode(Pins::BACKLIGHT, OUTPUT);
    pinMode(Pins::POWER_LED, OUTPUT);
    digitalWrite(Pins::BACKLIGHT, LOW);
    digitalWrite(Pins::POWER_LED, LOW);

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

    Serial.printf("Battery voltage: %.2fV\n\r", voltage);

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
