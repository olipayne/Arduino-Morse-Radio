#ifndef MORSECODE_H
#define MORSECODE_H

#include <Arduino.h>

String getMorseCode(char c);
void playStaticNoise(int overallSignalStrength);
int calculateSignalStrength(int potValue, int targetValue);

#endif
