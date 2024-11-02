#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Preferences.h>
#include <vector>
#include "Station.h" // Include Station.h to use the Station class

namespace Config
{
  // Morse code speed settings
  enum class MorseSpeed : uint8_t
  {
    SLOW,
    MEDIUM,
    FAST
  };

  // Shared variables
  extern unsigned int speakerDutyCycle; // Speaker volume (duty cycle)
  extern unsigned int morseFrequency;   // Speaker frequency (Hz)
  extern MorseSpeed morseSpeed;
  extern unsigned int characterGap;
  extern unsigned int partGap;
  extern unsigned int dotDuration;
  extern unsigned int dashDuration;

  // Preferences object for NVS storage
  extern Preferences preferences;

  // Global stations vector
  extern std::vector<Station> stations; // Declare stations as extern

  // Hardware pins
  constexpr int BLUE_LED_PIN = LED_BUILTIN; // Blue LED pin for Wi-Fi status
  constexpr int SPEAKER_CHANNEL = 1;        // PWM channel for speaker
  constexpr int LEEWAY = 100;               // Range around station frequency for locking

  // Function prototypes
  void setMorseSpeed(MorseSpeed speed); // Sets Morse code speed
  void saveConfigurations();            // Saves configurations to NVS
  void loadConfigurations();            // Loads configurations from NVS
} // namespace Config

#endif // CONFIG_H
