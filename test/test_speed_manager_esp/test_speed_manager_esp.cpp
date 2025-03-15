#include <unity.h>
#include "SpeedManagerESP.h"

// Mock ESP-IDF functions
extern "C" {
esp_err_t gpio_config(const gpio_config_t* cfg) { return ESP_OK; }

esp_err_t gpio_set_level(gpio_num_t gpio_num, uint32_t level) { return ESP_OK; }

int gpio_get_level_values[GPIO_NUM_MAX] = {1};  // Default to HIGH
int gpio_get_level(gpio_num_t gpio_num) { return gpio_get_level_values[gpio_num]; }

esp_err_t ledc_timer_config(const ledc_timer_config_t* timer_conf) { return ESP_OK; }

esp_err_t ledc_channel_config(const ledc_channel_config_t* channel_conf) { return ESP_OK; }

uint32_t ledc_duty_value = 0;
esp_err_t ledc_set_duty(ledc_mode_t mode, ledc_channel_t channel, uint32_t duty) {
  ledc_duty_value = duty;
  return ESP_OK;
}

esp_err_t ledc_update_duty(ledc_mode_t mode, ledc_channel_t channel) { return ESP_OK; }
}

// Mock ConfigManager
MorseSpeed currentSpeed = MorseSpeed::MEDIUM;

class MockConfigManager {
 public:
  static MockConfigManager& getInstance() {
    static MockConfigManager instance;
    return instance;
  }

  void setMorseSpeed(MorseSpeed speed) { currentSpeed = speed; }

  MorseSpeed getMorseSpeed() const { return currentSpeed; }
};

ConfigManager& ConfigManager::getInstance() {
  return reinterpret_cast<ConfigManager&>(MockConfigManager::getInstance());
}

void setUp(void) {
  // Reset state before each test
  for (int i = 0; i < GPIO_NUM_MAX; i++) {
    gpio_get_level_values[i] = 1;  // Set all pins to HIGH (not pressed)
  }
  ledc_duty_value = 0;
  currentSpeed = MorseSpeed::MEDIUM;
}

void tearDown(void) {
  // Clean up after each test
}

void test_speed_manager_esp_default_is_medium(void) {
  // Default state (no switches pressed) should be MEDIUM speed
  gpio_get_level_values[Pins::SLOW_DECODE] = 1;  // HIGH (not pressed)
  gpio_get_level_values[Pins::MED_DECODE] = 1;   // HIGH (not pressed)

  SpeedManagerESP::getInstance().update();

  // For MEDIUM speed, we expect 171 (2.2V - 67% of 3.3V)
  TEST_ASSERT_EQUAL(171, ledc_duty_value);
}

void test_speed_manager_esp_slow_switch(void) {
  // SLOW switch pressed should set SLOW speed
  gpio_get_level_values[Pins::SLOW_DECODE] = 0;  // LOW (pressed)
  gpio_get_level_values[Pins::MED_DECODE] = 1;   // HIGH (not pressed)

  SpeedManagerESP::getInstance().update();

  // For SLOW speed, we expect 85 (1.1V - 33% of 3.3V)
  TEST_ASSERT_EQUAL(85, ledc_duty_value);
}

void test_speed_manager_esp_med_switch_selects_fast(void) {
  // MED switch pressed should set FAST speed (swapped logic)
  gpio_get_level_values[Pins::SLOW_DECODE] = 1;  // HIGH (not pressed)
  gpio_get_level_values[Pins::MED_DECODE] = 0;   // LOW (pressed)

  SpeedManagerESP::getInstance().update();

  // For FAST speed, we expect 255 (3.3V - 100% of 3.3V)
  TEST_ASSERT_EQUAL(255, ledc_duty_value);
}

void test_speed_manager_esp_both_switches_slow_takes_precedence(void) {
  // If both switches are pressed, SLOW should take precedence
  gpio_get_level_values[Pins::SLOW_DECODE] = 0;  // LOW (pressed)
  gpio_get_level_values[Pins::MED_DECODE] = 0;   // LOW (pressed)

  SpeedManagerESP::getInstance().update();

  // For SLOW speed, we expect 85 (1.1V - 33% of 3.3V)
  TEST_ASSERT_EQUAL(85, ledc_duty_value);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_speed_manager_esp_default_is_medium);
  RUN_TEST(test_speed_manager_esp_slow_switch);
  RUN_TEST(test_speed_manager_esp_med_switch_selects_fast);
  RUN_TEST(test_speed_manager_esp_both_switches_slow_takes_precedence);

  return UNITY_END();
}