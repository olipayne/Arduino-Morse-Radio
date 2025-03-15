#ifndef SPEEDMANAGER_H
#define SPEEDMANAGER_H

#ifdef ESP_PLATFORM
#include "SpeedManagerESP.h"
using SpeedManager = SpeedManagerESP;
#else

#include "Arduino.h"
#include "Config.h"
#include "ConfigManager.h"

class SpeedManagerArduino {
 public:
  static SpeedManagerArduino& getInstance() {
    static SpeedManagerArduino instance;
    return instance;
  }

  void begin() {
    pinMode(Pins::SLOW_DECODE, INPUT_PULLUP);
    pinMode(Pins::MED_DECODE, INPUT_PULLUP);

    // Setup PWM for decode speed control using dedicated channel
    ledcSetup(PWMChannels::DECODE, 5000, 8);  // 5kHz frequency, 8-bit resolution
    ledcAttachPin(Pins::DECODE_PWM, PWMChannels::DECODE);
  }

  void update() {
    auto& config = ConfigManager::getInstance();

    // Read the decode speed switches
    bool slowSwitch = (digitalRead(Pins::SLOW_DECODE) == LOW);
    bool medSwitch = (digitalRead(Pins::MED_DECODE) == LOW);

    // Determine the speed based on switch states and set corresponding voltage
    if (slowSwitch) {
      // SLOW switch works as expected
      config.setMorseSpeed(MorseSpeed::SLOW);
      ledcWrite(PWMChannels::DECODE, 85);  // 1.1V (33% of 3.3V)
    } else if (medSwitch) {
      // Swapped: MED switch now selects FAST speed
      config.setMorseSpeed(MorseSpeed::FAST);
      ledcWrite(PWMChannels::DECODE, 255);  // 3.3V (100% of 3.3V)
    } else {
      // Swapped: Default (no switch pressed) is now MEDIUM speed
      config.setMorseSpeed(MorseSpeed::MEDIUM);
      ledcWrite(PWMChannels::DECODE, 171);  // 2.2V (67% of 3.3V)
    }
  }

 private:
  SpeedManagerArduino() = default;
  SpeedManagerArduino(const SpeedManagerArduino&) = delete;
  SpeedManagerArduino& operator=(const SpeedManagerArduino&) = delete;
};

using SpeedManager = SpeedManagerArduino;

#endif  // ESP_PLATFORM
#endif  // SPEEDMANAGER_H