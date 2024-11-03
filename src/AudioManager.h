// AudioManager.h
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

    // Initialization
    void begin();

    // Core audio functions
    void setVolume(int adcValue);
    void handlePlayback();
    void stop();

    // Audio effects
    void playStaticNoise(int signalStrength);
    void playMorseTone();
    void stopMorseTone();

    // Visual feedback
    void pulseDecodeLED();

    // Utility functions
    int getVolume() const { return currentVolume; }
    bool isPlaying() const { return ConfigManager::getInstance().isMorsePlaying(); }

private:
    AudioManager() {}                                       // Private constructor
    AudioManager(const AudioManager &) = delete;            // Delete copy constructor
    AudioManager &operator=(const AudioManager &) = delete; // Delete assignment operator

    void configurePWM();
    void updateVolume();

    int currentVolume = Audio::DEFAULT_VOLUME;
    int lastVolumeRead = 0;
    unsigned long lastVolumeUpdate = 0;
    static constexpr unsigned long VOLUME_UPDATE_INTERVAL = 50; // ms
};

#endif