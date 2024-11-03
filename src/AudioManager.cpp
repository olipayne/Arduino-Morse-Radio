#include "AudioManager.h"
#include "MorseCode.h"
#include "Config.h"

void setVolume(int volume)
{
    speakerDutyCycle = map(volume, 0, 4095, 0, 255); // Map ADC value to PWM duty cycle (0-255)
}

void handleAudioPlayback()
{
    // Read the current volume level from the VOLUME_ADC
    int volumeADC = analogRead(VOLUME_ADC); // Read the volume potentiometer (pin 18)
    setVolume(volumeADC);                   // Update speakerDutyCycle based on ADC value

    if (morsePlaying)
    {
        if (morseToneOn)
        {
            ledcWriteTone(SPEAKER_CHANNEL, morseFrequency); // Set frequency for Morse tone
            ledcWrite(SPEAKER_CHANNEL, speakerDutyCycle);   // Apply mapped volume to Morse code
            pulseDecodePWM();
        }
        else
        {
            ledcWrite(SPEAKER_CHANNEL, 0); // Turn off sound between Morse tones
        }
    }
    else
    {
        // Generate static noise when not locked onto a station
        int signalStrength = calculateSignalStrength(2000, 1000); // Replace 2000 and 1000 as needed
        playStaticNoise(signalStrength);
    }
}

void playStaticNoise(int signalStrength)
{
    // Adjust static noise frequency and volume based on signal strength and volume setting
    int noiseFrequency = random(100, 300); // Random frequency between 100Hz and 300Hz
    ledcWriteTone(SPEAKER_CHANNEL, noiseFrequency);
    ledcWrite(SPEAKER_CHANNEL, map(signalStrength, 0, 255, 0, speakerDutyCycle)); // Adjusted volume based on signal strength
}

void stopAudio()
{
    ledcWrite(SPEAKER_CHANNEL, 0); // Stop all audio output
}

void pulseDecodePWM()
{
    ledcWrite(DECODE_PWM_PIN, 255); // Pulse for decode indication
    delay(10);
    ledcWrite(DECODE_PWM_PIN, 0);
}
