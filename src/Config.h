#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Preferences.h>

/**
 * Pin Configuration Namespace
 * Contains all pin assignments for the radio hardware
 */
namespace Pins {
// Digital Input Pins
constexpr int POWER_SWITCH = 14;    // Main power switch
constexpr int LW_BAND_SWITCH = 12;  // Long wave band selection switch
constexpr int MW_BAND_SWITCH = 6;   // Medium wave band selection switch
constexpr int SLOW_DECODE = 5;      // Slow morse decode speed switch
constexpr int MED_DECODE = 11;      // Medium morse decode speed switch
constexpr int WIFI_BUTTON = 0;      // WiFi toggle button

// Analog Input Pins
constexpr int TUNING_POT = 17;  // Tuning potentiometer
constexpr int VOLUME_POT = 18;  // Volume potentiometer

// Output Pins
constexpr int BACKLIGHT = 33;    // Display backlight
constexpr int POWER_LED = 44;    // Power indicator LED
constexpr int LW_LED = 36;       // Long wave band indicator LED
constexpr int MW_LED = 37;       // Medium wave band indicator LED
constexpr int SW_LED = 38;       // Short wave band indicator LED
constexpr int LOCK_LED = 7;      // Station lock indicator LED
constexpr int CARRIER_PWM = 10;  // Signal strength indicator
constexpr int DECODE_PWM = 3;    // Decode speed indicator
constexpr int MORSE_LEDS = 35;   // Morse code indicator LEDs
constexpr int SPEAKER = 1;       // Audio output speaker
}  // namespace Pins

/**
 * PWM Channel Assignments
 * ESP32 has multiple PWM channels that need to be assigned to specific functions
 */
namespace PWMChannels {
constexpr uint8_t AUDIO = 1;      // Audio output
constexpr uint8_t DECODE = 2;     // Decode speed control
constexpr uint8_t CARRIER = 3;    // Signal strength indicator
constexpr uint8_t POWER_LED = 4;  // Power LED control
constexpr uint8_t LW_LED = 5;     // Long wave LED for battery indicator
constexpr uint8_t MW_LED = 6;     // Medium wave LED for battery indicator
constexpr uint8_t SW_LED = 7;     // Short wave LED for battery indicator
}  // namespace PWMChannels

/**
 * LED Configuration Namespace
 * Contains settings for LED brightness, PWM parameters, and battery thresholds
 */
namespace LEDConfig {
// PWM settings for LEDs
constexpr uint8_t PWM_RESOLUTION = 8;    // 8-bit resolution (0-255)
constexpr uint32_t PWM_FREQUENCY = 200;  // 200Hz PWM frequency
constexpr uint8_t MAX_BRIGHTNESS = 255;  // Maximum LED brightness
constexpr uint8_t MIN_BRIGHTNESS = 0;    // Minimum LED brightness (off)

// Timing parameters
constexpr uint32_t PULSE_PERIOD_MS = 1000;          // 1 second pulse period
constexpr uint32_t POWER_SWITCH_DEBOUNCE_MS = 500;  // Power switch debounce time

// LiPo battery voltage thresholds
constexpr float BATTERY_MIN_V = 3.2f;  // Battery empty (~0%)
constexpr float BATTERY_LOW_V = 3.5f;  // Battery low (~20%)
constexpr float BATTERY_MAX_V = 4.2f;  // Battery full (100%)
}  // namespace LEDConfig

/**
 * Audio Configuration Namespace
 * Contains settings for audio output, frequencies, and morse code timing
 */
namespace Audio {
// Audio output settings
constexpr int SPEAKER_CHANNEL = 0;       // PWM channel for speaker
constexpr int PWM_RESOLUTION = 8;        // 8-bit resolution (0-255)
constexpr int PWM_FREQUENCY = 5000;      // 5kHz PWM frequency
constexpr int MIN_STATIC_FREQ = 100;     // Minimum static noise frequency (Hz)
constexpr int MAX_STATIC_FREQ = 300;     // Maximum static noise frequency (Hz)
constexpr int DEFAULT_MORSE_FREQ = 800;  // Default morse code tone frequency (Hz)
constexpr int DEFAULT_VOLUME = 64;       // Default volume level (0-255)

/**
 * Morse timing parameters structure
 * Contains all timing values needed for morse code generation
 */
struct MorseTimings {
  unsigned int dotDuration;   // Duration of a dot (ms)
  unsigned int dashDuration;  // Duration of a dash (ms)
  unsigned int symbolGap;     // Gap between symbols within a character (ms)
  unsigned int letterGap;     // Gap between letters (ms)
  unsigned int wordGap;       // Gap between words (ms)
};

// Predefined timing sets for different morse speeds
extern const MorseTimings MORSE_SLOW;
extern const MorseTimings MORSE_MEDIUM;
extern const MorseTimings MORSE_FAST;
}  // namespace Audio

/**
 * Timing Configuration Namespace
 * Contains various timing constants used throughout the application
 */
namespace Timing {
constexpr unsigned long DEBOUNCE_DELAY = 500;      // Button debounce delay (ms)
constexpr unsigned long DEBUG_INTERVAL = 5000;     // Debug print interval (ms)
constexpr unsigned long WIFI_TIMEOUT = 120000;     // WiFi auto-off timeout (2 minutes)
constexpr unsigned long LED_FLASH_INTERVAL = 500;  // LED flash interval (ms)
}  // namespace Timing

/**
 * Radio Configuration Namespace
 * Contains settings specific to radio tuning
 */
namespace Radio {
constexpr int TUNING_LEEWAY = 50;  // Tuning tolerance range
constexpr int ADC_MAX = 4095;      // Maximum ADC value (12-bit)
}  // namespace Radio

/**
 * Morse speed enumeration
 * Defines the available morse code speeds
 */
enum class MorseSpeed { SLOW, MEDIUM, FAST };

/**
 * Wave band enumeration
 * Defines the available radio wave bands
 */
enum class WaveBand { LONG_WAVE, MEDIUM_WAVE, SHORT_WAVE };

// String conversion utilities
const char* toString(MorseSpeed speed);
const char* toString(WaveBand band);

/**
 * Configuration Manager Class
 * Handles persistent storage and retrieval of system settings
 */
class ConfigManager {
 public:
  static ConfigManager& getInstance() {
    static ConfigManager instance;
    return instance;
  }

  // Initialization and persistence
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
  unsigned int getInactivityTimeout() const { return inactivityTimeoutMinutes; }
  const Audio::MorseTimings& getCurrentMorseTimings() const;

  // Setters
  void setMorseSpeed(MorseSpeed speed);
  void setWaveBand(WaveBand band);
  void setMorseFrequency(unsigned int freq);
  void setSpeakerVolume(unsigned int vol);
  void setWifiEnabled(bool enabled);
  void setMorsePlaying(bool playing) { morsePlaying = playing; }
  void setMorseToneOn(bool on) { morseToneOn = on; }
  void setInactivityTimeout(unsigned int minutes);

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
  unsigned int inactivityTimeoutMinutes = 120;  // Default 120 minutes (2 hours)
  bool wifiEnabled = false;
  bool morsePlaying = false;
  bool morseToneOn = false;
};

#endif
