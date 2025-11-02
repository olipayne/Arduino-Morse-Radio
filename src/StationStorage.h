#ifndef STATION_STORAGE_H
#define STATION_STORAGE_H

#include <Preferences.h>
#include <vector>
#include "Station.h"

class StationStorage {
 public:
  static StationStorage& getInstance() {
    static StationStorage instance;
    return instance;
  }

  void saveStations(const std::vector<Station>& stations);
  void loadStations(std::vector<Station>& stations);

 private:
  StationStorage() = default;
  StationStorage(const StationStorage&) = delete;
  StationStorage& operator=(const StationStorage&) = delete;

  // Optimized: Generate preference keys without String allocations
  void generatePreferenceKey(char* buffer, size_t bufferSize, const char* prefix, size_t index) const;
};

#endif
