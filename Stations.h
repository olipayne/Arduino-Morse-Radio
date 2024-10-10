#ifndef STATIONS_H
#define STATIONS_H

#include <Arduino.h>

// Define the station structure
struct Station {
  const char* name;
  int frequency;
  int ledChannel;
  int ledPin;
};

// Extern declarations for station-related variables
extern const int numStations;
extern Station stations[];

#endif // STATIONS_H
