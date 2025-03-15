#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include "Arduino.h"

// Pin Configuration Namespace
namespace Pins {
// Digital Input Pins
const int POWER_SWITCH = 14;
const int LW_BAND_SWITCH = 12;
const int MW_BAND_SWITCH = 6;
const int SLOW_DECODE = 5;
const int MED_DECODE = 11;
const int WIFI_BUTTON = 0;

// Analog Input Pins
const int TUNING_POT = 17;
const int VOLUME_POT = 18;

// Output Pins
const int BACKLIGHT = 33;
const int POWER_LED = 44;
const int LW_LED = 36;
const int MW_LED = 37;
const int SW_LED = 38;
const int LOCK_LED = 7;
const int CARRIER_PWM = 10;
const int DECODE_PWM = 3;
const int MORSE_LEDS = 35;
const int SPEAKER = 1;
}  // namespace Pins

// PWM Channel Assignments
namespace PWMChannels {
const uint8_t AUDIO = 1;
const uint8_t DECODE = 2;
const uint8_t CARRIER = 3;
const uint8_t POWER_LED = 4;
const uint8_t LW_LED = 5;
const uint8_t MW_LED = 6;
const uint8_t SW_LED = 7;
}  // namespace PWMChannels

// LED Configuration Namespace
namespace LEDConfig {
const uint8_t PWM_RESOLUTION = 8;
const uint32_t PWM_FREQUENCY = 200;
const uint8_t MAX_BRIGHTNESS = 255;
const uint8_t MIN_BRIGHTNESS = 0;
}  // namespace LEDConfig

// Audio Configuration Namespace
namespace Audio {
const int SPEAKER_CHANNEL = 0;
const int PWM_RESOLUTION = 8;
const int PWM_FREQUENCY = 5000;
const int MIN_STATIC_FREQ = 100;
const int MAX_STATIC_FREQ = 300;
const int DEFAULT_MORSE_FREQ = 800;
const int DEFAULT_VOLUME = 64;

// Morse timing constants
struct MorseTimings {
  unsigned int dotDuration;
  unsigned int dashDuration;
  unsigned int symbolGap;
  unsigned int letterGap;
  unsigned int wordGap;
};

extern const MorseTimings MORSE_SLOW;
extern const MorseTimings MORSE_MEDIUM;
extern const MorseTimings MORSE_FAST;
}  // namespace Audio

// Radio Configuration Namespace
namespace Radio {
const int TUNING_LEEWAY = 50;
const int ADC_MAX = 4095;
}  // namespace Radio

// Enums with string support
enum class MorseSpeed { SLOW, MEDIUM, FAST };

enum class WaveBand { LONG_WAVE, MEDIUM_WAVE, SHORT_WAVE };

// String conversion utilities
const char* toString(MorseSpeed speed);
const char* toString(WaveBand band);

// Forward declarations
class ConfigManager;
class AudioManager;
class PowerManager;

#endif  // CONFIG_H