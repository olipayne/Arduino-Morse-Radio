#ifndef STATION_H
#define STATION_H

#include "Arduino.h"
#include "Config.h"

class Station {
 public:
  Station(const char* name, int frequency, WaveBand band, const String& message)
      : name(name), frequency(frequency), band(band), message(message), enabled(true) {}

  // Getters
  const char* getName() const { return name; }
  int getFrequency() const { return frequency; }
  WaveBand getBand() const { return band; }
  String getMessage() const { return message; }
  bool isEnabled() const { return enabled; }

  // Setters
  void setFrequency(int newFreq) { frequency = newFreq; }
  void setMessage(const String& newMsg) { message = newMsg; }
  void setEnabled(bool state) { enabled = state; }

  // Station tuning logic
  int getSignalStrength(int tuningValue) const {
    // If station is disabled, always return 0 signal strength
    if (!enabled) {
      return 0;
    }

    // Calculate the distance between the tuning value and station frequency
    int distance = abs(tuningValue - frequency);

    // If we're outside the tuning range, return 0 signal strength
    if (distance > Radio::TUNING_LEEWAY) {
      return 0;
    }

    // Calculate signal strength based on distance (closer = stronger)
    // Map the distance (0 to TUNING_LEEWAY) to signal strength (255 to 0)
    return map(distance, 0, Radio::TUNING_LEEWAY, 255, 0);
  }

  bool isInRange(int tuningValue) const {
    // Station must be enabled to be in range
    if (!enabled) {
      return false;
    }

    // Check if tuning value is within the station's frequency range
    int distance = abs(tuningValue - frequency);
    return distance <= Radio::TUNING_LEEWAY;
  }

 private:
  const char* name;
  int frequency;
  WaveBand band;
  String message;
  bool enabled;
};

#endif