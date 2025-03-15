#ifndef SPEEDMANAGER_ESP_H
#define SPEEDMANAGER_ESP_H

#include "Config.h"
#include "ConfigManager.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"

class SpeedManagerESP {
 public:
  static SpeedManagerESP& getInstance() {
    static SpeedManagerESP instance;
    return instance;
  }

  esp_err_t begin() {
    // Configure GPIO for decode speed switches
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << Pins::SLOW_DECODE) | (1ULL << Pins::MED_DECODE);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    // Configure LEDC timer for PWM
    ledc_timer_config_t timer_conf = {};
    timer_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
    timer_conf.duty_resolution = LEDC_TIMER_8_BIT;
    timer_conf.timer_num = LEDC_TIMER_0;
    timer_conf.freq_hz = 5000;
    timer_conf.clk_cfg = LEDC_AUTO_CLK;
    ESP_ERROR_CHECK(ledc_timer_config(&timer_conf));

    // Configure LEDC channel
    ledc_channel_config_t channel_conf = {};
    channel_conf.gpio_num = Pins::DECODE_PWM;
    channel_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
    channel_conf.channel = static_cast<ledc_channel_t>(PWMChannels::DECODE);
    channel_conf.timer_sel = LEDC_TIMER_0;
    channel_conf.duty = 0;
    channel_conf.hpoint = 0;
    ESP_ERROR_CHECK(ledc_channel_config(&channel_conf));

    return ESP_OK;
  }

  void update() {
    auto& config = ConfigManager::getInstance();

    // Read the decode speed switches
    bool slowSwitch = (gpio_get_level(static_cast<gpio_num_t>(Pins::SLOW_DECODE)) == 0);
    bool medSwitch = (gpio_get_level(static_cast<gpio_num_t>(Pins::MED_DECODE)) == 0);

    // Determine the speed based on switch states and set corresponding voltage
    uint32_t duty = 0;
    if (slowSwitch) {
      // SLOW switch works as expected
      config.setMorseSpeed(MorseSpeed::SLOW);
      duty = 85;  // 1.1V (33% of 3.3V)
    } else if (medSwitch) {
      // Swapped: MED switch now selects FAST speed
      config.setMorseSpeed(MorseSpeed::FAST);
      duty = 255;  // 3.3V (100% of 3.3V)
    } else {
      // Swapped: Default (no switch pressed) is now MEDIUM speed
      config.setMorseSpeed(MorseSpeed::MEDIUM);
      duty = 171;  // 2.2V (67% of 3.3V)
    }

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_HIGH_SPEED_MODE,
                                  static_cast<ledc_channel_t>(PWMChannels::DECODE), duty));
    ESP_ERROR_CHECK(
        ledc_update_duty(LEDC_HIGH_SPEED_MODE, static_cast<ledc_channel_t>(PWMChannels::DECODE)));
  }

 private:
  SpeedManagerESP() = default;
  SpeedManagerESP(const SpeedManagerESP&) = delete;
  SpeedManagerESP& operator=(const SpeedManagerESP&) = delete;
};

#endif  // SPEEDMANAGER_ESP_H