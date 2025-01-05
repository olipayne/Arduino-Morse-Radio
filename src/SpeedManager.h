#ifndef SPEEDMANAGER_H
#define SPEEDMANAGER_H

#include "Config.h"

class SpeedManager {
 public:
  static SpeedManager& getInstance() {
    static SpeedManager instance;
    return instance;
  }

  void begin();
  void update();  // Call this in the main loop

 private:
  SpeedManager() = default;
  SpeedManager(const SpeedManager&) = delete;
  SpeedManager& operator=(const SpeedManager&) = delete;
};

#endif