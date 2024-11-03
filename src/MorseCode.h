#ifndef MORSECODE_H
#define MORSECODE_H

#include <Arduino.h>

String getMorseCode(char c);
void playMorseMessage(const String &message);
void stopMorse();
int calculateSignalStrength(int potValue, int targetValue);

extern bool morsePlaying;
extern bool morseToneOn;

#endif
