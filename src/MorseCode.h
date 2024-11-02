#ifndef MORSECODE_H
#define MORSECODE_H

#include <Arduino.h>

namespace MorseCode
{
    String getMorseCode(char c);
    void playStaticNoise(int overallSignalStrength);
    int calculateSignalStrength(int potValue, int targetValue);
} // namespace MorseCode

#endif // MORSECODE_H
