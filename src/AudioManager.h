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

  static constexpr int MORSE_FREQUENCY = 600;  // Fixed 600Hz for Morse code

  // Updated static frequency range
  static constexpr int MIN_STATIC_FREQ = 100;  // 100 Hz
  static constexpr int MAX_STATIC_FREQ = 300;  // 300 Hz

  // State tracking
  int currentVolume = 0;
  unsigned long lastPulseTime;
  unsigned long lastVolumeUpdate;
  bool isPlayingMorse = false;
  static constexpr unsigned long VOLUME_UPDATE_INTERVAL = 10;  // Reduced from 20ms to 10ms

  // Volume control
  static constexpr int VOLUME_DEAD_ZONE = 50;  // ADC values below this are considered zero
  static constexpr int VOLUME_MAX = 255;       // Maximum PWM value
};

#endif
