#include "Config.h"
#include "Station.h"
#include <vector>

namespace Config
{
    // Initialize shared variables
    unsigned int speakerDutyCycle = 64; // Default volume
    unsigned int morseFrequency = 800;  // Default frequency
    MorseSpeed morseSpeed = MorseSpeed::MEDIUM;
    unsigned int characterGap = 0;
    unsigned int partGap = 0;
    unsigned int dotDuration = 0;
    unsigned int dashDuration = 0;

    Preferences preferences;

    // Global stations vector
    std::vector<Station> stations;

    // Function to set Morse code speed
    void setMorseSpeed(MorseSpeed speed)
    {
        morseSpeed = speed;

        constexpr int multiplier = 3; // Multiplier for dash and character gap

        switch (morseSpeed)
        {
        case MorseSpeed::SLOW:
            dotDuration = 500;
            break;
        case MorseSpeed::MEDIUM:
            dotDuration = 300;
            break;
        case MorseSpeed::FAST:
            dotDuration = 100;
            break;
        }

        dashDuration = dotDuration * multiplier;
        partGap = dotDuration;
        characterGap = dotDuration * multiplier;
    }

    // Function to load configurations from non-volatile storage
    void loadConfigurations()
    {
        preferences.begin("config", false);

        // Load global settings
        speakerDutyCycle = preferences.getUInt("volume", 64);
        morseFrequency = preferences.getUInt("frequency", 800);
        morseSpeed = static_cast<MorseSpeed>(preferences.getUInt("morseSpeed", static_cast<unsigned int>(MorseSpeed::MEDIUM)));

        // Update Morse code timing
        setMorseSpeed(morseSpeed);

        // Load stations
        size_t stationCount = preferences.getUInt("stationCount", 0);
        stations.clear();

        for (size_t i = 0; i < stationCount; ++i)
        {
            String baseKey = "station" + String(i);

            // Create keys as String and then get c_str() for Preferences methods
            String nameKey = baseKey + "_name";
            String freqKey = baseKey + "_freq";
            String msgKey = baseKey + "_msg";

            String name = preferences.getString(nameKey.c_str(), "Station" + String(i));
            int frequency = preferences.getInt(freqKey.c_str(), 1000 + i * 1000);
            String message = preferences.getString(msgKey.c_str(), "S");

            stations.emplace_back(name, frequency, message);
        }

        preferences.end();

        // If no stations were loaded, initialize default stations
        if (stations.empty())
        {
            stations.emplace_back("London", 1000, "L");
            stations.emplace_back("Hilversum", 2000, "H");
            stations.emplace_back("Barcelona", 3000, "B");
        }
    }

    // Function to save configurations to non-volatile storage
    void saveConfigurations()
    {
        preferences.begin("config", false);

        // Save global settings
        preferences.putUInt("volume", speakerDutyCycle);
        preferences.putUInt("frequency", morseFrequency);
        preferences.putUInt("morseSpeed", static_cast<unsigned int>(morseSpeed));

        // Save stations
        preferences.putUInt("stationCount", stations.size());
        for (size_t i = 0; i < stations.size(); ++i)
        {
            String baseKey = "station" + String(i);

            // Create keys as String and then get c_str() for Preferences methods
            String nameKey = baseKey + "_name";
            String freqKey = baseKey + "_freq";
            String msgKey = baseKey + "_msg";

            preferences.putString(nameKey.c_str(), stations[i].getName());
            preferences.putInt(freqKey.c_str(), stations[i].getFrequency());
            preferences.putString(msgKey.c_str(), stations[i].getMessage());
        }

        preferences.end();
    }
} // namespace Config
