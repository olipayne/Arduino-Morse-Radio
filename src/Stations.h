#ifndef STATIONS_H
#define STATIONS_H

#include <Arduino.h>
#include "Config.h"

struct Station
{
  const char *name;
  int frequency; // Editable frequency value
  WaveBand band;
  String message; // Editable Morse code message
};

extern Station stations[];
extern const int numStations;

#endif
