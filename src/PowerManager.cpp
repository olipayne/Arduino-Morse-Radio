#include "PowerManager.h"
#include "OTAConfig.h"
#include "OTAManager.h"
#include "PotentiometerReader.h"  // Include PotentiometerReader header
#include "driver/gpio.h"
#include "driver/rtc_io.h"

// PowerManager implementation
// The power switch is configured with:
// - Maximum drive strength pull-up
// - Input filtering for noise reduction
// - GPIO state holding during sleep
// - 500ms debounce time for stability

static RTC_DATA_ATTR bool justWentToSleep = false;  // Flag to indicate we just went to sleep
static RTC_DATA_ATTR bool rtcInitialized = false;   // Flag to track if RTC GPIO is initialized

// Initialize potentiometers
PotentiometerReader tuningPot(Pins::TUNING_POT);
PotentiometerReader volumePot(Pins::VOLUME_POT);

void PowerManager::begin() {
  // Stop Bluetooth if running (quick check, low overhead if already stopped)
  btStop();

  // Record boot time for OTA sequence detection
  bootTime = millis();
  
  // Initialize activity timer to current time (so timeout starts from boot)
  resetActivityTimer("System Boot");

  // Initialize UMS3
  ums3.begin();

  // One-time RTC GPIO initialization
  if (!rtcInitialized) {
    rtc_gpio_init(static_cast<gpio_num_t>(Pins::POWER_SWITCH));
    rtc_gpio_set_direction(static_cast<gpio_num_t>(Pins::POWER_SWITCH), RTC_GPIO_MODE_INPUT_ONLY);
    rtc_gpio_pullup_en(static_cast<gpio_num_t>(Pins::POWER_SWITCH));
    rtc_gpio_pulldown_dis(static_cast<gpio_num_t>(Pins::POWER_SWITCH));
    rtc_gpio_hold_en(static_cast<gpio_num_t>(Pins::POWER_SWITCH));
    rtcInitialized = true;
  }

  // If we're waking up right after going to sleep, go back to sleep
  if (justWentToSleep) {
    justWentToSleep = false;  // Clear the flag for next wake
    enterDeepSleep(SleepReason::POWER_OFF);
    return;
  }

  // Configure pins
  configurePins();

  // Initialize potentiometers
  tuningPot.begin();
  volumePot.begin();

  // Setup LED PWM channel for power LED
  ledcSetup(PWMChannels::POWER_LED, LEDConfig::PWM_FREQUENCY, LEDConfig::PWM_RESOLUTION);
  ledcAttachPin(Pins::POWER_LED, PWMChannels::POWER_LED);

  // Turn off all LEDs
  digitalWrite(Pins::LW_LED, LOW);
  digitalWrite(Pins::MW_LED, LOW);
  digitalWrite(Pins::SW_LED, LOW);
  ledcWrite(PWMChannels::POWER_LED, 0);

  // Start LED task
  startLEDTask();

  // Initial power indicator update
  updatePowerIndicators(true);

  // Enable the 3.3V 1 power supply
  ums3.setLDO2Power(true);

  // Configure ADC for power efficiency
  configureADC();

  // Initialize last known values
  lastTuningValue = readADC(Pins::TUNING_POT);
  lastVolumeValue = readADC(Pins::VOLUME_POT);

  // Initialize digital pin states with debounce
  updatePinStates();
}

void PowerManager::startLEDTask() {
  if (ledTaskHandle == nullptr) {
    xTaskCreate(LEDTaskCode,    // Task function
                "LED_Task",     // Task name
                4096,           // Stack size
                this,           // Task parameters (this pointer)
                1,              // Priority
                &ledTaskHandle  // Task handle
    );
  }
}

void PowerManager::stopLEDTask() {
  if (ledTaskHandle != nullptr) {
    vTaskDelete(ledTaskHandle);
    ledTaskHandle = nullptr;
  }
}

