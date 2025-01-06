#include "Station.h"

bool Station::isInRange(int tuningValue) const {
  return abs(tuningValue - frequency) <= Radio::TUNING_LEEWAY;
}

int Station::getSignalStrength(int tuningValue) const {
  int distance = abs(tuningValue - frequency);

  // If outside tuning leeway, return 0
  if (distance > Radio::TUNING_LEEWAY) {
    return 0;
  }

  // Calculate strength based on proximity to center frequency
  // The closer to the center frequency, the higher the strength
  // Maximum strength (255) when perfectly tuned, decreasing linearly to 0 at the edges
  return map(Radio::TUNING_LEEWAY - distance, 0, Radio::TUNING_LEEWAY, 0, LEDConfig::MAX_BRIGHTNESS);
}
