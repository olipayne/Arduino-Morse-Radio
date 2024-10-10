#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Morse code speed settings
enum MorseSpeed { LOW_SPEED, MEDIUM_SPEED, HIGH_SPEED };

// Declare variables to be shared across files
extern unsigned int speakerDutyCycle;
extern unsigned int morseFrequency;
extern String londonMessage;
extern String hilversumMessage;
extern String barcelonaMessage;
extern MorseSpeed morseSpeed;

// Function prototypes
void setMorseSpeed(MorseSpeed speed);
void saveConfigurations();

#endif // CONFIG_H