void PowerManager::LEDTaskCode(void* parameter) {
  // Get pointer to PowerManager instance to access ums3
  PowerManager* powerManager = static_cast<PowerManager*>(parameter);

  // Initialize PWM for LED
  ledcSetup(PWMChannels::POWER_LED, LEDConfig::PWM_FREQUENCY, LEDConfig::PWM_RESOLUTION);
  ledcAttachPin(Pins::POWER_LED, PWMChannels::POWER_LED);
  ledcWrite(PWMChannels::POWER_LED, 0);  // Start with LED off

  uint32_t startTime = millis();
  const TickType_t xFrequency = pdMS_TO_TICKS(20);  // 50Hz update rate
  TickType_t xLastWakeTime = xTaskGetTickCount();
  uint32_t lastFlashTime = 0;
  uint32_t lastBatteryReadTime = 0;
  bool flashState = false;

  // Get initial battery reading
  float cachedBatteryVoltage = powerManager->getBatteryVoltage();
  // Use LiPo discharge curve for accurate percentage
  float batteryPercent = PowerManager::voltageToPercent(cachedBatteryVoltage);
  uint8_t baseBrightness =
      (uint8_t)(LEDConfig::MAX_BRIGHTNESS * (0.2f + (batteryPercent / 100.0f) * 0.8f));
  lastBatteryReadTime = startTime;

  // Cache previous values to only update LED when they change
  bool lastUsbConnected = powerManager->ums3.getVbusPresent();
  uint8_t lastBrightness = 0;
  bool lastFlashState = false;

  while (true) {
    bool usbConnected = powerManager->ums3.getVbusPresent();
    uint32_t currentTime = millis();
    bool needsUpdate = false;
    uint8_t newBrightness = lastBrightness;

    // Update battery reading every 10 seconds
    if (currentTime - lastBatteryReadTime >= 10000) {
      float newVoltage = powerManager->getBatteryVoltage();
      // Only recalculate if voltage changed significantly (save CPU cycles)
      if (abs(newVoltage - cachedBatteryVoltage) > 0.01f) {
        cachedBatteryVoltage = newVoltage;

        // Calculate base brightness from battery level using LiPo discharge curve (20-100% of max)
        batteryPercent = PowerManager::voltageToPercent(cachedBatteryVoltage);

        baseBrightness =
            (uint8_t)(LEDConfig::MAX_BRIGHTNESS * (0.2f + (batteryPercent / 100.0f) * 0.8f));
        needsUpdate = true;
      }
      lastBatteryReadTime = currentTime;
    }

    // Only recalculate if USB state changed or update is needed
    if (usbConnected != lastUsbConnected) {
      lastUsbConnected = usbConnected;
      needsUpdate = true;
    }

    if (usbConnected) {
      // USB is connected, pulse between battery level and 100%
      // Only update brightness every 100ms during pulse to save CPU (still appears smooth)
      static uint32_t lastPulseUpdate = 0;
      if (needsUpdate || (currentTime - lastPulseUpdate >= 100)) {
        float progress = (float)((currentTime - startTime) % LEDConfig::PULSE_PERIOD_MS) /
                         LEDConfig::PULSE_PERIOD_MS;

        // Sine wave adjusted to pulse between base brightness and max
        float pulseRange = LEDConfig::MAX_BRIGHTNESS - baseBrightness;
        newBrightness = baseBrightness + (pulseRange * (sin(progress * 2 * PI) + 1.0f) * 0.5f);
        if (newBrightness != lastBrightness) {
          ledcWrite(PWMChannels::POWER_LED, newBrightness);
          lastBrightness = newBrightness;
        }
        lastPulseUpdate = currentTime;
      }
    } else {
      // USB not connected, show battery level
      // Critical battery level - flash the LED
      if (cachedBatteryVoltage <= LEDConfig::BATTERY_MIN_V) {
        if (currentTime - lastFlashTime >= 1000) {  // 1Hz flash rate
          flashState = !flashState;
          lastFlashTime = currentTime;
          newBrightness = flashState ? LEDConfig::MAX_BRIGHTNESS : 0;
          if (newBrightness != lastBrightness) {
            ledcWrite(PWMChannels::POWER_LED, newBrightness);
            lastBrightness = newBrightness;
            lastFlashState = flashState;
          }
        }
      }
      // Normal battery operation - steady brightness indicates level
      else {
        newBrightness = baseBrightness;
        if (newBrightness != lastBrightness) {
          ledcWrite(PWMChannels::POWER_LED, newBrightness);
          lastBrightness = newBrightness;
        }
      }
    }

    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void PowerManager::updatePowerIndicators(bool powerOn) {
  if (powerOn) {
    digitalWrite(Pins::BACKLIGHT, HIGH);
  } else {
    digitalWrite(Pins::BACKLIGHT, LOW);
    digitalWrite(Pins::LW_LED, LOW);
    digitalWrite(Pins::MW_LED, LOW);
    digitalWrite(Pins::SW_LED, LOW);
    digitalWrite(Pins::LOCK_LED, LOW);
  }
}

void PowerManager::updatePowerLED() {
  // This function is now handled by the LED task
}

void PowerManager::shutdownAllPins() {
  // Turn off all output pins except Power LED (handled by LED task)
  digitalWrite(Pins::BACKLIGHT, LOW);
  digitalWrite(Pins::LW_LED, LOW);
  digitalWrite(Pins::MW_LED, LOW);
  digitalWrite(Pins::SW_LED, LOW);
  digitalWrite(Pins::LOCK_LED, LOW);
}

void PowerManager::configurePins() {
  // Configure LED pins
  pinMode(Pins::POWER_LED, OUTPUT);  // Power LED will be configured in LED task
  pinMode(Pins::LW_LED, OUTPUT);
  pinMode(Pins::MW_LED, OUTPUT);
  pinMode(Pins::SW_LED, OUTPUT);
  pinMode(Pins::LOCK_LED, OUTPUT);
  pinMode(Pins::BACKLIGHT, OUTPUT);

  // Configure input pins
  pinMode(Pins::LW_BAND_SWITCH, INPUT_PULLUP);
  pinMode(Pins::MW_BAND_SWITCH, INPUT_PULLUP);
  pinMode(Pins::SLOW_DECODE, INPUT_PULLUP);
  pinMode(Pins::MED_DECODE, INPUT_PULLUP);
  pinMode(Pins::WIFI_BUTTON, INPUT_PULLUP);

  // Configure ADC pins
  pinMode(Pins::TUNING_POT, INPUT);
  pinMode(Pins::VOLUME_POT, INPUT);

  // Initialize LED states (except Power LED)
  digitalWrite(Pins::LW_LED, LOW);
  digitalWrite(Pins::MW_LED, LOW);
  digitalWrite(Pins::SW_LED, LOW);
  digitalWrite(Pins::LOCK_LED, LOW);
  digitalWrite(Pins::BACKLIGHT, LOW);
}

void PowerManager::updatePinStates() {
  lastLWState = digitalRead(Pins::LW_BAND_SWITCH) == LOW;
  lastMWState = digitalRead(Pins::MW_BAND_SWITCH) == LOW;
  lastSlowState = digitalRead(Pins::SLOW_DECODE) == LOW;
  lastMedState = digitalRead(Pins::MED_DECODE) == LOW;
  lastWiFiState = digitalRead(Pins::WIFI_BUTTON) == LOW;

  // Update LED states based on current mode
  WaveBand currentBand = ConfigManager::getInstance().getWaveBand();
  digitalWrite(Pins::LW_LED, currentBand == WaveBand::LONG_WAVE);
  digitalWrite(Pins::MW_LED, currentBand == WaveBand::MEDIUM_WAVE);
  digitalWrite(Pins::SW_LED, currentBand == WaveBand::SHORT_WAVE);
}

bool PowerManager::checkForInputChanges() {
  // First check power switch state
  checkPowerSwitch();

  bool activity = false;
  const char* activityReason = nullptr;

  // Update potentiometer values but don't track them as activity
  // (station changes are tracked explicitly in main.cpp)
  int currentTuning = readADC(Pins::TUNING_POT);
  int currentVolume = readADC(Pins::VOLUME_POT);

  // Check digital inputs for state changes
  bool currentLWState = digitalRead(Pins::LW_BAND_SWITCH) == LOW;
  bool currentMWState = digitalRead(Pins::MW_BAND_SWITCH) == LOW;
  bool currentSlowState = digitalRead(Pins::SLOW_DECODE) == LOW;
  bool currentMedState = digitalRead(Pins::MED_DECODE) == LOW;
  bool currentWiFiState = digitalRead(Pins::WIFI_BUTTON) == LOW;

  if (currentLWState != lastLWState) {
    activity = true;
    activityReason = "Long Wave Switch";
  }

  if (currentMWState != lastMWState) {
    activity = true;
    activityReason = "Medium Wave Switch";
  }

  if (currentSlowState != lastSlowState) {
    activity = true;
    activityReason = "Slow Decode Switch";
  }

  if (currentMedState != lastMedState) {
    activity = true;
    activityReason = "Medium Decode Switch";
  }

  if (currentWiFiState != lastWiFiState) {
    activity = true;
    activityReason = "WiFi Button";
  }

  // Update stored states
  lastTuningValue = currentTuning;
  lastVolumeValue = currentVolume;
  lastLWState = currentLWState;
  lastMWState = currentMWState;
  lastSlowState = currentSlowState;
  lastMedState = currentMedState;
  lastWiFiState = currentWiFiState;

  // Reset timer with reason if activity was detected
  if (activity && activityReason != nullptr) {
    resetActivityTimer(activityReason);
  }

  return activity;
}

int PowerManager::readADC(int pin) {
  if (pin == Pins::TUNING_POT) {
    return tuningPot.read();
  } else if (pin == Pins::VOLUME_POT) {
    return volumePot.read();
  }
  return analogRead(pin);
}

int PowerManager::readADCRaw(int pin) {
  if (pin == Pins::TUNING_POT) {
    return tuningPot.readRaw();
  } else if (pin == Pins::VOLUME_POT) {
    return volumePot.readRaw();
  }
  return analogRead(pin);
}

float PowerManager::getBatteryVoltage() { return ums3.getBatteryVoltage(); }

float PowerManager::getBatteryPercent() { return voltageToPercent(getBatteryVoltage()); }

// LiPo discharge curve lookup tables (defined in header as constexpr)
constexpr float PowerManager::LIPO_VOLTAGE_TABLE[];
constexpr float PowerManager::LIPO_PERCENT_TABLE[];

float PowerManager::voltageToPercent(float voltage) {
  // Handle edge cases
  if (voltage >= LIPO_VOLTAGE_TABLE[0]) {
    return 100.0f;
  }
  if (voltage <= LIPO_VOLTAGE_TABLE[LIPO_CURVE_POINTS - 1]) {
    return 0.0f;
  }
  
  // Find the two points to interpolate between
  // Table is sorted descending (4.2V -> 3.2V)
  for (int i = 0; i < LIPO_CURVE_POINTS - 1; i++) {
    if (voltage <= LIPO_VOLTAGE_TABLE[i] && voltage > LIPO_VOLTAGE_TABLE[i + 1]) {
      // Linear interpolation between the two points
      float voltageRange = LIPO_VOLTAGE_TABLE[i] - LIPO_VOLTAGE_TABLE[i + 1];
      float percentRange = LIPO_PERCENT_TABLE[i] - LIPO_PERCENT_TABLE[i + 1];
      float voltageOffset = LIPO_VOLTAGE_TABLE[i] - voltage;
      float percentOffset = (voltageOffset / voltageRange) * percentRange;
      return LIPO_PERCENT_TABLE[i] - percentOffset;
    }
  }
  
  // Fallback (shouldn't reach here)
  return 0.0f;
}

bool PowerManager::isLowBattery() {
  float voltage = getBatteryVoltage();
  return voltage < LEDConfig::BATTERY_LOW_V;
}

void PowerManager::displayBatteryStatus() {
  float voltage = getBatteryVoltage();
  float percent = voltageToPercent(voltage);
  bool isPluggedIn = ums3.getVbusPresent();

  ums3.setPixelBrightness(10);

  if (isPluggedIn) {
    ums3.setPixelBrightness(50);
  } else {
    ums3.setPixelBrightness(10);
  }
  // Battery status display based on actual percentage
  // Using LiPo curve: >60% = Green, 25-60% = Yellow, <25% = Red
  if (percent >= 60.0f) {
    ums3.setPixelColor(0, 255, 0);  // Good charge - Green
  } else if (percent >= 25.0f) {
    ums3.setPixelColor(255, 255, 0);  // Moderate charge - Yellow
  } else {
    ums3.setPixelColor(255, 0, 0);  // Low Battery - Red
  }
}

void PowerManager::displayBatteryLevel() {
  float voltage = getBatteryVoltage();
  float percent = voltageToPercent(voltage);

  // Turn off all LEDs first
  digitalWrite(Pins::LW_LED, LOW);
  digitalWrite(Pins::MW_LED, LOW);
  digitalWrite(Pins::SW_LED, LOW);

  // Display battery level using binary LEDs based on actual percentage
  // >66% = 3 LEDs, 33-66% = 2 LEDs, 10-33% = 1 LED, <10% = 0 LEDs
  if (percent >= 66.0f) {
    // High battery - all LEDs on
    digitalWrite(Pins::LW_LED, HIGH);
    digitalWrite(Pins::MW_LED, HIGH);
    digitalWrite(Pins::SW_LED, HIGH);
  } else if (percent >= 33.0f) {
    // Medium battery - two LEDs on
    digitalWrite(Pins::LW_LED, HIGH);
    digitalWrite(Pins::MW_LED, HIGH);
  } else if (percent >= 10.0f) {
    // Low battery - one LED on
    digitalWrite(Pins::LW_LED, HIGH);
  }
  // Below 10% - all LEDs off (critical)
}

void PowerManager::enterDeepSleep(SleepReason reason) {
  // Prepare for sleep
  shutdownAllPins();

  // If entering sleep due to low battery, flash LW LED
  if (reason == SleepReason::BATTERY_CRITICAL) {
    // Flash LW LED 10 times quickly
    for (int i = 0; i < 10; i++) {
      digitalWrite(Pins::LW_LED, HIGH);
      delay(50);  // 50ms on
      digitalWrite(Pins::LW_LED, LOW);
      delay(50);  // 50ms off
    }
  }

  // Configure wake-up on GPIO with pull-up (wake on button press - LOW)
  esp_sleep_enable_ext0_wakeup(static_cast<gpio_num_t>(Pins::POWER_SWITCH), 0);

  // Enter deep sleep
  esp_deep_sleep_start();
}

void PowerManager::checkPowerSwitch() {
  bool switchPressed =
      !rtc_gpio_get_level(static_cast<gpio_num_t>(Pins::POWER_SWITCH));  // Active low with pull-up

  // If momentary switch is pressed (connected to ground), go to sleep
  if (switchPressed) {
    justWentToSleep = true;  // Set flag before sleeping
    enterDeepSleep(SleepReason::POWER_OFF);
  }
}

void PowerManager::configureADC() {
  // Set ADC resolution to maximum
  analogReadResolution(12);

  // Configure ADC for better stability
  adcAttachPin(Pins::TUNING_POT);
  adcAttachPin(Pins::VOLUME_POT);

  // Set attenuation for the full voltage range (0-3.3V)
  analogSetPinAttenuation(Pins::TUNING_POT, ADC_11db);
  analogSetPinAttenuation(Pins::VOLUME_POT, ADC_11db);
}

void PowerManager::checkActivity() {
  // Check for input changes (will reset timer internally if activity detected)
  checkForInputChanges();
  
  // Check if timeout has elapsed
  unsigned long currentTime = millis();
  // Get timeout from ConfigManager and convert minutes to milliseconds
  unsigned long inactivityTimeoutMs = ConfigManager::getInstance().getInactivityTimeout() * 60000UL;
  if (currentTime - lastActivityTime >= inactivityTimeoutMs) {
    // Only enter deep sleep if power switch is still ON
    if (digitalRead(Pins::POWER_SWITCH) == HIGH) {
#ifndef DEBUG_SERIAL_OUTPUT
      // Enable deep sleep on inactivity for release builds only
#ifdef DEBUG_SERIAL_OUTPUT
      Serial.println(F("Entering deep sleep due to inactivity"));
#endif
      enterDeepSleep(SleepReason::INACTIVITY);
#else
      // In debug mode, just log the timeout
      static bool timeoutLogged = false;
      if (!timeoutLogged) {
        Serial.println(F("Inactivity timeout reached (deep sleep disabled in debug mode)"));
        timeoutLogged = true;
      }
#endif
    }
  }
}

void PowerManager::resetActivityTimer(const char* reason) {
  lastActivityTime = millis();
  
#ifdef DEBUG_SERIAL_OUTPUT
  if (reason != nullptr) {
    Serial.print(F("Activity timer reset: "));
    Serial.println(reason);
  } else {
    Serial.println(F("Activity timer reset"));
  }
#endif
}

void PowerManager::checkOTABootSequence() {
  if (!isInOTABootWindow()) {
    return;
  }

  // Check if WiFi button is pressed
  if (digitalRead(Pins::WIFI_BUTTON) == LOW) {
    unsigned long currentTime = millis();

    // Debounce the button press
    if (currentTime - lastWiFiButtonPress > BUTTON_DEBOUNCE) {
      wifiButtonPressCount++;
      lastWiFiButtonPress = currentTime;

#ifdef DEBUG_SERIAL_OUTPUT
      Serial.printf("WiFi button press %d/3\n", wifiButtonPressCount);
#endif

      // If we've detected 3 presses, trigger OTA update
      if (wifiButtonPressCount >= 3) {
#ifdef DEBUG_SERIAL_OUTPUT
        Serial.println(F("OTA boot sequence detected! Starting update..."));
#endif

        // Configure WiFi credentials from OTAConfig
        auto& otaManager = OTAManager::getInstance();
        otaManager.clearWiFiCredentials();

        // Add all configured WiFi networks
        for (size_t i = 0; i < OTAConfig::WIFI_NETWORK_COUNT; i++) {
          otaManager.addWiFiCredentials(OTAConfig::WIFI_NETWORKS[i].ssid,
                                        OTAConfig::WIFI_NETWORKS[i].password);
        }

        // Start the update process
        OTAManager::UpdateResult result = otaManager.checkAndUpdate();

        // Handle the result
        switch (result) {
          case OTAManager::UpdateResult::SUCCESS:
            // Device will reboot automatically
            break;
          case OTAManager::UpdateResult::NO_UPDATE_NEEDED:
#ifdef DEBUG_SERIAL_OUTPUT
            Serial.println(F("No update needed"));
#endif
            break;
          case OTAManager::UpdateResult::WIFI_FAILED:
#ifdef DEBUG_SERIAL_OUTPUT
            Serial.println(F("WiFi connection failed"));
#endif
            // Flash all LEDs to indicate failure
            for (int i = 0; i < 5; i++) {
              digitalWrite(Pins::LW_LED, HIGH);
              digitalWrite(Pins::MW_LED, HIGH);
              digitalWrite(Pins::SW_LED, HIGH);
              delay(200);
              digitalWrite(Pins::LW_LED, LOW);
              digitalWrite(Pins::MW_LED, LOW);
              digitalWrite(Pins::SW_LED, LOW);
              delay(200);
            }
            break;
          case OTAManager::UpdateResult::VERSION_CHECK_FAILED:
          case OTAManager::UpdateResult::DOWNLOAD_FAILED:
          case OTAManager::UpdateResult::INSTALL_FAILED:
          case OTAManager::UpdateResult::NETWORK_ERROR:
#ifdef DEBUG_SERIAL_OUTPUT
            Serial.println(F("OTA update failed"));
#endif
            // Flash error pattern
            for (int i = 0; i < 10; i++) {
              digitalWrite(Pins::LW_LED, HIGH);
              delay(100);
              digitalWrite(Pins::LW_LED, LOW);
              delay(100);
            }
            break;
        }

        // Reset the counter to prevent multiple attempts
        wifiButtonPressCount = 0;
      }
    }
  }
}

bool PowerManager::isInOTABootWindow() const { return (millis() - bootTime) < OTA_BOOT_WINDOW; }
