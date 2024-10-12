#ifndef STATIONS_H
#define STATIONS_H

#include <Arduino.h>

// Structure to represent a station
struct Station
{
  const char *name;
  int frequency; // ADC value corresponding to the station
};

// Extern declarations for stations array and numStations
extern Station stations[];
extern const int numStations;

#endif // STATIONS_H
