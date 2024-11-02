#include "AudioManager.h"
#include "MorseCode.h"

void setVolume(int volume)
{
    speakerDutyCycle = map(volume, 0, 4095, 0, 255);
}

void handleAudioPlayback()
{
    if (morsePlaying)
    {
        if (morseToneOn)
        {
            ledcWriteTone(SPEAKER_CHANNEL, morseFrequency);
            ledcWrite(SPEAKER_CHANNEL, speakerDutyCycle);
            pulseDecodePWM();
        }
        else
        {
            ledcWrite(SPEAKER_CHANNEL, 0);
        }
    }
    else
    {
        // Replace 2000 and 1000 with actual values as necessary
        int signalStrength = calculateSignalStrength(2000, 1000);
        playStaticNoise(signalStrength);
    }
}

void stopAudio()
{
    ledcWrite(SPEAKER_CHANNEL, 0);
}

void pulseDecodePWM()
{
    ledcWrite(DECODE_PWM_PIN, 255);
    delay(10);
    ledcWrite(DECODE_PWM_PIN, 0);
}
