#include <unity.h>
#include "Config.h"
#include "ConfigManager.h"
#include "WaveBandManager.h"

// Mock Arduino functions
int _digitalRead_values[50] = {0};
int digitalRead(int pin) { return _digitalRead_values[pin]; }

void digitalWrite(int pin, int value) {
  // Mock implementation
}

void pinMode(int pin, int mode) {
  // Mock implementation
}

// Mock ConfigManager implementation
ConfigManager& ConfigManager::getInstance() {
  static ConfigManager instance;
  return instance;
}

// Test the WaveBandManager update method with the swapped logic
void test_wave_band_manager_update_swapped_logic() {
  // Reset the WaveBandManager
  WaveBandManager::getInstance().begin();

  // Test with LW switch pressed (LOW)
  _digitalRead_values[Pins::LW_BAND_SWITCH] = LOW;
  _digitalRead_values[Pins::MW_BAND_SWITCH] = HIGH;
  WaveBandManager::getInstance().update();
  TEST_ASSERT_EQUAL(WaveBand::LONG_WAVE, ConfigManager::getInstance().getWaveBand());

  // Test with MW switch pressed (LOW) - should select SHORT_WAVE with swapped logic
  _digitalRead_values[Pins::LW_BAND_SWITCH] = HIGH;
  _digitalRead_values[Pins::MW_BAND_SWITCH] = LOW;
  WaveBandManager::getInstance().update();
  TEST_ASSERT_EQUAL(WaveBand::SHORT_WAVE, ConfigManager::getInstance().getWaveBand());

  // Test with no switch pressed (both HIGH) - should default to MEDIUM_WAVE with swapped logic
  _digitalRead_values[Pins::LW_BAND_SWITCH] = HIGH;
  _digitalRead_values[Pins::MW_BAND_SWITCH] = HIGH;
  WaveBandManager::getInstance().update();
  TEST_ASSERT_EQUAL(WaveBand::MEDIUM_WAVE, ConfigManager::getInstance().getWaveBand());

  // Test with both switches pressed (both LOW) - should prioritize LW
  _digitalRead_values[Pins::LW_BAND_SWITCH] = LOW;
  _digitalRead_values[Pins::MW_BAND_SWITCH] = LOW;
  WaveBandManager::getInstance().update();
  TEST_ASSERT_EQUAL(WaveBand::LONG_WAVE, ConfigManager::getInstance().getWaveBand());
}

// Test the WaveBandManager getCurrentBand method
void test_wave_band_manager_get_current_band() {
  // Set the band in the ConfigManager
  ConfigManager::getInstance().setWaveBand(WaveBand::LONG_WAVE);
  TEST_ASSERT_EQUAL(WaveBand::LONG_WAVE, WaveBandManager::getInstance().getCurrentBand());

  ConfigManager::getInstance().setWaveBand(WaveBand::MEDIUM_WAVE);
  TEST_ASSERT_EQUAL(WaveBand::MEDIUM_WAVE, WaveBandManager::getInstance().getCurrentBand());

  ConfigManager::getInstance().setWaveBand(WaveBand::SHORT_WAVE);
  TEST_ASSERT_EQUAL(WaveBand::SHORT_WAVE, WaveBandManager::getInstance().getCurrentBand());
}

void setUp(void) {
  // Reset before each test
  for (int i = 0; i < 50; i++) {
    _digitalRead_values[i] = HIGH;  // Default to HIGH (not pressed)
  }
  ConfigManager::getInstance().setWaveBand(WaveBand::SHORT_WAVE);
}

void tearDown(void) {
  // Clean up after each test
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_wave_band_manager_update_swapped_logic);
  RUN_TEST(test_wave_band_manager_get_current_band);

  return UNITY_END();
}