#ifndef STATION_H
#define STATION_H

#include <Arduino.h>
#include "Config.h"

class Station
{
public:
    Station(const char *name, int frequency, WaveBand band, const String &message)
        : name(name), frequency(frequency), band(band), message(message), enabled(true) {}

    // Getters
    const char *getName() const { return name; }
    int getFrequency() const { return frequency; }
    WaveBand getBand() const { return band; }
    String getMessage() const { return message; }
    bool isEnabled() const { return enabled; }

    // Setters
    void setFrequency(int newFreq) { frequency = newFreq; }
    void setMessage(const String &newMsg) { message = newMsg; }
    void setEnabled(bool state) { enabled = state; }

    // Station tuning logic
    int getSignalStrength(int tuningValue) const;
    bool isInRange(int tuningValue) const;

private:
    const char *name;
    int frequency;
    WaveBand band;
    String message;
    bool enabled;
};

#endif
