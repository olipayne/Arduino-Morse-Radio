#ifndef MORSECODE_H
#define MORSECODE_H

#include <Arduino.h>

// Function to get Morse code for a character
String getMorseCode(char c);
void playStaticNoise(int overallSignalStrength);
int calculateSignalStrength(int potValue, int targetValue);

#endif // MORSECODE_H
