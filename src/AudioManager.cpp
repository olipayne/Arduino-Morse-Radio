#include "AudioManager.h"

void AudioManager::begin()
{
    configurePWM();
    lastPulseTime = 0;
}

void AudioManager::configurePWM()
{
    // Configure PWM for speaker
    ledcSetup(Audio::SPEAKER_CHANNEL, MORSE_FREQUENCY, 8); // 800Hz, 8-bit resolution
    ledcAttachPin(Pins::SPEAKER, Audio::SPEAKER_CHANNEL);

    // Configure decode PWM
    ledcSetup(DECODE_PWM_CHANNEL, 5000, 8); // 5kHz, 8-bit for decode pulses
    ledcAttachPin(Pins::DECODE_PWM, DECODE_PWM_CHANNEL);
    ledcWrite(DECODE_PWM_CHANNEL, 0); // Start with output off
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

void AudioManager::pulseDecodePWM()
{
    // Set the decode PWM pin to full on
    ledcWrite(DECODE_PWM_CHANNEL, 255);
    delay(10); // Very short pulse
    ledcWrite(DECODE_PWM_CHANNEL, 0);
}