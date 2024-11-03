#include "SpeedManager.h"

void SpeedManager::begin()
{
    pinMode(Pins::SLOW_DECODE, INPUT_PULLUP);
    pinMode(Pins::MED_DECODE, INPUT_PULLUP);
}

void SpeedManager::update()
{
    auto &config = ConfigManager::getInstance();

    // Read the decode speed switches
    bool slowSwitch = (digitalRead(Pins::SLOW_DECODE) == LOW);
    bool medSwitch = (digitalRead(Pins::MED_DECODE) == LOW);

    // Determine the speed based on switch states
    if (slowSwitch)
    {
        config.setMorseSpeed(MorseSpeed::SLOW);
    }
    else if (medSwitch)
    {
        config.setMorseSpeed(MorseSpeed::MEDIUM);
    }
    else
    {
        config.setMorseSpeed(MorseSpeed::FAST);
    }
}