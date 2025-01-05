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
  Station* closest = nullptr;
  bool stationLocked = false;
  signalStrength = 0;

  // Check all stations in the current band
  for (auto& station : stations) {
    if (station.getBand() == band && station.isEnabled()) {
      // Check if we're in range of this station
      if (station.isInRange(tuningValue)) {
        closest = &station;
        stationLocked = true;
        signalStrength = station.getSignalStrength(tuningValue);
        break;
      }
      // If not in range but closest so far, track it
      else if (!closest) {
        closest = &station;
      }
    }
  }

  // Update signal indicators
  auto& signalMgr = SignalManager::getInstance();
  signalMgr.updateLockStatus(stationLocked);
  signalMgr.updateSignalStrength(signalStrength);

#ifdef DEBUG_SERIAL_OUTPUT
  signalMgr.debugPrint(stationLocked, closest ? closest->getName() : nullptr, signalStrength);
#endif

  return closest;
}

Station* StationManager::getStation(size_t index) {
  if (index < stations.size()) {
    return &stations[index];
  }
  return nullptr;
}

std::vector<Station*> StationManager::getStationsForBand(WaveBand band) {
  std::vector<Station*> bandStations;
  for (auto& station : stations) {
    if (station.getBand() == band) {
      bandStations.push_back(&station);
    }
  }
  return bandStations;
}

void StationManager::updateStation(size_t index, int frequency, const String& message) {
  updateStation(index, frequency, message, true);
}

void StationManager::updateStation(size_t index, int frequency, const String& message,
                                   bool enabled) {
  if (index < stations.size()) {
    stations[index].setFrequency(frequency);
    stations[index].setMessage(message);
    stations[index].setEnabled(enabled);
    saveToPreferences();
  }
}

void StationManager::saveToPreferences() { StationStorage::getInstance().saveStations(stations); }

void StationManager::loadFromPreferences() { StationStorage::getInstance().loadStations(stations); }

void StationManager::resetToDefaults() {
  initializeDefaultStations();
  saveToPreferences();
}
