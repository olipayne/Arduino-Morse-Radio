// StationManager.h
#ifndef STATIONMANAGER_H
#define STATIONMANAGER_H

#include <Arduino.h>

struct Station
{
  String name;
  int frequency;
};

class StationManager
{
public:
  StationManager(int leeway);

  void addStation(const String &name, int frequency);
  int getSignalStrength(int potValue, const Station &station) const;
  const Station *getStrongestStation(int potValue, int &signalStrength) const;
  int getOverallSignalStrength(int potValue) const;

private:
  Station stations[10]; // Max 10 stations
  int stationCount;
  int leeway;
};

#endif
