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

    static constexpr int MORSE_FREQUENCY = 800; // Fixed 800Hz for morse

    // State tracking
    int currentVolume = 0;
    int lastVolumeRead = 0;
    unsigned long lastVolumeUpdate = 0;
    static constexpr unsigned long VOLUME_UPDATE_INTERVAL = 50; // ms
};

#endif