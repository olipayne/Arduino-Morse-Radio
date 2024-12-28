#include "AudioManager.h"

void AudioManager::begin()
{
    configurePWM();
    lastPulseTime = 0;
}

void AudioManager::configurePWM()
{
    // Configure PWM with Morse frequency as base (higher than static noise)
    ledcSetup(Audio::SPEAKER_CHANNEL, MORSE_FREQUENCY, 8);
    ledcAttachPin(Pins::SPEAKER, Audio::SPEAKER_CHANNEL);
}

void AudioManager::setVolume(int adcValue)
{
    currentVolume = map(adcValue, 0, Radio::ADC_MAX, 0, 255); // Map to 8-bit
    
    // If currently playing Morse tone, update volume without changing frequency
    if (isPlayingMorse) {
        ledcWrite(Audio::SPEAKER_CHANNEL, currentVolume);
    }
}

void AudioManager::handlePlayback()
{
    // Update volume periodically
    unsigned long currentTime = millis();
    if (currentTime - lastVolumeUpdate >= VOLUME_UPDATE_INTERVAL)
    {
        int volumeRead = analogRead(Pins::VOLUME_POT);
        setVolume(volumeRead);
        lastVolumeUpdate = currentTime;
    }
}

void AudioManager::playMorseTone()
{
    isPlayingMorse = true;
    // Set exact 600Hz frequency for Morse code
    ledcWriteTone(Audio::SPEAKER_CHANNEL, MORSE_FREQUENCY);
    ledcWrite(Audio::SPEAKER_CHANNEL, currentVolume);
}

void AudioManager::stopMorseTone()
{
    isPlayingMorse = false;
    ledcWrite(Audio::SPEAKER_CHANNEL, 0);
}

void AudioManager::playStaticNoise(int signalStrength)
{
    isPlayingMorse = false;
    // Generate random noise within the adjusted frequency range
    int noiseFrequency = random(MIN_STATIC_FREQ, MAX_STATIC_FREQ + 1);
    // Invert signalStrength mapping
    int scaledVolume = map(signalStrength, 0, 255, currentVolume, 0);

    ledcWriteTone(Audio::SPEAKER_CHANNEL, noiseFrequency);
    ledcWrite(Audio::SPEAKER_CHANNEL, scaledVolume);
}

void AudioManager::stop()
{
    isPlayingMorse = false;
    ledcWrite(Audio::SPEAKER_CHANNEL, 0);
}
