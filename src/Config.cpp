#include "Config.h"

// Define morseSpeed with a default value
unsigned int morseFrequency = 800;    // Set a default frequency
MorseSpeed morseSpeed = MEDIUM_SPEED; // Set a default speed value
WaveBand currentWave = SHORT_WAVE;    // Define other global variables as needed
unsigned int speakerDutyCycle = 64;   // Set a default volume value

void loadConfigurations()
{
    preferences.begin("config", false);
    speakerDutyCycle = preferences.getUInt("volume", 64);
    morseFrequency = preferences.getUInt("frequency", 800);
    preferences.end();
}

void saveConfigurations()
{
    preferences.begin("config", false);
    preferences.putUInt("volume", speakerDutyCycle);
    preferences.putUInt("frequency", morseFrequency);
    preferences.end();
}
