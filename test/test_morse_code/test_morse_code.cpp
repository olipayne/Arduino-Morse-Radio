#include <unity.h>
#include <string>
#include "MorseCode.h"

// Mock classes for testing
class MockConfigManager {
 public:
  static MockConfigManager& getInstance() {
    static MockConfigManager instance;
    return instance;
  }

  bool morsePlaying = false;
  bool morseToneOn = false;
  MorseSpeed morseSpeed = MorseSpeed::MEDIUM;

  void setMorsePlaying(bool playing) { morsePlaying = playing; }
  void setMorseToneOn(bool on) { morseToneOn = on; }
  bool isMorsePlaying() const { return morsePlaying; }
  bool isMorseToneOn() const { return morseToneOn; }
  void setMorseSpeed(MorseSpeed speed) { morseSpeed = speed; }
  MorseSpeed getMorseSpeed() const { return morseSpeed; }

  const Audio::MorseTimings& getCurrentMorseTimings() const {
    static Audio::MorseTimings timings = {100, 300, 100, 300, 700};
    return timings;
  }
};

class MockAudioManager {
 public:
  static MockAudioManager& getInstance() {
    static MockAudioManager instance;
    return instance;
  }

  bool playingMorseTone = false;

  void playMorseTone() { playingMorseTone = true; }
  void stopMorseTone() { playingMorseTone = false; }
};

// Mock Arduino functions
unsigned long _millis = 0;
unsigned long millis() { return _millis; }

void digitalWrite(int pin, int value) {
  // Mock implementation
}

void pinMode(int pin, int mode) {
  // Mock implementation
}

// Test the MorseCode getSymbol method
void test_morse_code_get_symbol() {
  // We need to access the private getSymbol method
  // This is a bit of a hack for testing purposes
  class TestMorseCode : public MorseCode {
   public:
    static String getSymbolTest(char c) { return getInstance().getSymbol(c); }
  };

  // Test some letters
  TEST_ASSERT_EQUAL_STRING(".-", TestMorseCode::getSymbolTest('A').c_str());
  TEST_ASSERT_EQUAL_STRING("-...", TestMorseCode::getSymbolTest('B').c_str());
  TEST_ASSERT_EQUAL_STRING(".", TestMorseCode::getSymbolTest('E').c_str());

  // Test some numbers
  TEST_ASSERT_EQUAL_STRING("-----", TestMorseCode::getSymbolTest('0').c_str());
  TEST_ASSERT_EQUAL_STRING(".----", TestMorseCode::getSymbolTest('1').c_str());

  // Test case insensitivity
  TEST_ASSERT_EQUAL_STRING(".-", TestMorseCode::getSymbolTest('a').c_str());

  // Test space
  TEST_ASSERT_EQUAL_STRING(" ", TestMorseCode::getSymbolTest(' ').c_str());

  // Test unsupported character
  TEST_ASSERT_EQUAL_STRING("", TestMorseCode::getSymbolTest('!').c_str());
}

// Test the MorseCode startMessage and stop methods
void test_morse_code_start_stop() {
  // Reset mock objects
  MockConfigManager::getInstance().morsePlaying = false;
  MockConfigManager::getInstance().morseToneOn = false;
  MockAudioManager::getInstance().playingMorseTone = false;

  // Start a message
  MorseCode::getInstance().startMessage("TEST");

  // Check that the message is playing
  TEST_ASSERT_TRUE(MockConfigManager::getInstance().morsePlaying);
  TEST_ASSERT_FALSE(MockConfigManager::getInstance().morseToneOn);

  // Stop the message
  MorseCode::getInstance().stop();

  // Check that the message is not playing
  TEST_ASSERT_FALSE(MockConfigManager::getInstance().morsePlaying);
  TEST_ASSERT_FALSE(MockConfigManager::getInstance().morseToneOn);
  TEST_ASSERT_FALSE(MockAudioManager::getInstance().playingMorseTone);
}

// Test the MorseCode isPlaying method
void test_morse_code_is_playing() {
  // Reset mock objects
  MockConfigManager::getInstance().morsePlaying = false;

  // Check that the message is not playing
  TEST_ASSERT_FALSE(MorseCode::getInstance().isPlaying());

  // Set the message to playing
  MockConfigManager::getInstance().morsePlaying = true;

  // Check that the message is playing
  TEST_ASSERT_TRUE(MorseCode::getInstance().isPlaying());
}

// Test the MorseCode setSpeed method
void test_morse_code_set_speed() {
  // Reset mock objects
  MockConfigManager::getInstance().morseSpeed = MorseSpeed::MEDIUM;

  // Set the speed to SLOW
  MorseCode::getInstance().setSpeed(MorseSpeed::SLOW);

  // Check that the speed was set
  TEST_ASSERT_EQUAL(MorseSpeed::SLOW, MockConfigManager::getInstance().getMorseSpeed());

  // Set the speed to FAST
  MorseCode::getInstance().setSpeed(MorseSpeed::FAST);

  // Check that the speed was set
  TEST_ASSERT_EQUAL(MorseSpeed::FAST, MockConfigManager::getInstance().getMorseSpeed());
}

void setUp(void) {
  // Reset mock objects before each test
  MockConfigManager::getInstance().morsePlaying = false;
  MockConfigManager::getInstance().morseToneOn = false;
  MockConfigManager::getInstance().morseSpeed = MorseSpeed::MEDIUM;
  MockAudioManager::getInstance().playingMorseTone = false;
  _millis = 0;
}

void tearDown(void) {
  // Clean up after each test
}

// Override the ConfigManager and AudioManager getInstance methods for testing
namespace std {
template <>
struct remove_reference<ConfigManager&> {
  typedef MockConfigManager type;
};

template <>
struct remove_reference<AudioManager&> {
  typedef MockAudioManager type;
};
}  // namespace std

ConfigManager& ConfigManager::getInstance() {
  return reinterpret_cast<ConfigManager&>(MockConfigManager::getInstance());
}

AudioManager& AudioManager::getInstance() {
  return reinterpret_cast<AudioManager&>(MockAudioManager::getInstance());
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_morse_code_get_symbol);
  RUN_TEST(test_morse_code_start_stop);
  RUN_TEST(test_morse_code_is_playing);
  RUN_TEST(test_morse_code_set_speed);

  return UNITY_END();
}