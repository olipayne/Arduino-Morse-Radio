#include "StationManager.h"

// Define default stations
const StationManager::StationDefaults StationManager::DEFAULT_STATIONS[] = {
    // Long Wave
    {"Athens", 300, WaveBand::LONG_WAVE, "FIRST CLUE FOUND IN LOBBY"},
    {"Budapest", 1000, WaveBand::LONG_WAVE, "CHECK BEHIND BLUE PAINTING"},
    {"Warsaw", 1400, WaveBand::LONG_WAVE, "LOCKER 237 CODE 4521"},
    {"Prague", 2000, WaveBand::LONG_WAVE, "RED KEY OPENS NEXT DOOR"},
    {"Cairo", 2200, WaveBand::LONG_WAVE, "LOOK UNDER THE STAIRS"},
    {"Monaco", 2400, WaveBand::LONG_WAVE, "BASEMENT HOLDS SECRET"},
    {"Oslo", 2900, WaveBand::LONG_WAVE, "CHECK THE BOOKSHELF"},
    {"Ankara", 3200, WaveBand::LONG_WAVE, "THIRD FLOOR NEXT"},
    {"Kiev", 3900, WaveBand::LONG_WAVE, "FINAL CODE 1893"},

    // Medium Wave
    {"Hilversum", 300, WaveBand::MEDIUM_WAVE, "START IN CONFERENCE ROOM"},
    {"Edinburgh", 900, WaveBand::MEDIUM_WAVE, "GREEN BOX HAS TOOLS"},
    {"Luxembourg", 1000, WaveBand::MEDIUM_WAVE, "PASSWORD IS EAGLE"},
    {"Dublin", 2000, WaveBand::MEDIUM_WAVE, "CHECK KITCHEN DRAWER"},
    {"London", 2100, WaveBand::MEDIUM_WAVE, "FOLLOW THE MAP"},
    {"Vienna", 2400, WaveBand::MEDIUM_WAVE, "NEXT CLUE IN VAULT"},
    {"Berlin", 3000, WaveBand::MEDIUM_WAVE, "COMBINATION 8675"},
    {"Rome", 3200, WaveBand::MEDIUM_WAVE, "LOOK BEHIND CLOCK"},
    {"Paris", 3900, WaveBand::MEDIUM_WAVE, "FINAL ROOM KEY"},

    // Short Wave
    {"Canberra", 300, WaveBand::SHORT_WAVE, "BEGIN AT RECEPTION"},
    {"Jerusalem", 900, WaveBand::SHORT_WAVE, "CHECK THE SAFE"},
    {"Bangkok", 1300, WaveBand::SHORT_WAVE, "WHITE CABINET TOP"},
    {"Havana", 2000, WaveBand::SHORT_WAVE, "USB KEY IN DRAWER"},
    {"Beijing", 2300, WaveBand::SHORT_WAVE, "FOLLOW RED MARKS"},
    {"Cape Town", 2500, WaveBand::SHORT_WAVE, "NEXT CODE 9312"},
    {"Toronto", 2700, WaveBand::SHORT_WAVE, "CHECK OLD FILES"},
    {"Washington", 3400, WaveBand::SHORT_WAVE, "LAST DOOR LEFT"},
    {"Moscow", 3900, WaveBand::SHORT_WAVE, "VICTORY ACHIEVED"}};

const size_t StationManager::DEFAULT_STATION_COUNT = sizeof(DEFAULT_STATIONS) / sizeof(DEFAULT_STATIONS[0]);

// Station implementation
int Station::getSignalStrength(int tuningValue) const
{
    // Scale tuning value from ADC range (0-4095) to station frequency range (0-4000)
    int scaledTuning = map(tuningValue, 0, Radio::ADC_MAX, 0, 4000);
    int difference = abs(scaledTuning - frequency);

    // If not in range, no signal
    if (difference > Radio::TUNING_LEEWAY)
    {
        return 0;
    }

    // When perfectly tuned (difference = 0), signal is max (255)
    // As difference approaches TUNING_LEEWAY, signal approaches 0
    return map(difference, Radio::TUNING_LEEWAY, 0, 0, 255);
}

bool Station::isInRange(int tuningValue) const
{
    int scaledTuning = map(tuningValue, 0, Radio::ADC_MAX, 0, 4000);
    return abs(scaledTuning - frequency) <= Radio::TUNING_LEEWAY;
}

