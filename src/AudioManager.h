#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include "Config.h"

class AudioManager {
 public:
  static AudioManager& getInstance() {
    static AudioManager instance;
    return instance;
  }

  void begin();
  void setVolume(int adcValue);
  void handlePlayback();
  void playMorseTone();
  void stopMorseTone();
  void playStaticNoise(int signalStrength);
  void stop();

 private:
  AudioManager() = default;
  AudioManager(const AudioManager&) = delete;
  AudioManager& operator=(const AudioManager&) = delete;

  void configurePWM();
  int calculateVolumeLevel(int adcValue);

  // Static noise generation helper methods
  void updateStaticPattern();
  int calculateStaticVolume();

  static constexpr int MORSE_FREQUENCY = 600;  // Fixed 600Hz for Morse code

  // Updated static frequency range
  static constexpr int MIN_STATIC_FREQ = 100;  // 100 Hz
  static constexpr int MAX_STATIC_FREQ = 300;  // 300 Hz

  // Static noise generation parameters
  static constexpr int STATIC_PATTERN_CHANGE_INTERVAL = 50;  // ms between static pattern updates
  static constexpr int MAX_CRACKLE_CHANCE =
      7;  // % chance of a crackle at maximum static (reduced from 30)
  static constexpr int CRACKLE_DURATION_MIN = 30;  // Minimum duration of a crackle in ms
  static constexpr int CRACKLE_DURATION_MAX = 80;  // Maximum duration of a crackle in ms

  // State tracking
  int currentVolume = 0;
  unsigned long lastPulseTime;
  unsigned long lastVolumeUpdate;
  unsigned long lastStaticPatternUpdate = 0;
  unsigned long crackleEndTime = 0;
  bool isPlayingMorse = false;
  bool isCrackling = false;
  bool isStaticPlaying = false;     // Track if static noise is currently playing
  int currentSignalStrength = 255;  // Start with full signal (no static)
  int staticBaseFrequency = MIN_STATIC_FREQ;

  static constexpr unsigned long VOLUME_UPDATE_INTERVAL = 10;  // Reduced from 20ms to 10ms

  // Volume control
  static constexpr int VOLUME_DEAD_ZONE = 50;  // ADC values below this are considered zero
  static constexpr int VOLUME_MAX = 255;       // Maximum PWM value
};

#endif
