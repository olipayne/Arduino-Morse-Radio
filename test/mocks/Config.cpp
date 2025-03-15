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
const Audio::MorseTimings Audio::MORSE_SLOW = createTimings(500);  // 500ms base = 2000ms letter gap
const Audio::MorseTimings Audio::MORSE_MEDIUM =
    createTimings(350);                                            // 350ms base = 1400ms letter gap
const Audio::MorseTimings Audio::MORSE_FAST = createTimings(200);  // 200ms base = 800ms letter gap

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