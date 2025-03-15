#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "Config.h"
#include "Preferences.h"

class ConfigManager {
 public:
  static ConfigManager& getInstance();

  void begin() {}
  void save() {}
  void load() {}
  void reset() {}

  // Getters
  MorseSpeed getMorseSpeed() const { return morseSpeed; }
  WaveBand getWaveBand() const { return currentBand; }
  unsigned int getMorseFrequency() const { return morseFrequency; }
  unsigned int getSpeakerVolume() const { return speakerVolume; }
  bool isWifiEnabled() const { return wifiEnabled; }
  bool isMorsePlaying() const { return morsePlaying; }
  bool isMorseToneOn() const { return morseToneOn; }

  // Setters
  void setMorseSpeed(MorseSpeed speed) { morseSpeed = speed; }
  void setWaveBand(WaveBand band) { currentBand = band; }
  void setMorseFrequency(unsigned int freq) { morseFrequency = freq; }
  void setSpeakerVolume(unsigned int vol) { speakerVolume = vol; }
  void setWifiEnabled(bool enabled) { wifiEnabled = enabled; }
  void setMorsePlaying(bool playing) { morsePlaying = playing; }
  void setMorseToneOn(bool on) { morseToneOn = on; }

 private:
  // State variables
  MorseSpeed morseSpeed = MorseSpeed::MEDIUM;
  WaveBand currentBand = WaveBand::SHORT_WAVE;
  unsigned int morseFrequency = Audio::DEFAULT_MORSE_FREQ;
  unsigned int speakerVolume = Audio::DEFAULT_VOLUME;
  bool wifiEnabled = false;
  bool morsePlaying = false;
  bool morseToneOn = false;
};

#endif  // CONFIGMANAGER_H