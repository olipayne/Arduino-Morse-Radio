#include "StationStorage.h"
#include "Config.h"

String StationStorage::generatePreferenceKey(const char *prefix, size_t index) const
{
    return String(prefix) + String(index);
}

void StationStorage::saveStations(const std::vector<Station> &stations)
{
    Preferences prefs;
    prefs.begin("stations", false);

    for (size_t i = 0; i < stations.size(); i++)
    {
        String freqKey = generatePreferenceKey("freq", i);
        String msgKey = generatePreferenceKey("msg", i);
        String enabledKey = generatePreferenceKey("enabled", i);

        prefs.putInt(freqKey.c_str(), stations[i].getFrequency());
        prefs.putString(msgKey.c_str(), stations[i].getMessage());
        prefs.putBool(enabledKey.c_str(), stations[i].isEnabled());
    }

    prefs.end();
}

void StationStorage::loadStations(std::vector<Station> &stations)
{
    Preferences prefs;
    prefs.begin("stations", true);

    for (size_t i = 0; i < stations.size(); i++)
    {
        String freqKey = generatePreferenceKey("freq", i);
        String msgKey = generatePreferenceKey("msg", i);
        String enabledKey = generatePreferenceKey("enabled", i);

        int freq = prefs.getInt(freqKey.c_str(), stations[i].getFrequency());
        String msg = prefs.getString(msgKey.c_str(), stations[i].getMessage());
        bool enabled = prefs.getBool(enabledKey.c_str(), true); // Default to enabled

        stations[i].setFrequency(freq);
        stations[i].setMessage(msg);
        stations[i].setEnabled(enabled);
    }

    prefs.end();
}
