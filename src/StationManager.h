#ifndef STATIONS_H
#define STATIONS_H

#include <Arduino.h>
#include <vector>
#include "Config.h"

class Station
{
public:
  Station(const char *name, int frequency, WaveBand band, const String &message)
      : name(name), frequency(frequency), band(band), message(message) {}

  // Getters
  const char *getName() const { return name; }
  int getFrequency() const { return frequency; }
  WaveBand getBand() const { return band; }
  String getMessage() const { return message; }

  // Setters
  void setFrequency(int newFreq) { frequency = newFreq; }
  void setMessage(const String &newMsg) { message = newMsg; }

  // Station tuning logic
  int getSignalStrength(int tuningValue) const;
  bool isInRange(int tuningValue) const;

private:
  const char *name;
  int frequency;
  WaveBand band;
  String message;
};

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

  // Helper methods
  void initializeDefaultStations();
  String generatePreferenceKey(const char *prefix, size_t index) const;

  std::vector<Station> stations;

  // Default station definitions
  struct StationDefaults
  {
    const char *name;
    int frequency;
    WaveBand band;
    const char *message;
  };

  static const StationDefaults DEFAULT_STATIONS[];
  static const size_t DEFAULT_STATION_COUNT;
};

#endif