#include "StationStorage.h"
#include "Config.h"
#include <cstdio>

// Optimized: Generate preference keys without String allocations
void StationStorage::generatePreferenceKey(char* buffer, size_t bufferSize, const char* prefix, size_t index) const {
  snprintf(buffer, bufferSize, "%s%zu", prefix, index);
}

void StationStorage::saveStations(const std::vector<Station>& stations) {
  Preferences prefs;
  if (!prefs.begin("stations", false)) {
    return;
  }

  char keyBuffer[32];  // Buffer for key generation (e.g., "freq0", "msg15", etc.)

  for (size_t i = 0; i < stations.size(); i++) {
    generatePreferenceKey(keyBuffer, sizeof(keyBuffer), "freq", i);
    prefs.putInt(keyBuffer, stations[i].getFrequency());

    generatePreferenceKey(keyBuffer, sizeof(keyBuffer), "msg", i);
    prefs.putString(keyBuffer, stations[i].getMessage());

    generatePreferenceKey(keyBuffer, sizeof(keyBuffer), "enabled", i);
    prefs.putBool(keyBuffer, stations[i].isEnabled());
  }

  prefs.end();
}

void StationStorage::loadStations(std::vector<Station>& stations) {
  Preferences prefs;
  if (!prefs.begin("stations", true)) {
    return;
  }

  char keyBuffer[32];  // Buffer for key generation

  for (size_t i = 0; i < stations.size(); i++) {
    generatePreferenceKey(keyBuffer, sizeof(keyBuffer), "freq", i);
    int freq = prefs.getInt(keyBuffer, stations[i].getFrequency());

    generatePreferenceKey(keyBuffer, sizeof(keyBuffer), "msg", i);
    String msg = prefs.getString(keyBuffer, stations[i].getMessage());

    generatePreferenceKey(keyBuffer, sizeof(keyBuffer), "enabled", i);
    bool enabled = prefs.getBool(keyBuffer, true);  // Default to enabled

    stations[i].setFrequency(freq);
    stations[i].setMessage(msg);
    stations[i].setEnabled(enabled);
  }

  prefs.end();
}
