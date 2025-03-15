#include "Station.h"

/**
 * Check if the tuning value is within range of this station
 *
 * @param tuningValue The current tuning potentiometer value (0-4095)
 * @return true if the station is in range and enabled, false otherwise
 */
bool Station::isInRange(int tuningValue) const {
  // Station must be enabled to be in range
  if (!enabled) {
    return false;
  }

  // Check if tuning value is within the station's frequency range
  int distance = abs(tuningValue - frequency);
  return distance <= Radio::TUNING_LEEWAY;
}

/**
 * Calculate the signal strength based on how close the tuning value is to this station's frequency
 *
 * @param tuningValue The current tuning potentiometer value (0-4095)
 * @return Signal strength (0-255), where 0 means no signal and 255 is maximum strength
 */
int Station::getSignalStrength(int tuningValue) const {
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
