// MorseCodePlayer.h
#ifndef MORSECODEPLAYER_H
#define MORSECODEPLAYER_H

#include <Arduino.h>

class MorseCodePlayer
{
public:
    MorseCodePlayer();

    void setMessage(const String &message);
    void setSpeed(unsigned int dotDuration);
    void update();
    bool isPlaying() const;
    bool isToneOn() const;
    unsigned int getFrequency() const;
    void setFrequency(unsigned int frequency);
    void setVolume(unsigned int volume);
    unsigned int getVolume() const;

private:
    String getMorseCode(char c);

    String message;
    int messageIndex;
    String currentMorseCode;
    int morseCodeIndex;

    unsigned long timer;
    bool toneOn;

    unsigned int dotDuration;
    unsigned int dashDuration;
    unsigned int partGap;
    unsigned int characterGap;

    bool playing;

    // Audio settings
    unsigned int frequency;
    unsigned int volume;
};

#endif
