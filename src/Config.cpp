#include "Config.h"

namespace {
// Helper function to create MorseTimings from a base duration
Audio::MorseTimings createTimings(unsigned int baseDuration) {
  return Audio::MorseTimings{
      .dotDuration = baseDuration,
      .dashDuration = baseDuration * 3,  // Dash is 3x dot
      .symbolGap = baseDuration,         // Symbol gap equals dot length
      .letterGap = baseDuration * 4,     // Letter gap is 4x dot
      .wordGap = baseDuration * 8        // Word gap is 8x dot
  };
}
}  // namespace

// Define morse timing constants based on base durations
const Audio::MorseTimings Audio::MORSE_SLOW = createTimings(500);
const Audio::MorseTimings Audio::MORSE_MEDIUM = createTimings(350);
const Audio::MorseTimings Audio::MORSE_FAST = createTimings(200);

const char* toString(MorseSpeed speed) {
  switch (speed) {
    case MorseSpeed::SLOW:
      return "Slow";
    case MorseSpeed::MEDIUM:
      return "Medium";
    case MorseSpeed::FAST:
      return "Fast";
    default:
      return "Unknown";
  }
}

const char* toString(WaveBand band) {
  switch (band) {
    case WaveBand::LONG_WAVE:
      return "Long Wave";
    case WaveBand::MEDIUM_WAVE:
      return "Medium Wave";
    case WaveBand::SHORT_WAVE:
      return "Short Wave";
    default:
      return "Unknown";
  }
}

void ConfigManager::begin() { load(); }

void ConfigManager::save() {
  preferences.begin("config", false);
  preferences.putUChar("morseSpeed", static_cast<uint8_t>(morseSpeed));
  preferences.putUChar("waveBand", static_cast<uint8_t>(currentBand));
  preferences.putUInt("frequency", morseFrequency);
  preferences.putUInt("volume", speakerVolume);
  preferences.end();
}

void ConfigManager::load() {
  preferences.begin("config", true);
  morseSpeed = static_cast<MorseSpeed>(
      preferences.getUChar("morseSpeed", static_cast<uint8_t>(MorseSpeed::MEDIUM)));
  currentBand = static_cast<WaveBand>(
      preferences.getUChar("waveBand", static_cast<uint8_t>(WaveBand::SHORT_WAVE)));
  morseFrequency = preferences.getUInt("frequency", Audio::DEFAULT_MORSE_FREQ);
  speakerVolume = preferences.getUInt("volume", Audio::DEFAULT_VOLUME);
  preferences.end();
}

void ConfigManager::reset() {
  morseSpeed = MorseSpeed::MEDIUM;
  currentBand = WaveBand::SHORT_WAVE;
  morseFrequency = Audio::DEFAULT_MORSE_FREQ;
  speakerVolume = Audio::DEFAULT_VOLUME;
  wifiEnabled = false;
  morsePlaying = false;
  morseToneOn = false;
  save();
}

const Audio::MorseTimings& ConfigManager::getCurrentMorseTimings() const {
  switch (morseSpeed) {
    case MorseSpeed::SLOW:
      return Audio::MORSE_SLOW;
    case MorseSpeed::FAST:
      return Audio::MORSE_FAST;
    default:
      return Audio::MORSE_MEDIUM;
  }
}

void ConfigManager::setMorseSpeed(MorseSpeed speed) { morseSpeed = speed; }

void ConfigManager::setWaveBand(WaveBand band) { currentBand = band; }

void ConfigManager::setMorseFrequency(unsigned int freq) { morseFrequency = freq; }

void ConfigManager::setSpeakerVolume(unsigned int vol) { speakerVolume = vol; }
