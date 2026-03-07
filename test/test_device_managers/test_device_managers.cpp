#include <unity.h>

#include "../../src/Config.h"
#include "../../src/SignalManager.h"
#include "../../src/SpeedManager.h"
#include "../../src/WaveBandManager.h"

#include "../mocks/HardwareEmulator.h"
#include "../mocks/HardwareEmulator.cpp"

void setUp() {
  auto& hw = HardwareEmulator::getInstance();
  hw.reset();
  ConfigManager::getInstance().reset();
}

void tearDown() {}

void test_speed_manager_updates_state_and_skips_redundant_pwm_write() {
  auto& hw = HardwareEmulator::getInstance();
  auto& speed = SpeedManager::getInstance();
  auto& config = ConfigManager::getInstance();

  speed.begin();

  hw.setPinState(Pins::SLOW_DECODE, HIGH);
  hw.setPinState(Pins::MED_DECODE, HIGH);

  speed.update();
  TEST_ASSERT_EQUAL(static_cast<int>(MorseSpeed::MEDIUM), static_cast<int>(config.getMorseSpeed()));
  TEST_ASSERT_EQUAL(171, static_cast<int>(hw.getLedcDuty(PWMChannels::DECODE)));

  uint32_t writesBefore = hw.getLedcWriteCount(PWMChannels::DECODE);
  speed.update();
  TEST_ASSERT_EQUAL_UINT32(writesBefore, hw.getLedcWriteCount(PWMChannels::DECODE));

  hw.setPinState(Pins::MED_DECODE, LOW);
  speed.update();
  TEST_ASSERT_EQUAL(static_cast<int>(MorseSpeed::FAST), static_cast<int>(config.getMorseSpeed()));
  TEST_ASSERT_EQUAL(255, static_cast<int>(hw.getLedcDuty(PWMChannels::DECODE)));
}

void test_wave_band_manager_updates_band_and_leds() {
  auto& hw = HardwareEmulator::getInstance();
  auto& manager = WaveBandManager::getInstance();
  auto& config = ConfigManager::getInstance();

  manager.begin();

  hw.setPinState(Pins::LW_BAND_SWITCH, LOW);
  hw.setPinState(Pins::MW_BAND_SWITCH, HIGH);
  manager.update();

  TEST_ASSERT_EQUAL(static_cast<int>(WaveBand::LONG_WAVE), static_cast<int>(config.getWaveBand()));
  TEST_ASSERT_EQUAL(HIGH, hw.getPinState(Pins::LW_LED));
  TEST_ASSERT_EQUAL(LOW, hw.getPinState(Pins::MW_LED));
  TEST_ASSERT_EQUAL(LOW, hw.getPinState(Pins::SW_LED));
}

void test_signal_manager_clamps_and_deduplicates_updates() {
  auto& hw = HardwareEmulator::getInstance();
  auto& signal = SignalManager::getInstance();

  signal.begin();

  signal.updateSignalStrength(-10);
  TEST_ASSERT_EQUAL(0, static_cast<int>(hw.getLedcDuty(PWMChannels::CARRIER)));
  uint32_t writesBefore = hw.getLedcWriteCount(PWMChannels::CARRIER);

  signal.updateSignalStrength(0);
  TEST_ASSERT_EQUAL_UINT32(writesBefore, hw.getLedcWriteCount(PWMChannels::CARRIER));

  signal.updateSignalStrength(255);
  TEST_ASSERT_EQUAL(255, static_cast<int>(hw.getLedcDuty(PWMChannels::CARRIER)));

  signal.updateLockStatus(true);
  TEST_ASSERT_EQUAL(HIGH, hw.getPinState(Pins::LOCK_LED));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_speed_manager_updates_state_and_skips_redundant_pwm_write);
  RUN_TEST(test_wave_band_manager_updates_band_and_leds);
  RUN_TEST(test_signal_manager_clamps_and_deduplicates_updates);
  return UNITY_END();
}
