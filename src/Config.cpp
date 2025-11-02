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

// String constants stored in PROGMEM
static const char str_slow[] PROGMEM = "Slow";
static const char str_medium[] PROGMEM = "Medium";
static const char str_fast[] PROGMEM = "Fast";
static const char str_unknown[] PROGMEM = "Unknown";

const char* toString(MorseSpeed speed) {
  switch (speed) {
    case MorseSpeed::SLOW:
      return str_slow;
    case MorseSpeed::MEDIUM:
      return str_medium;
    case MorseSpeed::FAST:
      return str_fast;
    default:
      return str_unknown;
  }
}

// Wave band string constants stored in PROGMEM
static const char str_long_wave[] PROGMEM = "Long Wave";
static const char str_medium_wave[] PROGMEM = "Medium Wave";
static const char str_short_wave[] PROGMEM = "Short Wave";

const char* toString(WaveBand band) {
  switch (band) {
    case WaveBand::LONG_WAVE:
      return str_long_wave;
    case WaveBand::MEDIUM_WAVE:
      return str_medium_wave;
    case WaveBand::SHORT_WAVE:
      return str_short_wave;
    default:
      return str_unknown;
  }
}

void ConfigManager::begin() { load(); }

void ConfigManager::save() {
  if (!preferences.begin("config", false)) {
    return;
  }
  preferences.putUChar("morseSpeed", static_cast<uint8_t>(morseSpeed));
  preferences.putUChar("waveBand", static_cast<uint8_t>(currentBand));
  preferences.putUInt("frequency", morseFrequency);
  preferences.putUInt("volume", speakerVolume);
  preferences.end();
}

void ConfigManager::load() {
  if (!preferences.begin("config", true)) {
    return;
  }
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
