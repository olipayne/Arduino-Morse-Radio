#include "SpeedManager.h"

void updatePlaybackSpeed()
{
    if (digitalRead(SLOW_DECODE) == LOW)
    {
        morseSpeed = LOW_SPEED;
    }
    else if (digitalRead(MED_DECODE) == LOW)
    {
        morseSpeed = MEDIUM_SPEED;
    }
    else
    {
        morseSpeed = HIGH_SPEED;
    }
}
