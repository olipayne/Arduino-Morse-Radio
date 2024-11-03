// MorseCode.h
#ifndef MORSE_CODE_H
#define MORSE_CODE_H

#include <Arduino.h>
#include "Config.h"
#include "AudioManager.h"

class MorseCode
{
public:
    static MorseCode &getInstance()
    {
        static MorseCode instance;
        return instance;
    }

    // Core functions
    void begin();
    void playMessage(const String &message);
    void stop();

    // Settings
    void setSpeed(MorseSpeed speed)
    {
        ConfigManager::getInstance().setMorseSpeed(speed);
    }

    // Status
    bool isPlaying() const
    {
        return ConfigManager::getInstance().isMorsePlaying();
    }

    // Debug
    void printMessage(const String &message); // Prints Morse code pattern to Serial

private:
    MorseCode() = default;
    MorseCode(const MorseCode &) = delete;
    MorseCode &operator=(const MorseCode &) = delete;

    // Morse code conversion
    String getSymbol(char c) const;
    void playSymbol(char symbol);

    // Timing helpers
    void dotDelay();
    void dashDelay();
    void symbolGap();
    void letterGap();
    void wordGap();

    // Internal state tracking
    unsigned long lastToneStart = 0;
    bool shouldStop = false;

    // Constants
    static constexpr char DOT = '.';
    static constexpr char DASH = '-';
    static constexpr char SPACE = ' ';

    // Lookup table for Morse code patterns
    static const char *const MORSE_PATTERNS[];
};

#endif