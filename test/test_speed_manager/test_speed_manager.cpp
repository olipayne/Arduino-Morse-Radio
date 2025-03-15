#include <stdio.h>
#include <unity.h>
#include "SpeedManager.h"

// Mock Arduino functions
int digitalReadValues[100] = {0};

void digitalWrite(int pin, int value) {
  // Mock implementation
}

int digitalRead(int pin) {
  if (pin == Pins::SLOW_DECODE) {
    return digitalReadValues[0];
  } else if (pin == Pins::MED_DECODE) {
    return digitalReadValues[1];
  }
  return HIGH;  // Default to HIGH (not pressed) for other pins
}

void pinMode(int pin, int mode) {
  // Mock implementation
}

void ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution) {
  // Mock implementation
}

void ledcAttachPin(int pin, uint8_t channel) {
  // Mock implementation
}

int ledcWriteValue = 0;
void ledcWrite(uint8_t channel, uint32_t duty) {
  printf("ledcWrite called with channel %d, duty %d\n", channel, duty);
  ledcWriteValue = duty;
}

// Mock ConfigManager
MorseSpeed currentSpeed = MorseSpeed::MEDIUM;

// Create a mock implementation of ConfigManager
class MockConfigManager {
 public:
  static MockConfigManager& getInstance() {
    static MockConfigManager instance;
    return instance;
  }

  void setMorseSpeed(MorseSpeed speed) {
    printf("Setting speed to: %d\n", static_cast<int>(speed));
    currentSpeed = speed;
  }

  MorseSpeed getMorseSpeed() const { return currentSpeed; }
};

// Override ConfigManager::getInstance to return our mock
ConfigManager& ConfigManager::getInstance() {
  return reinterpret_cast<ConfigManager&>(MockConfigManager::getInstance());
}

void setUp(void) {
  // Reset state before each test
  digitalReadValues[0] = HIGH;  // SLOW_DECODE not pressed
  digitalReadValues[1] = HIGH;  // MED_DECODE not pressed
  ledcWriteValue = 0;
}

void tearDown(void) {
  // Clean up after each test
}

void test_speed_manager_default_is_medium(void) {
  // Default state (no switches pressed) should be MEDIUM speed
  digitalReadValues[0] = HIGH;  // SLOW_DECODE not pressed
  digitalReadValues[1] = HIGH;  // MED_DECODE not pressed

  SpeedManager::getInstance().update();

  // For MEDIUM speed, we expect 171 (2.2V - 67% of 3.3V)
  TEST_ASSERT_EQUAL(171, ledcWriteValue);
}

void test_speed_manager_slow_switch(void) {
  // SLOW switch pressed should set SLOW speed
  digitalReadValues[0] = LOW;   // SLOW_DECODE pressed
  digitalReadValues[1] = HIGH;  // MED_DECODE not pressed

  SpeedManager::getInstance().update();

  // For SLOW speed, we expect 85 (1.1V - 33% of 3.3V)
  TEST_ASSERT_EQUAL(85, ledcWriteValue);
}

void test_speed_manager_med_switch_selects_fast(void) {
  // MED switch pressed should set FAST speed (swapped logic)
  digitalReadValues[0] = HIGH;  // SLOW_DECODE not pressed
  digitalReadValues[1] = LOW;   // MED_DECODE pressed

  SpeedManager::getInstance().update();

  // For FAST speed, we expect 255 (3.3V - 100% of 3.3V)
  TEST_ASSERT_EQUAL(255, ledcWriteValue);
}

void test_speed_manager_both_switches_slow_takes_precedence(void) {
  // If both switches are pressed, SLOW should take precedence
  digitalReadValues[0] = LOW;  // SLOW_DECODE pressed
  digitalReadValues[1] = LOW;  // MED_DECODE pressed

  SpeedManager::getInstance().update();

  // For SLOW speed, we expect 85 (1.1V - 33% of 3.3V)
  TEST_ASSERT_EQUAL(85, ledcWriteValue);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_speed_manager_default_is_medium);
  RUN_TEST(test_speed_manager_slow_switch);
  RUN_TEST(test_speed_manager_med_switch_selects_fast);
  RUN_TEST(test_speed_manager_both_switches_slow_takes_precedence);

  return UNITY_END();
}