#include "PowerManager.h"
#include "driver/rtc_io.h"
#include "driver/gpio.h"

// PowerManager implementation
// The power switch is configured with:
// - Maximum drive strength pull-up
// - Input filtering for noise reduction
// - GPIO state holding during sleep
// - 100ms debounce time for stability

bool PowerManager::inactivitySleep = false;

void PowerManager::begin()
{
    btStop();

    // Initialize FeatherS3 specific features
    ums3.begin();

    // Set up PWM for power LED (do this early for proper shutdown)
    ledcSetup(LED_CHANNEL, LED_FREQ, LED_RESOLUTION);
    ledcAttachPin(Pins::POWER_LED, LED_CHANNEL);
    ledcWrite(LED_CHANNEL, 0);  // Start with LED off

    // If we're waking from inactivity sleep and switch is still ON, go back to sleep
    if (inactivitySleep && digitalRead(Pins::POWER_SWITCH) == HIGH) {
        enterDeepSleep(SleepReason::INACTIVITY);
        return;
    }
    
    // Clear inactivity flag since we're awake now
    inactivitySleep = false;

    // Enable the 3.3V 1 power supply
    ums3.setLDO2Power(true);

    // Configure pins with explicit pull-ups and set pin modes
    configurePins();

    // Turn on power LED at full brightness
    ledcWrite(LED_CHANNEL, MAX_BRIGHTNESS);

    // Start LED task
    startLEDTask();

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

void PowerManager::startLEDTask()
{
    if (ledTaskHandle == nullptr) {
        xTaskCreate(
            LEDTaskCode,        // Task function
            "LED_Task",         // Task name
            2048,              // Stack size
            this,              // Task parameters (this pointer)
            1,                 // Priority
            &ledTaskHandle     // Task handle
        );
    }
}

void PowerManager::stopLEDTask()
{
    if (ledTaskHandle != nullptr) {
        vTaskDelete(ledTaskHandle);
        ledTaskHandle = nullptr;
    }
}

void PowerManager::LEDTaskCode(void* parameter)
{
    PowerManager* powerManager = static_cast<PowerManager*>(parameter);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(PULSE_INTERVAL);

    while (true) {
        if (powerManager->shouldPulse) {
            if (powerManager->pulseIncreasing) {
                if (powerManager->currentBrightness <= MAX_BRIGHTNESS - BRIGHTNESS_STEP) {
                    powerManager->currentBrightness += BRIGHTNESS_STEP;
                } else {
                    powerManager->currentBrightness = MAX_BRIGHTNESS;
                    powerManager->pulseIncreasing = false;
                }
            } else {
                if (powerManager->currentBrightness >= MIN_BRIGHTNESS + BRIGHTNESS_STEP) {
                    powerManager->currentBrightness -= BRIGHTNESS_STEP;
                } else {
                    powerManager->currentBrightness = MIN_BRIGHTNESS;
                    powerManager->pulseIncreasing = true;
                }
            }
            ledcWrite(LED_CHANNEL, powerManager->currentBrightness);
        }
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void PowerManager::updatePowerLED()
{
    if (!digitalRead(Pins::POWER_SWITCH)) {
        shouldPulse = false;
        ledcWrite(LED_CHANNEL, 0);
        return;
    }

    float voltage = getBatteryVoltage();
    
    // If plugged in and fully charged, solid LED
    if (isUSBPowered() && voltage >= FULLY_CHARGED_THRESHOLD) {
        shouldPulse = false;
        ledcWrite(LED_CHANNEL, MAX_BRIGHTNESS);
        return;
    }

    // If plugged in but not fully charged, pulse
    if (isUSBPowered()) {
        shouldPulse = true;
        return;
    }

    // On battery power, brightness indicates level
    shouldPulse = false;
    updateLEDBrightness(voltage);
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
            enterDeepSleep(SleepReason::POWER_OFF);
        }
    }
}

void PowerManager::updatePowerIndicators(bool powerOn)
{
    digitalWrite(Pins::BACKLIGHT, powerOn ? HIGH : LOW);
    digitalWrite(Pins::POWER_LED, powerOn ? HIGH : LOW);
}

void PowerManager::shutdownAllPins()
{
    // Stop morse code output
    MorseCode::getInstance().stop();

    // Turn off all LEDs
    ledcWrite(LED_CHANNEL, 0);          // Power LED
    digitalWrite(Pins::BACKLIGHT, LOW);
    digitalWrite(Pins::LW_LED, LOW);
    digitalWrite(Pins::MW_LED, LOW);
    digitalWrite(Pins::SW_LED, LOW);
    digitalWrite(Pins::LOCK_LED, LOW);
    digitalWrite(Pins::MORSE_LEDS, LOW);

    // Turn off all PWM channels
    ledcWrite(PWMChannels::AUDIO, 0);   // Audio output
    ledcWrite(PWMChannels::DECODE, 0);  // Decode speed control
    ledcWrite(PWMChannels::CARRIER, 0); // Signal strength indicator

    // Disable power supply
    ums3.setLDO2Power(false);
}

void PowerManager::enterDeepSleep(SleepReason reason)
{
#ifdef DEBUG_SERIAL_OUTPUT
    Serial.println("Entering deep sleep mode...");
#endif

    // Shutdown all pins and peripherals
    shutdownAllPins();

    if (reason == SleepReason::INACTIVITY) {
        // Set flag in RTC memory
        inactivitySleep = true;
        // Wake when power switch goes low (turned off)
        esp_sleep_enable_ext0_wakeup(static_cast<gpio_num_t>(Pins::POWER_SWITCH), 0);
    } else {
        // Wake when power switch goes high (turned on)
        esp_sleep_enable_ext0_wakeup(static_cast<gpio_num_t>(Pins::POWER_SWITCH), 1);
    }

    // Enter deep sleep
    esp_deep_sleep_start();
}

void PowerManager::checkActivity()
{
    if (!checkForInputChanges()) {
        unsigned long currentTime = millis();
        if (currentTime - lastActivityTime >= INACTIVITY_TIMEOUT) {
            // Only enter deep sleep if power switch is still ON
            if (digitalRead(Pins::POWER_SWITCH) == HIGH) {
#ifdef DEBUG_SERIAL_OUTPUT
                Serial.println("Inactivity timeout - entering deep sleep");
#endif
                enterDeepSleep(SleepReason::INACTIVITY);
            }
        }
    }
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

void PowerManager::updateLEDBrightness(float batteryVoltage)
{
    unsigned long currentTime = millis();
    
    // Critical battery level - flash the LED
    if (batteryVoltage <= LOW_BATTERY_THRESHOLD) {
        if (currentTime - lastFlashUpdate >= FLASH_INTERVAL) {
            lastFlashUpdate = currentTime;
            currentBrightness = (currentBrightness == 0) ? CRITICAL_BRIGHTNESS : 0;
            ledcWrite(LED_CHANNEL, currentBrightness);
        }
        return;
    }
    
    // Normal battery operation - brightness indicates level
    float voltageRange = MAX_BATTERY_VOLTAGE - MIN_BATTERY_VOLTAGE;
    float normalizedVoltage = (batteryVoltage - MIN_BATTERY_VOLTAGE) / voltageRange;
    normalizedVoltage = constrain(normalizedVoltage, 0.0f, 1.0f);
    
    uint8_t brightness = MIN_BRIGHTNESS + (normalizedVoltage * (MAX_BRIGHTNESS - MIN_BRIGHTNESS));
    if (brightness != currentBrightness) {
        currentBrightness = brightness;
        ledcWrite(LED_CHANNEL, currentBrightness);
    }
}
