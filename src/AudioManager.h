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
  int smoothVolume(int newReading);

  static constexpr int MORSE_FREQUENCY = 600;  // Fixed 600Hz for Morse code

  // Updated static frequency range
  static constexpr int MIN_STATIC_FREQ = 100;  // 100 Hz
  static constexpr int MAX_STATIC_FREQ = 300;  // 300 Hz

  // Volume smoothing
  static constexpr int VOLUME_SAMPLES = 3;      // Reduced from 5 to be more responsive
  static constexpr int VOLUME_THRESHOLD = 1;    // Reduced from 3 to be more sensitive
  int volumeReadings[VOLUME_SAMPLES] = {0};
  int volumeIndex = 0;
  int smoothedVolume = 0;

  // State tracking
  int currentVolume = 0;
  int lastVolumeRead = 0;
  unsigned long lastVolumeUpdate = 0;
  unsigned long lastPulseTime = 0;
  bool isPlayingMorse = false;
  static constexpr unsigned long VOLUME_UPDATE_INTERVAL = 20;  // Reduced from 50ms to 20ms
};

#endif
