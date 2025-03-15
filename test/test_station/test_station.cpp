#include <unity.h>
#include "Config.h"
#include "Station.h"

// Test the Station constructor and getters
void test_station_constructor_and_getters() {
  // Create a test station
  Station station("Test Station", 2000, WaveBand::MEDIUM_WAVE, "TEST MESSAGE");

  // Test getters
  TEST_ASSERT_EQUAL_STRING("Test Station", station.getName());
  TEST_ASSERT_EQUAL(2000, station.getFrequency());
  TEST_ASSERT_EQUAL(WaveBand::MEDIUM_WAVE, station.getBand());
  TEST_ASSERT_EQUAL_STRING("TEST MESSAGE", station.getMessage().c_str());
  TEST_ASSERT_TRUE(station.isEnabled());
}

// Test the Station setters
void test_station_setters() {
  // Create a test station
  Station station("Test Station", 2000, WaveBand::MEDIUM_WAVE, "TEST MESSAGE");

  // Test setters
  station.setFrequency(3000);
  TEST_ASSERT_EQUAL(3000, station.getFrequency());

  station.setMessage("NEW MESSAGE");
  TEST_ASSERT_EQUAL_STRING("NEW MESSAGE", station.getMessage().c_str());

  station.setEnabled(false);
  TEST_ASSERT_FALSE(station.isEnabled());

  station.setEnabled(true);
  TEST_ASSERT_TRUE(station.isEnabled());
}

// Test the isInRange method
void test_station_is_in_range() {
  // Create a test station
  Station station("Test Station", 2000, WaveBand::MEDIUM_WAVE, "TEST MESSAGE");

  // Test in range
  TEST_ASSERT_TRUE(station.isInRange(2000));  // Exact match
  TEST_ASSERT_TRUE(station.isInRange(2050));  // Upper bound
  TEST_ASSERT_TRUE(station.isInRange(1950));  // Lower bound

  // Test out of range
  TEST_ASSERT_FALSE(station.isInRange(2051));  // Just outside upper bound
  TEST_ASSERT_FALSE(station.isInRange(1949));  // Just outside lower bound
  TEST_ASSERT_FALSE(station.isInRange(3000));  // Far outside

  // Test with disabled station
  station.setEnabled(false);
  TEST_ASSERT_FALSE(station.isInRange(2000));  // Exact match but disabled
}

// Test the getSignalStrength method
void test_station_get_signal_strength() {
  // Create a test station
  Station station("Test Station", 2000, WaveBand::MEDIUM_WAVE, "TEST MESSAGE");

  // Test signal strength at various distances
  TEST_ASSERT_EQUAL(255, station.getSignalStrength(2000));  // Exact match = max strength
  TEST_ASSERT_EQUAL(0, station.getSignalStrength(2051));    // Just outside range = no signal

  // Test some values in between
  int strength1 = station.getSignalStrength(1975);
  int strength2 = station.getSignalStrength(1950);

  // Signal should be stronger when closer to the frequency
  TEST_ASSERT_TRUE(strength1 > strength2);

  // Test with disabled station
  station.setEnabled(false);
  TEST_ASSERT_EQUAL(0, station.getSignalStrength(2000));  // Disabled = no signal
}

void setUp(void) {
  // Set up code if needed
}

void tearDown(void) {
  // Tear down code if needed
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_station_constructor_and_getters);
  RUN_TEST(test_station_setters);
  RUN_TEST(test_station_is_in_range);
  RUN_TEST(test_station_get_signal_strength);

  return UNITY_END();
}