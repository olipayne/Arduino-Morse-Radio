#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include "Config.h"

class AudioManager
{
public:
    static AudioManager &getInstance()
    {
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
    AudioManager(const AudioManager &) = delete;
    AudioManager &operator=(const AudioManager &) = delete;

    void configurePWM();

    static constexpr int MORSE_FREQUENCY = 600; // Fixed 600Hz for Morse code

    // Updated static frequency range
    static constexpr int MIN_STATIC_FREQ = 100; // 100 Hz
    static constexpr int MAX_STATIC_FREQ = 300; // 300 Hz

    // State tracking
    int currentVolume = 0;
    int lastVolumeRead = 0;
    unsigned long lastVolumeUpdate = 0;
    unsigned long lastPulseTime = 0;
    bool isPlayingMorse = false;
    static constexpr unsigned long VOLUME_UPDATE_INTERVAL = 50; // ms
};

#endif
