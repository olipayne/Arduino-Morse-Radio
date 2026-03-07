#include "SpeedManager.h"

void SpeedManager::begin() {
  pinMode(Pins::SLOW_DECODE, INPUT_PULLUP);
  pinMode(Pins::MED_DECODE, INPUT_PULLUP);

  // Setup PWM for decode speed control using dedicated channel
  ledcSetup(PWMChannels::DECODE, 5000, 8);  // 5kHz frequency, 8-bit resolution
  ledcAttachPin(Pins::DECODE_PWM, PWMChannels::DECODE);
}

void SpeedManager::update() {
  auto& config = ConfigManager::getInstance();

  // Read the decode speed switches
  bool slowSwitch = (digitalRead(Pins::SLOW_DECODE) == LOW);
  bool medSwitch = (digitalRead(Pins::MED_DECODE) == LOW);

  MorseSpeed targetSpeed = MorseSpeed::MEDIUM;
  int pwmDuty = 171;

  if (slowSwitch) {
    // SLOW switch works as expected
    targetSpeed = MorseSpeed::SLOW;
    pwmDuty = 85;  // 1.1V (33% of 3.3V)
  } else if (medSwitch) {
    // Swapped: MED switch now selects FAST speed
    // This is because the hardware was changed from rotary to toggle switches
    targetSpeed = MorseSpeed::FAST;
    pwmDuty = 255;  // 3.3V (100% of 3.3V)
  }

  static bool initialized = false;
  static MorseSpeed lastSpeed = MorseSpeed::MEDIUM;
  static int lastDuty = 171;
  if (initialized && targetSpeed == lastSpeed && pwmDuty == lastDuty) {
    return;
  }

  config.setMorseSpeed(targetSpeed);
  ledcWrite(PWMChannels::DECODE, pwmDuty);
  lastSpeed = targetSpeed;
  lastDuty = pwmDuty;
  initialized = true;
}
