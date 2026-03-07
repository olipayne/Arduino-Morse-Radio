#include <unity.h>

#include "../../src/Config.h"
#include "../../src/StationManager.h"

#include "../mocks/HardwareEmulator.h"
#include "../mocks/HardwareEmulator.cpp"

void setUp() {
  HardwareEmulator::getInstance().reset();
  ConfigManager::getInstance().reset();
}

void tearDown() {}

void test_station_manager_finds_closest_station_for_band() {
  auto& manager = StationManager::getInstance();
  manager.begin();

  int signalStrength = 0;
  Station* station = manager.findClosestStation(2048, WaveBand::MEDIUM_WAVE, signalStrength);

  TEST_ASSERT_NOT_NULL(station);
  TEST_ASSERT_EQUAL_STRING("Dublin", station->getName());
  TEST_ASSERT_TRUE(signalStrength > 0);
}

void test_station_updates_persist_through_preferences_storage() {
  auto& manager = StationManager::getInstance();
  manager.begin();

  manager.updateStation(12, 2100, "DEVICE TEST MESSAGE", false);

  manager.begin();
  Station* station = manager.getStation(12);

  TEST_ASSERT_NOT_NULL(station);
  TEST_ASSERT_EQUAL(2100, station->getFrequency());
  TEST_ASSERT_EQUAL_STRING("DEVICE TEST MESSAGE", station->getMessage().c_str());
  TEST_ASSERT_FALSE(station->isEnabled());
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_station_manager_finds_closest_station_for_band);
  RUN_TEST(test_station_updates_persist_through_preferences_storage);
  return UNITY_END();
}
