#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Preferences.h>

// Pin Configuration Namespace
namespace Pins {
// Digital Input Pins
constexpr int POWER_SWITCH = 14;
constexpr int LW_BAND_SWITCH = 12;
constexpr int MW_BAND_SWITCH = 6;
constexpr int SLOW_DECODE = 5;
constexpr int MED_DECODE = 11;
constexpr int WIFI_BUTTON = 0;

// Analog Input Pins
constexpr int TUNING_POT = 17;
constexpr int VOLUME_POT = 18;

// Output Pins
constexpr int BACKLIGHT = 33;
constexpr int POWER_LED = 44;
constexpr int LW_LED = 36;
constexpr int MW_LED = 37;
constexpr int SW_LED = 38;
constexpr int LOCK_LED = 7;
constexpr int CARRIER_PWM = 10;
constexpr int DECODE_PWM = 3;
constexpr int MORSE_LEDS = 35;
constexpr int SPEAKER = 1;
}  // namespace Pins

// PWM Channel Assignments
namespace PWMChannels {
constexpr uint8_t AUDIO = 1;      // Audio output
constexpr uint8_t DECODE = 2;     // Decode speed control
constexpr uint8_t CARRIER = 3;    // Signal strength indicator
constexpr uint8_t POWER_LED = 4;  // Power LED control
constexpr uint8_t LW_LED = 5;     // Long wave LED for battery indicator
constexpr uint8_t MW_LED = 6;     // Medium wave LED for battery indicator
constexpr uint8_t SW_LED = 7;     // Short wave LED for battery indicator
}  // namespace PWMChannels

// LED Configuration Namespace
namespace LEDConfig {
constexpr uint8_t PWM_RESOLUTION = 8;
constexpr uint32_t PWM_FREQUENCY = 200;  // 200Hz PWM frequency
constexpr uint8_t MAX_BRIGHTNESS = 255;
constexpr uint8_t MIN_BRIGHTNESS = 0;
constexpr uint32_t PULSE_PERIOD_MS = 1000;          // 1 second pulse period
constexpr uint32_t POWER_SWITCH_DEBOUNCE_MS = 500;  // Power switch debounce time

// LiPo battery voltage levels
constexpr float BATTERY_MIN_V = 3.2f;  // Battery empty (~0%)
constexpr float BATTERY_LOW_V = 3.5f;  // Battery low (~20%)
constexpr float BATTERY_MAX_V = 4.2f;  // Battery full (100%)
}  // namespace LEDConfig

// Audio Configuration Namespace
namespace Audio {
constexpr int SPEAKER_CHANNEL = 0;
constexpr int PWM_RESOLUTION = 8;
constexpr int PWM_FREQUENCY = 5000;
constexpr int MIN_STATIC_FREQ = 100;
constexpr int MAX_STATIC_FREQ = 300;
constexpr int DEFAULT_MORSE_FREQ = 800;
constexpr int DEFAULT_VOLUME = 64;

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

// Timing Configuration Namespace
namespace Timing {
constexpr unsigned long DEBOUNCE_DELAY = 500;      // ms
constexpr unsigned long DEBUG_INTERVAL = 5000;     // ms
constexpr unsigned long WIFI_TIMEOUT = 120000;     // 2 minutes
constexpr unsigned long LED_FLASH_INTERVAL = 500;  // ms
}  // namespace Timing

// Radio Configuration Namespace
namespace Radio {
constexpr int TUNING_LEEWAY = 50;  // Tolerance for tuning
constexpr int ADC_MAX = 4095;      // Maximum ADC value
}  // namespace Radio

// Enums with string support
enum class MorseSpeed { SLOW, MEDIUM, FAST };

enum class WaveBand { LONG_WAVE, MEDIUM_WAVE, SHORT_WAVE };

// String conversion utilities
const char* toString(MorseSpeed speed);
const char* toString(WaveBand band);

// Configuration Manager Class
class ConfigManager {
 public:
  static ConfigManager& getInstance() {
    static ConfigManager instance;
    return instance;
  }

  void begin();
  void save();
  void load();
  void reset();

  // Getters
  MorseSpeed getMorseSpeed() const { return morseSpeed; }
  WaveBand getWaveBand() const { return currentBand; }
  unsigned int getMorseFrequency() const { return morseFrequency; }
  unsigned int getSpeakerVolume() const { return speakerVolume; }
  bool isWifiEnabled() const { return wifiEnabled; }
  bool isMorsePlaying() const { return morsePlaying; }
  bool isMorseToneOn() const { return morseToneOn; }
  const Audio::MorseTimings& getCurrentMorseTimings() const;

  // Setters
  void setMorseSpeed(MorseSpeed speed);
  void setWaveBand(WaveBand band);
  void setMorseFrequency(unsigned int freq);
  void setSpeakerVolume(unsigned int vol);
  void setWifiEnabled(bool enabled);
  void setMorsePlaying(bool playing) { morsePlaying = playing; }
  void setMorseToneOn(bool on) { morseToneOn = on; }

 private:
  ConfigManager() {}
  ConfigManager(const ConfigManager&) = delete;
  ConfigManager& operator=(const ConfigManager&) = delete;

  Preferences preferences;

  // State variables
  MorseSpeed morseSpeed = MorseSpeed::MEDIUM;
  WaveBand currentBand = WaveBand::SHORT_WAVE;
  unsigned int morseFrequency = Audio::DEFAULT_MORSE_FREQ;
  unsigned int speakerVolume = Audio::DEFAULT_VOLUME;
  bool wifiEnabled = false;
  bool morsePlaying = false;
  bool morseToneOn = false;
};

#endif
