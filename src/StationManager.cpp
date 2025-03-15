#include "StationManager.h"

void StationManager::begin() {
  SignalManager::getInstance().begin();

#ifdef DEBUG_SERIAL_OUTPUT
  Serial.println("StationManager initialized");
#endif

  initializeDefaultStations();
  loadFromPreferences();
}

void StationManager::initializeDefaultStations() {
  stations.clear();
  stations.reserve(StationDefaults::STATION_COUNT);

  for (size_t i = 0; i < StationDefaults::STATION_COUNT; i++) {
    const auto& def = StationDefaults::STATIONS[i];
    stations.emplace_back(def.name, def.frequency, def.band, def.message);
  }
}

Station* StationManager::findClosestStation(int tuningValue, WaveBand band, int& signalStrength) {
  Station* closestStation = nullptr;
  int bestSignalStrength = 0;

  // Iterate through all stations to find the closest one for the current band
  for (auto& station : stations) {
    // Skip stations that are not enabled or not on the current band
    if (!station.isEnabled() || station.getBand() != band) {
      continue;
    }

    // Calculate signal strength for this station
    int strength = station.getSignalStrength(tuningValue);

    // If this station has a better signal than what we've found so far, update our best match
    if (strength > bestSignalStrength) {
      bestSignalStrength = strength;
      closestStation = &station;
    }
  }

  // Update the output parameter with the best signal strength we found
  signalStrength = bestSignalStrength;

  return closestStation;
}

Station* StationManager::getStation(size_t index) {
  // Bounds checking to prevent out-of-range access
  if (index < stations.size()) {
    return &stations[index];
  }

  // Return nullptr if the index is out of bounds
  return nullptr;
}

std::vector<Station*> StationManager::getStationsForBand(WaveBand band) {
  std::vector<Station*> result;

  // Reserve space to avoid reallocations
  result.reserve(stations.size());

  // Collect all stations for the specified band
  for (auto& station : stations) {
    if (station.getBand() == band) {
      result.push_back(&station);
    }
  }

  return result;
}

void StationManager::updateStation(size_t index, int frequency, const String& message) {
  updateStation(index, frequency, message, true);
}

void StationManager::updateStation(size_t index, int frequency, const String& message,
                                   bool enabled) {
  // Bounds checking to prevent out-of-range access
  if (index >= stations.size()) {
    return;
  }

  // Update the station properties
  stations[index].setFrequency(frequency);
  stations[index].setMessage(message);
  stations[index].setEnabled(enabled);

  // Save changes to persistent storage
  saveToPreferences();
}

void StationManager::saveToPreferences() { StationStorage::getInstance().saveStations(stations); }

void StationManager::loadFromPreferences() { StationStorage::getInstance().loadStations(stations); }

void StationManager::resetToDefaults() {
  initializeDefaultStations();
  saveToPreferences();
}
