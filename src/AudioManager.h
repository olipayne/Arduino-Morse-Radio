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
    void pulseDecodePWM(); // New method for decode pulse

private:
    AudioManager() = default;
    AudioManager(const AudioManager &) = delete;
    AudioManager &operator=(const AudioManager &) = delete;

    void configurePWM();

    static constexpr int MORSE_FREQUENCY = 800;  // Fixed 800Hz for morse
    static constexpr int DECODE_PWM_CHANNEL = 1; // Separate channel for decode pulses
    static constexpr int PULSE_DURATION_MS = 50; // Duration of decode pulse

    // State tracking
    int currentVolume = 0;
    int lastVolumeRead = 0;
    unsigned long lastVolumeUpdate = 0;
    unsigned long lastPulseTime = 0;
    static constexpr unsigned long VOLUME_UPDATE_INTERVAL = 50; // ms
};

#endif