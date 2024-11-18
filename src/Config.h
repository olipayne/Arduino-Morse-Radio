#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Preferences.h>

// Debug logging system that compiles out in production
namespace Log
{
// Compile-time constant to enable/disable logging
#if defined(DEBUG_SERIAL_OUTPUT)
  static constexpr bool enabled = true;
#else
  static constexpr bool enabled = false;
#endif

  // Helper function to convert anything to a String
  template <typename T>
  static String toString(const T &arg)
  {
    return String(arg);
  }

  // Specialization for const char*
  static String toString(const char *arg)
  {
    return String(arg);
  }

  // Specialization for String
  static String toString(const String &arg)
  {
    return arg;
  }

  // Specialization for IPAddress
  static String toString(const IPAddress &arg)
  {
    return arg.toString();
  }

  // Single argument println
  template <typename T>
  static inline void println(const T &arg)
  {
    if constexpr (enabled)
    {
      Serial.println(toString(arg));
      delay(100);
    }
  }

  // Two argument println
  template <typename T1, typename T2>
  static inline void println(const T1 &arg1, const T2 &arg2)
  {
    if constexpr (enabled)
    {
      Serial.println(toString(arg1) + toString(arg2));
      delay(100);
    }
  }

  // Three argument println
  template <typename T1, typename T2, typename T3>
  static inline void println(const T1 &arg1, const T2 &arg2, const T3 &arg3)
  {
    if constexpr (enabled)
    {
      Serial.println(toString(arg1) + toString(arg2) + toString(arg3));
      delay(100);
    }
  }
}

// Pin Configuration Namespace
namespace Pins
{
  // Digital Input Pins
  constexpr uint8_t POWER_SWITCH = 14;
  constexpr uint8_t LW_BAND_SWITCH = 12;
  constexpr uint8_t MW_BAND_SWITCH = 6;
  constexpr uint8_t SLOW_DECODE = 5;
  constexpr uint8_t MED_DECODE = 11;
  constexpr uint8_t AUDIO_SWITCH = 10;
  constexpr uint8_t WIFI_BUTTON = 0;

  // Analog Input Pins
  constexpr uint8_t TUNING_POT = 17;
  constexpr uint8_t VOLUME_POT = 18;

  // Output Pins
  constexpr uint8_t BACKLIGHT = 33;
  constexpr uint8_t POWER_LED = 35;
  constexpr uint8_t LW_LED = 36;
  constexpr uint8_t MW_LED = 37;
  constexpr uint8_t SW_LED = 38;
  constexpr uint8_t LOCK_LED = 43;
  constexpr uint8_t CARRIER_PWM = 7;
  constexpr uint8_t DECODE_PWM = 3;
  constexpr uint8_t MORSE_LEDS = 44;
  constexpr uint8_t SPEAKER = 1;
}

// Audio Configuration Namespace
namespace Audio
{
  constexpr int SPEAKER_CHANNEL = 0;
  constexpr int PWM_RESOLUTION = 8;
  constexpr int PWM_FREQUENCY = 5000;
  constexpr int MIN_STATIC_FREQ = 100;
  constexpr int MAX_STATIC_FREQ = 300;
  constexpr int DEFAULT_MORSE_FREQ = 800;
  constexpr int DEFAULT_VOLUME = 64;

  // Morse timing constants
  struct MorseTimings
  {
    unsigned int dotDuration;
    unsigned int dashDuration;
    unsigned int symbolGap;
    unsigned int letterGap;
    unsigned int wordGap;
  };

  extern const MorseTimings MORSE_SLOW;
  extern const MorseTimings MORSE_MEDIUM;
  extern const MorseTimings MORSE_FAST;
}

// Timing Configuration Namespace
namespace Timing
{
  constexpr unsigned long DEBOUNCE_DELAY = 500;     // ms
  constexpr unsigned long DEBUG_INTERVAL = 5000;    // ms
  constexpr unsigned long WIFI_TIMEOUT = 120000;    // 2 minutes
  constexpr unsigned long LED_FLASH_INTERVAL = 500; // ms
}

// Radio Configuration Namespace
namespace Radio
{
  constexpr int TUNING_LEEWAY = 100; // Tolerance for tuning
  constexpr int ADC_MAX = 4095;      // Maximum ADC value
}

// Enums with string support
enum class MorseSpeed
{
  SLOW,
  MEDIUM,
  FAST
};

enum class WaveBand
{
  LONG_WAVE,
  MEDIUM_WAVE,
  SHORT_WAVE
};

// String conversion utilities
const char *toString(MorseSpeed speed);
const char *toString(WaveBand band);

// Configuration Manager Class
class ConfigManager
{
public:
  static ConfigManager &getInstance()
  {
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
  const Audio::MorseTimings &getCurrentMorseTimings() const;

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
  ConfigManager(const ConfigManager &) = delete;
  ConfigManager &operator=(const ConfigManager &) = delete;

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
