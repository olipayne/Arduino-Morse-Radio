#ifndef STATIONS_H
#define STATIONS_H

#include <Arduino.h>
#include <vector>
#include "Station.h"
#include "SignalManager.h"
#include "StationStorage.h"
#include "StationDefaults.h"

class StationManager
{
public:
  static StationManager &getInstance()
  {
    static StationManager instance;
    return instance;
  }

  // Initialization
  void begin();

  // Station finding and access
  Station *findClosestStation(int tuningValue, WaveBand band, int &signalStrength);
  const std::vector<Station> &getAllStations() const { return stations; }
  Station *getStation(size_t index);
  size_t getStationCount() const { return stations.size(); }

  // Station configuration
  void updateStation(size_t index, int frequency, const String &message);

  // Band-specific operations
  std::vector<Station *> getStationsForBand(WaveBand band);

  // Persistence
  void saveToPreferences();
  void loadFromPreferences();
  void resetToDefaults();

private:
  StationManager() = default;
  StationManager(const StationManager &) = delete;
  StationManager &operator=(const StationManager &) = delete;

  void initializeDefaultStations();

  std::vector<Station> stations;
};

#endif
