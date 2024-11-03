// AudioManager.cpp
#include "AudioManager.h"

void AudioManager::begin()
{
    // Configure PWM channels for audio output
    ledcSetup(Audio::SPEAKER_CHANNEL, Audio::PWM_FREQUENCY, Audio::PWM_RESOLUTION);
    ledcAttachPin(Pins::SPEAKER, Audio::SPEAKER_CHANNEL);

    // Configure PWM for decode indicator
    ledcSetup(1, Audio::PWM_FREQUENCY, Audio::PWM_RESOLUTION);
    ledcAttachPin(Pins::DECODE_PWM, 1);

    // Initialize volume control
    pinMode(Pins::VOLUME_POT, INPUT);
    updateVolume();
}

void AudioManager::configurePWM()
{
    ledcSetup(Audio::SPEAKER_CHANNEL, Audio::PWM_FREQUENCY, Audio::PWM_RESOLUTION);
    ledcAttachPin(Pins::SPEAKER, Audio::SPEAKER_CHANNEL);
}

void AudioManager::setVolume(int adcValue)
{
    auto &config = ConfigManager::getInstance();
    currentVolume = map(adcValue, 0, Radio::ADC_MAX, 0, 255);
    config.setSpeakerVolume(currentVolume);
}

void AudioManager::updateVolume()
{
    unsigned long currentTime = millis();
    if (currentTime - lastVolumeUpdate >= VOLUME_UPDATE_INTERVAL)
    {
        int volumeRead = analogRead(Pins::VOLUME_POT);
        if (abs(volumeRead - lastVolumeRead) > 50)
        { // Apply some hysteresis
            setVolume(volumeRead);
            lastVolumeRead = volumeRead;
        }
        lastVolumeUpdate = currentTime;
    }
}

void AudioManager::handlePlayback()
{
    auto &config = ConfigManager::getInstance();
    updateVolume();

    if (config.isMorsePlaying())
    {
        if (config.isMorseToneOn())
        {
            playMorseTone();
            pulseDecodeLED();
        }
        else
        {
            stopMorseTone();
        }
    }
    else
    {
        // When not playing Morse code, generate static based on tuning
        int signalStrength = analogRead(Pins::TUNING_POT); // This will be refined based on station tuning
        playStaticNoise(signalStrength);
    }
}

void AudioManager::playStaticNoise(int signalStrength)
{
    // Generate random frequency for static noise
    int noiseFrequency = random(Audio::MIN_STATIC_FREQ, Audio::MAX_STATIC_FREQ);
    ledcWriteTone(Audio::SPEAKER_CHANNEL, noiseFrequency);

    // Scale volume based on signal strength
    int scaledVolume = map(signalStrength, 0, 255, 0, currentVolume);
    ledcWrite(Audio::SPEAKER_CHANNEL, scaledVolume);
}

void AudioManager::playMorseTone()
{
    auto &config = ConfigManager::getInstance();
    ledcWriteTone(Audio::SPEAKER_CHANNEL, config.getMorseFrequency());
    ledcWrite(Audio::SPEAKER_CHANNEL, currentVolume);
}

void AudioManager::stopMorseTone()
{
    ledcWrite(Audio::SPEAKER_CHANNEL, 0);
}

void AudioManager::stop()
{
    ledcWrite(Audio::SPEAKER_CHANNEL, 0);
    auto &config = ConfigManager::getInstance();
    config.setMorsePlaying(false);
    config.setMorseToneOn(false);
}

void AudioManager::pulseDecodeLED()
{
    static unsigned long lastPulse = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastPulse >= 10)
    { // 10ms pulse width
        digitalWrite(Pins::DECODE_PWM, HIGH);
        delay(1); // Very short pulse
        digitalWrite(Pins::DECODE_PWM, LOW);
        lastPulse = currentTime;
    }
}