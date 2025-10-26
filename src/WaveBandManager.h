#ifndef WAVEBANDMANAGER_H
#define WAVEBANDMANAGER_H

#include "Config.h"

class WaveBandManager {
 public:
  static WaveBandManager& getInstance() {
    static WaveBandManager instance;
    return instance;
  }

  // Initialization
  void begin();

  // Core functionality
  void update();
  WaveBand getCurrentBand() const;

  // LED control
  void updateLEDs();
  void setLEDBrightness(uint8_t brightness);
  void turnOffAllBandLEDs();

 private:
  WaveBandManager() = default;
  WaveBandManager(const WaveBandManager&) = delete;
  WaveBandManager& operator=(const WaveBandManager&) = delete;

  // Pin initialization
  void initializePins();

  // LED state tracking
  void updateBandLED(WaveBand band);

  // Internal state
  uint8_t ledBrightness = LEDConfig::MAX_BRIGHTNESS;

  // LED to Band mapping
  struct BandLED {
    WaveBand band;
    uint8_t pin;
  };

  static const BandLED BAND_LEDS[];
  static constexpr size_t NUM_BANDS = 3;
};

#endif
