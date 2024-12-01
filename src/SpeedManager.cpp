#include "SpeedManager.h"

// Using channel 2 to avoid conflicts with Audio (0) and Decode (1)
static constexpr int SPEED_PWM_CHANNEL = 2;

void SpeedManager::begin()
{
    pinMode(Pins::SLOW_DECODE, INPUT_PULLUP);
    pinMode(Pins::MED_DECODE, INPUT_PULLUP);

    // Setup PWM for DECODE_PWM pin
    ledcSetup(SPEED_PWM_CHANNEL, 5000, 8); // Channel 2, 5kHz frequency, 8-bit resolution
    ledcAttachPin(Pins::DECODE_PWM, SPEED_PWM_CHANNEL);
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
        ledcWrite(SPEED_PWM_CHANNEL, 85); // 1.1V (33% of 3.3V)
    }
    else if (medSwitch)
    {
        config.setMorseSpeed(MorseSpeed::MEDIUM);
        ledcWrite(SPEED_PWM_CHANNEL, 171); // 2.2V (67% of 3.3V)
    }
    else
    {
        config.setMorseSpeed(MorseSpeed::FAST);
        ledcWrite(SPEED_PWM_CHANNEL, 255); // 3.3V (100% of 3.3V)
    }
}
