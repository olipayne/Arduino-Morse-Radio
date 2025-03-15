#ifdef ESP_PLATFORM
// ESP-IDF includes
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#else
// Arduino includes
#include "Arduino.h"
#endif

// Our includes
#include "Config.h"
#include "SpeedManager.h"
#include "Station.h"
#include "WaveBandManager.h"

static const char* TAG = "Radio";

#ifdef ESP_PLATFORM
extern "C" void app_main(void)
#else
void setup()
#endif
{
  ESP_LOGI(TAG, "Radio starting up...");

  // Initialize managers
  SpeedManager::getInstance().begin();
  WaveBandManager::getInstance().begin();

#ifdef ESP_PLATFORM
  // ESP-IDF main loop
  while (1) {
    SpeedManager::getInstance().update();
    WaveBandManager::getInstance().update();
    vTaskDelay(pdMS_TO_TICKS(100));  // 100ms delay
  }
#endif
}

#ifndef ESP_PLATFORM
void loop() {
  SpeedManager::getInstance().update();
  WaveBandManager::getInstance().update();
  delay(100);  // 100ms delay
}
#endif