// StationManager implementation
void StationManager::begin()
{
    // Initialize LOCK_LED as digital output
    pinMode(Pins::LOCK_LED, OUTPUT);
    digitalWrite(Pins::LOCK_LED, LOW); // Start with LED off

    // Initialize CARRIER_PWM with 5kHz frequency and 8-bit resolution
    ledcSetup(PWMChannels::CARRIER_PWM, 5000, 8);
    ledcAttachPin(Pins::CARRIER_PWM, PWMChannels::CARRIER_PWM);
    ledcWrite(PWMChannels::CARRIER_PWM, 0); // Start with no signal

    initializeDefaultStations();
    loadFromPreferences();
}

void StationManager::updateLockLED(bool locked)
{
    if (locked != isStationLocked)
    {
        isStationLocked = locked;
        digitalWrite(Pins::LOCK_LED, locked ? HIGH : LOW);
    }
}

void StationManager::initializeDefaultStations()
{
    stations.clear();
    stations.reserve(DEFAULT_STATION_COUNT);

    for (size_t i = 0; i < DEFAULT_STATION_COUNT; i++)
    {
        const auto &def = DEFAULT_STATIONS[i];
        stations.emplace_back(def.name, def.frequency, def.band, def.message);
    }
}

Station *StationManager::findClosestStation(int tuningValue, WaveBand band, int &signalStrength)
{
    Station *closest = nullptr;
    signalStrength = 0;
    bool stationLocked = false;

    // First, check if any station is in range
    for (auto &station : stations)
    {
        if (station.getBand() == band && station.isInRange(tuningValue))
        {
            int strength = station.getSignalStrength(tuningValue);
            if (strength > signalStrength)
            {
                signalStrength = strength;
                closest = &station;
                stationLocked = true;
            }
        }
    }

    // If no station is in range, find the one with the strongest signal
    if (!stationLocked)
    {
        signalStrength = 0; // Reset signal strength
        for (auto &station : stations)
        {
            if (station.getBand() == band)
            {
                int strength = station.getSignalStrength(tuningValue);
                if (strength > signalStrength)
                {
                    signalStrength = strength;
                    closest = &station;
                }
            }
        }
    }

    // Always update lock LED status
    updateLockLED(stationLocked);

    // Update carrier PWM to show signal strength (0-255)
    ledcWrite(PWMChannels::CARRIER_PWM, signalStrength);

    return closest;
}

Station *StationManager::getStation(size_t index)
{
    if (index < stations.size())
    {
        return &stations[index];
    }
    return nullptr;
}

std::vector<Station *> StationManager::getStationsForBand(WaveBand band)
{
    std::vector<Station *> bandStations;
    for (auto &station : stations)
    {
        if (station.getBand() == band)
        {
            bandStations.push_back(&station);
        }
    }
    return bandStations;
}

void StationManager::updateStation(size_t index, int frequency, const String &message)
{
    if (index < stations.size())
    {
        stations[index].setFrequency(frequency);
        stations[index].setMessage(message);
        saveToPreferences();
    }
}

String StationManager::generatePreferenceKey(const char *prefix, size_t index) const
{
    return String(prefix) + String(index);
}

void StationManager::saveToPreferences()
{
    auto &config = ConfigManager::getInstance();
    Preferences prefs;
    prefs.begin("stations", false);

    for (size_t i = 0; i < stations.size(); i++)
    {
        String freqKey = generatePreferenceKey("freq", i);
        String msgKey = generatePreferenceKey("msg", i);

        prefs.putInt(freqKey.c_str(), stations[i].getFrequency());
        prefs.putString(msgKey.c_str(), stations[i].getMessage());
    }

    prefs.end();
}

void StationManager::loadFromPreferences()
{
    Preferences prefs;
    prefs.begin("stations", true);

    for (size_t i = 0; i < stations.size(); i++)
    {
        String freqKey = generatePreferenceKey("freq", i);
        String msgKey = generatePreferenceKey("msg", i);

        int freq = prefs.getInt(freqKey.c_str(), stations[i].getFrequency());
        String msg = prefs.getString(msgKey.c_str(), stations[i].getMessage());

        stations[i].setFrequency(freq);
        stations[i].setMessage(msg);
    }

    prefs.end();
}

void StationManager::resetToDefaults()
{
    initializeDefaultStations();
    saveToPreferences();
}
