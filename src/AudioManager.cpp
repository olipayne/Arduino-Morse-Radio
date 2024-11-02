// AudioManager.cpp
#include "AudioManager.h"

AudioManager::AudioManager(int speakerPin, int pwmChannel)
{
    this->speakerPin = speakerPin;
    this->pwmChannel = pwmChannel;
}

void AudioManager::init()
{
    // Configure PWM
    const int PWM_FREQUENCY = 5000; // PWM frequency in Hz
    const int PWM_RESOLUTION = 8;   // PWM resolution (8-bit)

    ledcSetup(pwmChannel, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(speakerPin, pwmChannel);

    // Initially set the speaker to silent
    ledcWrite(pwmChannel, 0);
}

void AudioManager::playTone(unsigned int frequency, unsigned int dutyCycle)
{
    ledcWriteTone(pwmChannel, frequency);
    ledcWrite(pwmChannel, dutyCycle);
}

void AudioManager::stopTone()
{
    ledcWrite(pwmChannel, 0);
    ledcWriteTone(pwmChannel, 0);
}

void AudioManager::playStaticNoise(unsigned int volume)
{
    static unsigned long lastChangeTime = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastChangeTime > 50)
    { // Change frequency every 50ms
        int noiseFrequency = random(100, 300);
        if (volume > 0)
        {
            ledcWriteTone(pwmChannel, noiseFrequency);
            ledcWrite(pwmChannel, volume);
        }
        else
        {
            stopTone();
        }
        lastChangeTime = currentTime;
    }
}
