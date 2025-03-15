#ifndef WAVEBANDMANAGER_H
#define WAVEBANDMANAGER_H

#include "Arduino.h"
#include "Config.h"

class WaveBandManager {
 public:
  static WaveBandManager& getInstance() {
    static WaveBandManager instance;
    return instance;
  }

  struct BandLED {
    WaveBand band;
    int pin;
  };

  static const size_t NUM_BANDS = 3;
  static const BandLED BAND_LEDS[];

  void begin() {
    initializePins();
    update();  // Initial update
  }

  void initializePins() {
    // Initialize switch pins with pullups
    pinMode(Pins::LW_BAND_SWITCH, INPUT_PULLUP);
    pinMode(Pins::MW_BAND_SWITCH, INPUT_PULLUP);

    // Initialize LED pins as digital outputs
    for (size_t i = 0; i < NUM_BANDS; i++) {
      pinMode(BAND_LEDS[i].pin, OUTPUT);
      digitalWrite(BAND_LEDS[i].pin, LOW);
    }
  }

  void update() {
    auto& config = ConfigManager::getInstance();
    WaveBand newBand;

    // Read switch states
    if (digitalRead(Pins::LW_BAND_SWITCH) == LOW) {
      // LONG_WAVE switch works as expected
      newBand = WaveBand::LONG_WAVE;
    } else if (digitalRead(Pins::MW_BAND_SWITCH) == LOW) {
      // Swapped: MW switch now selects SHORT_WAVE
      // This is because the hardware was changed from rotary to toggle switches
      newBand = WaveBand::SHORT_WAVE;
    } else {
      // Swapped: Default (no switch pressed) is now MEDIUM_WAVE
      // This is because the hardware was changed from rotary to toggle switches
      newBand = WaveBand::MEDIUM_WAVE;
    }

    // Only update if band has changed
    if (newBand != config.getWaveBand()) {
      config.setWaveBand(newBand);
      updateLEDs();
    }
  }

  WaveBand getCurrentBand() const { return ConfigManager::getInstance().getWaveBand(); }

  void updateLEDs() {
    turnOffAllBandLEDs();
    updateBandLED(getCurrentBand());
  }

  void turnOffAllBandLEDs() {
    for (size_t i = 0; i < NUM_BANDS; i++) {
      digitalWrite(BAND_LEDS[i].pin, LOW);
    }
  }

  void updateBandLED(WaveBand band) {
    for (size_t i = 0; i < NUM_BANDS; i++) {
      if (BAND_LEDS[i].band == band) {
        digitalWrite(BAND_LEDS[i].pin, HIGH);
        break;
      }
    }
  }

 private:
  WaveBandManager() = default;
  WaveBandManager(const WaveBandManager&) = delete;
  WaveBandManager& operator=(const WaveBandManager&) = delete;
};

const WaveBandManager::BandLED WaveBandManager::BAND_LEDS[] = {
    {WaveBand::LONG_WAVE, Pins::LW_LED},
    {WaveBand::MEDIUM_WAVE, Pins::MW_LED},
    {WaveBand::SHORT_WAVE, Pins::SW_LED}};

#endif  // WAVEBANDMANAGER_H