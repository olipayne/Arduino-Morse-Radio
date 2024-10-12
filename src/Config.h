#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Morse code speed settings
enum MorseSpeed
{
  LOW_SPEED,
  MEDIUM_SPEED,
  HIGH_SPEED
};

// Shared variables
extern unsigned int speakerDutyCycle; // Speaker volume (duty cycle)
extern unsigned int morseFrequency;   // Speaker frequency (Hz)
extern String londonMessage;
extern String hilversumMessage;
extern String barcelonaMessage;
extern MorseSpeed morseSpeed;

// Hardware pins
extern const int blueLEDPin; // Blue LED pin for Wi-Fi status

// Function prototypes
void setMorseSpeed(MorseSpeed speed); // Sets Morse code speed
void saveConfigurations();            // Saves configurations to NVS

#endif // CONFIG_H
