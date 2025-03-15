#ifndef SIGNALMANAGER_H
#define SIGNALMANAGER_H

#include <Arduino.h>
#include "Config.h"

class SignalManager {
 public:
  static SignalManager& getInstance() {
    static SignalManager instance;
    return instance;
  }

  void begin();

  // Signal status management
  void updateLockStatus(bool locked);
  void updateSignalStrength(int strength);

  // Debug output
  void debugPrint(bool locked, const char* stationName, int signalStrength);

  // Getters
  bool isStationLocked() const { return isLocked; }
  int getSignalStrength() const { return currentSignalStrength; }

 private:
  SignalManager() = default;
  SignalManager(const SignalManager&) = delete;
  SignalManager& operator=(const SignalManager&) = delete;

  // State tracking
  bool isLocked = false;
  int currentSignalStrength = 0;
};

#endif
