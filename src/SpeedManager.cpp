#include "SpeedManager.h"

void SpeedManager::begin()
{
    pinMode(Pins::SLOW_DECODE, INPUT_PULLUP);
    pinMode(Pins::MED_DECODE, INPUT_PULLUP);

    // Setup PWM for DECODE_PWM pin using dedicated channel
    ledcSetup(PWMChannels::SPEED, 5000, 8); // 5kHz frequency, 8-bit resolution
    ledcAttachPin(Pins::DECODE_PWM, PWMChannels::SPEED);
}

void SpeedManager::update()
{
    auto &config = ConfigManager::getInstance();

    // Read the decode speed switches
    bool slowSwitch = (digitalRead(Pins::SLOW_DECODE) == LOW);
    bool medSwitch = (digitalRead(Pins::MED_DECODE) == LOW);

    // Determine the speed based on switch states and set corresponding voltage
    if (slowSwitch)
    {
        config.setMorseSpeed(MorseSpeed::SLOW);
        ledcWrite(PWMChannels::SPEED, 85); // 1.1V (33% of 3.3V)
    }
    else if (medSwitch)
    {
        config.setMorseSpeed(MorseSpeed::MEDIUM);
        ledcWrite(PWMChannels::SPEED, 171); // 2.2V (67% of 3.3V)
    }
    else
    {
        config.setMorseSpeed(MorseSpeed::FAST);
        ledcWrite(PWMChannels::SPEED, 255); // 3.3V (100% of 3.3V)
    }
}
