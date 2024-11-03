#include "AudioManager.h"

void AudioManager::begin()
{
    configurePWM();
}

void AudioManager::configurePWM()
{
    // Configure PWM for speaker
    ledcSetup(Audio::SPEAKER_CHANNEL, MORSE_FREQUENCY, 8); // 800Hz, 8-bit resolution
    ledcAttachPin(Pins::SPEAKER, Audio::SPEAKER_CHANNEL);

    // Configure decode indicator
    ledcSetup(1, 5000, 8);
    ledcAttachPin(Pins::DECODE_PWM, 1);
}

void AudioManager::setVolume(int adcValue)
{
    currentVolume = map(adcValue, 0, Radio::ADC_MAX, 0, 255); // Map to 8-bit
}

void AudioManager::handlePlayback()
{
    // Update volume periodically
    unsigned long currentTime = millis();
    if (currentTime - lastVolumeUpdate >= VOLUME_UPDATE_INTERVAL)
    {
        int volumeRead = analogRead(Pins::VOLUME_POT);
        if (abs(volumeRead - lastVolumeRead) > 50)
        { // Apply hysteresis
            setVolume(volumeRead);
            lastVolumeRead = volumeRead;
        }
        lastVolumeUpdate = currentTime;
    }
}

void AudioManager::playMorseTone()
{
    // Always set 800Hz before playing the tone
    ledcWriteTone(Audio::SPEAKER_CHANNEL, MORSE_FREQUENCY);
    ledcWrite(Audio::SPEAKER_CHANNEL, currentVolume);
}

void AudioManager::stopMorseTone()
{
    ledcWrite(Audio::SPEAKER_CHANNEL, 0);
}

void AudioManager::playStaticNoise(int signalStrength)
{
    // Generate random noise with filtered frequency range
    int noiseFrequency = random(Audio::MIN_STATIC_FREQ, Audio::MAX_STATIC_FREQ);
    int scaledVolume = map(signalStrength, 0, 255, 0, currentVolume);

    ledcWriteTone(Audio::SPEAKER_CHANNEL, noiseFrequency);
    ledcWrite(Audio::SPEAKER_CHANNEL, scaledVolume);
}

void AudioManager::stop()
{
    ledcWrite(Audio::SPEAKER_CHANNEL, 0);
}