#include "PowerManager.h"
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

// Initialize potentiometers
PotentiometerReader tuningPot(Pins::TUNING_POT);
PotentiometerReader volumePot(Pins::VOLUME_POT);

void PowerManager::begin() {
  btStop();

  // Initialize UMS3
  ums3.begin();

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

  // Setup LED PWM channels for battery display
  ledcSetup(PWMChannels::LW_LED, LEDConfig::PWM_FREQUENCY, LEDConfig::PWM_RESOLUTION);
  ledcSetup(PWMChannels::MW_LED, LEDConfig::PWM_FREQUENCY, LEDConfig::PWM_RESOLUTION);
  ledcSetup(PWMChannels::SW_LED, LEDConfig::PWM_FREQUENCY, LEDConfig::PWM_RESOLUTION);

  ledcAttachPin(Pins::LW_LED, PWMChannels::LW_LED);
  ledcAttachPin(Pins::MW_LED, PWMChannels::MW_LED);
  ledcAttachPin(Pins::SW_LED, PWMChannels::SW_LED);

  // Display battery level for 2 seconds
  displayBatteryLevel();
  delay(1000);

  // Turn off all LEDs after battery display
  ledcWrite(PWMChannels::LW_LED, 0);
  ledcWrite(PWMChannels::MW_LED, 0);
  ledcWrite(PWMChannels::SW_LED, 0);

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
  float batteryPercent =
      constrain((cachedBatteryVoltage - LEDConfig::BATTERY_MIN_V) /
                    (LEDConfig::BATTERY_MAX_V - LEDConfig::BATTERY_MIN_V) * 100.0f,
                0.0f, 100.0f);
  uint8_t baseBrightness =
      (uint8_t)(LEDConfig::MAX_BRIGHTNESS * (0.2f + (batteryPercent / 100.0f) * 0.8f));
  lastBatteryReadTime = startTime;

  while (true) {
    bool usbConnected = powerManager->ums3.getVbusPresent();
    uint32_t currentTime = millis();

    // Update battery reading every 10 seconds
    if (currentTime - lastBatteryReadTime >= 10000) {
      cachedBatteryVoltage = powerManager->getBatteryVoltage();

      // Calculate base brightness from battery level (20-100% of max)
      batteryPercent = constrain((cachedBatteryVoltage - LEDConfig::BATTERY_MIN_V) /
                                     (LEDConfig::BATTERY_MAX_V - LEDConfig::BATTERY_MIN_V) * 100.0f,
                                 0.0f, 100.0f);

      baseBrightness =
          (uint8_t)(LEDConfig::MAX_BRIGHTNESS * (0.2f + (batteryPercent / 100.0f) * 0.8f));

      lastBatteryReadTime = currentTime;
    }

    if (usbConnected) {
      // USB is connected, pulse between battery level and 100%
      float progress = (float)((currentTime - startTime) % LEDConfig::PULSE_PERIOD_MS) /
                       LEDConfig::PULSE_PERIOD_MS;

      // Sine wave adjusted to pulse between base brightness and max
      float pulseRange = LEDConfig::MAX_BRIGHTNESS - baseBrightness;
      float brightness = baseBrightness + (pulseRange * (sin(progress * 2 * PI) + 1.0f) * 0.5f);

      ledcWrite(PWMChannels::POWER_LED, (uint8_t)brightness);
    } else {
      // USB not connected, show battery level
      // Critical battery level - flash the LED
      if (cachedBatteryVoltage <= LEDConfig::BATTERY_MIN_V) {
        if (currentTime - lastFlashTime >= 1000) {  // 1Hz flash rate
          flashState = !flashState;
          lastFlashTime = currentTime;
        }
        ledcWrite(PWMChannels::POWER_LED, flashState ? LEDConfig::MAX_BRIGHTNESS : 0);
      }
      // Normal battery operation - steady brightness indicates level
      else {
        ledcWrite(PWMChannels::POWER_LED, baseBrightness);
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
  // Configure power switch pin with internal pull-up
  rtc_gpio_init(static_cast<gpio_num_t>(Pins::POWER_SWITCH));
  rtc_gpio_set_direction(static_cast<gpio_num_t>(Pins::POWER_SWITCH), RTC_GPIO_MODE_INPUT_ONLY);
  rtc_gpio_pullup_en(static_cast<gpio_num_t>(Pins::POWER_SWITCH));
  rtc_gpio_pulldown_dis(static_cast<gpio_num_t>(Pins::POWER_SWITCH));
  rtc_gpio_hold_en(static_cast<gpio_num_t>(Pins::POWER_SWITCH));

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

  // Check potentiometers using filtered readings
  int currentTuning = readADC(Pins::TUNING_POT);
  int currentVolume = readADC(Pins::VOLUME_POT);

  if (abs(currentTuning - lastTuningValue) > POTENTIOMETER_THRESHOLD) {
    activity = true;
  }

  if (abs(currentVolume - lastVolumeValue) > POTENTIOMETER_THRESHOLD) {
    activity = true;
  }

  // Check digital inputs for state changes
  bool currentLWState = digitalRead(Pins::LW_BAND_SWITCH) == LOW;
  bool currentMWState = digitalRead(Pins::MW_BAND_SWITCH) == LOW;
  bool currentSlowState = digitalRead(Pins::SLOW_DECODE) == LOW;
  bool currentMedState = digitalRead(Pins::MED_DECODE) == LOW;
  bool currentWiFiState = digitalRead(Pins::WIFI_BUTTON) == LOW;

  if (currentLWState != lastLWState) {
    activity = true;
  }

  if (currentMWState != lastMWState) {
    activity = true;
  }

  if (currentSlowState != lastSlowState) {
    activity = true;
  }

  if (currentMedState != lastMedState) {
    activity = true;
  }

  if (currentWiFiState != lastWiFiState) {
    activity = true;
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

int PowerManager::readADC(int pin) {
  if (pin == Pins::TUNING_POT) {
    return tuningPot.read();
  } else if (pin == Pins::VOLUME_POT) {
    return volumePot.read();
  }
  return analogRead(pin);
}

float PowerManager::getBatteryVoltage() { return ums3.getBatteryVoltage(); }

bool PowerManager::isLowBattery() {
  float voltage = getBatteryVoltage();
  return voltage < LOW_BATTERY_THRESHOLD;
}

void PowerManager::displayBatteryStatus() {
  float voltage = getBatteryVoltage();
  bool isPluggedIn = ums3.getVbusPresent();

  ums3.setPixelBrightness(10);

  if (isPluggedIn) {
    ums3.setPixelBrightness(50);
  } else {
    ums3.setPixelBrightness(10);
  }
  // Normal battery status display
  if (voltage >= 4.0) {
    ums3.setPixelColor(0, 255, 0);  // Fully Charged - Green
  } else if (voltage >= 3.7) {
    ums3.setPixelColor(255, 255, 0);  // Moderately Charged - Yellow
  } else {
    ums3.setPixelColor(255, 0, 0);  // Low Battery - Red
  }
}

void PowerManager::displayBatteryLevel() {
  float voltage = getBatteryVoltage();

  // Calculate battery percentage (0-100%)
  float percentage = (voltage - MIN_BATTERY_VOLTAGE) / (MAX_BATTERY_VOLTAGE - MIN_BATTERY_VOLTAGE);
  percentage = constrain(percentage, 0.0f, 1.0f);

  // Calculate how many "full" LEDs we need
  int fullLEDs = floor(percentage * 3);

  // Calculate the PWM value for the partial LED
  float remainder = (percentage * 3) - fullLEDs;
  uint8_t partialBrightness = remainder * LEDConfig::MAX_BRIGHTNESS;

  // Set LED states based on battery level
  switch (fullLEDs) {
    case 0:
      // Battery 0-33%: Only bottom LED might be partially lit
      ledcWrite(PWMChannels::SW_LED, partialBrightness);
      ledcWrite(PWMChannels::MW_LED, 0);
      ledcWrite(PWMChannels::LW_LED, 0);
      break;

    case 1:
      // Battery 33-66%: Bottom LED full, middle LED might be partial
      ledcWrite(PWMChannels::SW_LED, LEDConfig::MAX_BRIGHTNESS);
      ledcWrite(PWMChannels::MW_LED, partialBrightness);
      ledcWrite(PWMChannels::LW_LED, 0);
      break;

    case 2:
      // Battery 66-99%: Bottom and middle LED full, top LED might be partial
      ledcWrite(PWMChannels::SW_LED, LEDConfig::MAX_BRIGHTNESS);
      ledcWrite(PWMChannels::MW_LED, LEDConfig::MAX_BRIGHTNESS);
      ledcWrite(PWMChannels::LW_LED, partialBrightness);
      break;

    case 3:
      // Battery 100%: All LEDs full
      ledcWrite(PWMChannels::SW_LED, LEDConfig::MAX_BRIGHTNESS);
      ledcWrite(PWMChannels::MW_LED, LEDConfig::MAX_BRIGHTNESS);
      ledcWrite(PWMChannels::LW_LED, LEDConfig::MAX_BRIGHTNESS);
      break;
  }
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
  if (!checkForInputChanges()) {
    unsigned long currentTime = millis();
    if (currentTime - lastActivityTime >= INACTIVITY_TIMEOUT) {
      // Only enter deep sleep if power switch is still ON
      if (digitalRead(Pins::POWER_SWITCH) == HIGH) {
        // enterDeepSleep(SleepReason::INACTIVITY);
      }
    }
  }
}
