#ifndef STATION_H
#define STATION_H

#include <Arduino.h>
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

#endif
