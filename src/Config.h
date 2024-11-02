#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Preferences.h> // Include Preferences library

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
extern Preferences preferences;   // Declare preferences
extern unsigned int characterGap; // Declare characterGap
extern unsigned int partGap;      // Declare partGap
extern unsigned int dotDuration;  // Declare dotDuration
extern unsigned int dashDuration; // Declare dashDuration

// Hardware pins
extern const int blueLEDPin;      // Blue LED pin for Wi-Fi status
extern const int SPEAKER_CHANNEL; // Declare SPEAKER_CHANNEL
extern const int LEEWAY;          // Declare LEEWAY

// Function prototypes
void setMorseSpeed(MorseSpeed speed); // Sets Morse code speed
void saveConfigurations();            // Saves configurations to NVS
void loadConfigurations();            // Loads configurations from NVS

#endif
