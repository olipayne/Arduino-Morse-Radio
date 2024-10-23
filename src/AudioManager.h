// AudioManager.h
#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <Arduino.h>

class AudioManager
{
public:
    AudioManager(int speakerPin, int pwmChannel);

    void init();
    void playTone(unsigned int frequency, unsigned int dutyCycle);
    void stopTone();
    void playStaticNoise(unsigned int volume);

private:
    int speakerPin;
    int pwmChannel;
};

#endif
