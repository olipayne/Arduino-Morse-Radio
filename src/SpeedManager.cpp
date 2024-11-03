#include "SpeedManager.h"

void updatePlaybackSpeed()
{
    auto &config = ConfigManager::getInstance();

    if (digitalRead(Pins::SLOW_DECODE) == LOW)
    {
        config.setMorseSpeed(MorseSpeed::SLOW);
    }
    else if (digitalRead(Pins::MED_DECODE) == LOW)
    {
        config.setMorseSpeed(MorseSpeed::MEDIUM);
    }
    else
    {
        config.setMorseSpeed(MorseSpeed::FAST);
    }
}