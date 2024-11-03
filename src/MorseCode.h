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

    void begin();
    void update(); // Call this in the main loop
    void startMessage(const String &message);
    void stop();

    void setSpeed(MorseSpeed speed)
    {
        ConfigManager::getInstance().setMorseSpeed(speed);
    }

    bool isPlaying() const
    {
        return ConfigManager::getInstance().isMorsePlaying();
    }

private:
    MorseCode() = default;
    MorseCode(const MorseCode &) = delete;
    MorseCode &operator=(const MorseCode &) = delete;

    String getSymbol(char c) const;

    // Message state
    String currentMessage;
    size_t messageIndex = 0; // Current character in message
    size_t symbolIndex = 0;  // Current symbol in character
    String currentMorseChar; // Current character's morse code

    // Timing state
    unsigned long lastStateChange = 0;
    unsigned long tuneInStartTime = 0;
    bool inTuneInDelay = false;

    // Constants
    static constexpr unsigned long TUNE_IN_DELAY = 1000; // 1 second delay before starting
    static const char *const MORSE_PATTERNS[];
};

#endif