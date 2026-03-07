#include <unity.h>

#include "../../src/AudioManager.h"
#include "../../src/Config.h"
#include "../../src/MorseCode.h"

#include "../mocks/AudioProbe.h"
#include "../mocks/HardwareEmulator.h"
#include "../mocks/HardwareEmulator.cpp"
#include "../mocks/AudioManagerTestDouble.cpp"
#include "../../src/MorseCode.cpp"

void setUp() {
  HardwareEmulator::getInstance().reset();
  ConfigManager::getInstance().reset();
  AudioProbe::clear();
  AudioManager::getInstance().begin();
  ConfigManager::getInstance().setMorseSpeed(MorseSpeed::FAST);
  ConfigManager::getInstance().setSpeakerVolume(120);
}

void tearDown() {}

void test_morse_flow_respects_tunein_symbol_and_word_gap_timing() {
  auto& hw = HardwareEmulator::getInstance();
  auto& morse = MorseCode::getInstance();

  morse.begin();
  morse.startMessage("E E");
  AudioProbe::clear();

  morse.update();
  TEST_ASSERT_FALSE(ConfigManager::getInstance().isMorseToneOn());
  TEST_ASSERT_EQUAL(LOW, hw.getPinState(Pins::MORSE_LEDS));
  TEST_ASSERT_EQUAL(1, static_cast<int>(AudioProbe::events().size()));
  TEST_ASSERT_FALSE(AudioProbe::events()[0].toneOn);

  hw.advanceMillis(1000);
  morse.update();
  TEST_ASSERT_TRUE(ConfigManager::getInstance().isMorseToneOn());
  TEST_ASSERT_EQUAL(HIGH, hw.getPinState(Pins::MORSE_LEDS));
  TEST_ASSERT_EQUAL(3, static_cast<int>(AudioProbe::events().size()));
  TEST_ASSERT_FALSE(AudioProbe::events()[1].toneOn);
  TEST_ASSERT_TRUE(AudioProbe::events()[2].toneOn);
  TEST_ASSERT_EQUAL(600, static_cast<int>(hw.getLedcTone(Audio::SPEAKER_CHANNEL)));

  hw.advanceMillis(200);
  morse.update();
  TEST_ASSERT_FALSE(ConfigManager::getInstance().isMorseToneOn());
  TEST_ASSERT_EQUAL(LOW, hw.getPinState(Pins::MORSE_LEDS));
  TEST_ASSERT_EQUAL(4, static_cast<int>(AudioProbe::events().size()));
  TEST_ASSERT_FALSE(AudioProbe::events()[3].toneOn);
  TEST_ASSERT_EQUAL(1200UL, AudioProbe::events()[3].timestamp);

  hw.advanceMillis(800);
  morse.update();
  TEST_ASSERT_FALSE(ConfigManager::getInstance().isMorseToneOn());

  hw.advanceMillis(1600);
  morse.update();
  TEST_ASSERT_TRUE(ConfigManager::getInstance().isMorseToneOn());
  TEST_ASSERT_EQUAL(6, static_cast<int>(AudioProbe::events().size()));
  TEST_ASSERT_FALSE(AudioProbe::events()[4].toneOn);
  TEST_ASSERT_TRUE(AudioProbe::events()[5].toneOn);
  TEST_ASSERT_EQUAL(3600UL, AudioProbe::events()[5].timestamp);

  morse.stop();
  TEST_ASSERT_FALSE(ConfigManager::getInstance().isMorsePlaying());
  TEST_ASSERT_FALSE(ConfigManager::getInstance().isMorseToneOn());
  TEST_ASSERT_EQUAL(LOW, hw.getPinState(Pins::MORSE_LEDS));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_morse_flow_respects_tunein_symbol_and_word_gap_timing);
  return UNITY_END();
}
