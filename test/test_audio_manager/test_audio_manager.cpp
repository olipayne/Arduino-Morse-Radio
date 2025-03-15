#include <unity.h>
#include "AudioManager.h"
#include "Config.h"

// Mock Arduino functions and variables
unsigned long _millis = 0;
unsigned long millis() { return _millis; }

int _ledcRead_value = 0;
int ledcRead(uint8_t channel) { return _ledcRead_value; }

void ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution) {
  // Mock implementation
}

void ledcAttachPin(int pin, uint8_t channel) {
  // Mock implementation
}

void ledcDetachPin(int pin) {
  // Mock implementation
}

void ledcWrite(uint8_t channel, uint32_t duty) {
  // Mock implementation
}

void ledcWriteTone(uint8_t channel, uint32_t freq) {
  // Mock implementation
}

int random(int min, int max) {
  // Deterministic "random" for testing
  return min;
}

// Mock PowerManager for ADC readings
class MockPowerManager {
 public:
  static MockPowerManager& getInstance() {
    static MockPowerManager instance;
    return instance;
  }

  int adcValue = 0;

  int readADC(int pin) { return adcValue; }
};

PowerManager& PowerManager::getInstance() {
  return reinterpret_cast<PowerManager&>(MockPowerManager::getInstance());
}

// Define Radio namespace constants for testing
namespace Radio {
constexpr int ADC_MAX = 4095;
}

// Test the AudioManager calculateVolumeLevel method
void test_audio_manager_calculate_volume_level() {
  // We need to access the private calculateVolumeLevel method
  // This is a bit of a hack for testing purposes
  class TestAudioManager : public AudioManager {
   public:
    static int calculateVolumeLevelTest(int adcValue) {
      return getInstance().calculateVolumeLevel(adcValue);
    }
  };

  // Test with ADC value below dead zone
  TEST_ASSERT_EQUAL(0, TestAudioManager::calculateVolumeLevelTest(0));
  TEST_ASSERT_EQUAL(0, TestAudioManager::calculateVolumeLevelTest(49));

  // Test with ADC value at dead zone boundary
  TEST_ASSERT_EQUAL(0, TestAudioManager::calculateVolumeLevelTest(50));

  // Test with ADC value above dead zone
  TEST_ASSERT_EQUAL(1, TestAudioManager::calculateVolumeLevelTest(51));

  // Test with maximum ADC value
  TEST_ASSERT_EQUAL(255, TestAudioManager::calculateVolumeLevelTest(4095));

  // Test with mid-range ADC value
  int midVolume = TestAudioManager::calculateVolumeLevelTest(2048);
  TEST_ASSERT_TRUE(midVolume > 0);
  TEST_ASSERT_TRUE(midVolume < 255);
}

// Test the AudioManager setVolume method
void test_audio_manager_set_volume() {
  // Reset the AudioManager
  AudioManager::getInstance().begin();

  // Test with volume below dead zone
  AudioManager::getInstance().setVolume(0);

  // Test with volume above dead zone
  _ledcRead_value = 100;  // Simulate that we're making sound
  AudioManager::getInstance().setVolume(2048);

  // Test with maximum volume
  AudioManager::getInstance().setVolume(4095);
}

// Test the AudioManager playMorseTone and stopMorseTone methods
void test_audio_manager_morse_tone() {
  // Reset the AudioManager
  AudioManager::getInstance().begin();

  // Test playing morse tone
  AudioManager::getInstance().playMorseTone();

  // Test stopping morse tone
  AudioManager::getInstance().stopMorseTone();
}

// Test the AudioManager playStaticNoise method
void test_audio_manager_static_noise() {
  // Reset the AudioManager
  AudioManager::getInstance().begin();

  // Test with no signal
  AudioManager::getInstance().playStaticNoise(0);

  // Test with weak signal
  AudioManager::getInstance().playStaticNoise(50);

  // Test with strong signal
  AudioManager::getInstance().playStaticNoise(200);

  // Test with maximum signal
  AudioManager::getInstance().playStaticNoise(255);
}

// Test the AudioManager stop method
void test_audio_manager_stop() {
  // Reset the AudioManager
  AudioManager::getInstance().begin();

  // Test stopping audio
  AudioManager::getInstance().stop();
}

// Test the AudioManager handlePlayback method
void test_audio_manager_handle_playback() {
  // Reset the AudioManager
  AudioManager::getInstance().begin();

  // Set up mock ADC reading
  MockPowerManager::getInstance().adcValue = 2048;

  // Test handling playback
  AudioManager::getInstance().handlePlayback();

  // Advance time and test again
  _millis += 20;
  AudioManager::getInstance().handlePlayback();
}

void setUp(void) {
  // Reset before each test
  _millis = 0;
  _ledcRead_value = 0;
  MockPowerManager::getInstance().adcValue = 0;
}

void tearDown(void) {
  // Clean up after each test
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_audio_manager_calculate_volume_level);
  RUN_TEST(test_audio_manager_set_volume);
  RUN_TEST(test_audio_manager_morse_tone);
  RUN_TEST(test_audio_manager_static_noise);
  RUN_TEST(test_audio_manager_stop);
  RUN_TEST(test_audio_manager_handle_playback);

  return UNITY_END();
}