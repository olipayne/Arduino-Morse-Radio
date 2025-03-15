#include <unity.h>
#include "Config.h"

void setUp(void) {
  // No setup needed
}

void tearDown(void) {
  // No cleanup needed
}

void test_morse_slow_timings(void) {
  const auto& timings = Audio::MORSE_SLOW;
  TEST_ASSERT_EQUAL(500, timings.dotDuration);
  TEST_ASSERT_EQUAL(1500, timings.dashDuration);  // 3x dot
  TEST_ASSERT_EQUAL(500, timings.symbolGap);      // 1x dot
  TEST_ASSERT_EQUAL(2000, timings.letterGap);     // 4x dot
  TEST_ASSERT_EQUAL(3500, timings.wordGap);       // 7x dot
}

void test_morse_medium_timings(void) {
  const auto& timings = Audio::MORSE_MEDIUM;
  TEST_ASSERT_EQUAL(350, timings.dotDuration);
  TEST_ASSERT_EQUAL(1050, timings.dashDuration);  // 3x dot
  TEST_ASSERT_EQUAL(350, timings.symbolGap);      // 1x dot
  TEST_ASSERT_EQUAL(1400, timings.letterGap);     // 4x dot
  TEST_ASSERT_EQUAL(2450, timings.wordGap);       // 7x dot
}

void test_morse_fast_timings(void) {
  const auto& timings = Audio::MORSE_FAST;
  TEST_ASSERT_EQUAL(200, timings.dotDuration);
  TEST_ASSERT_EQUAL(600, timings.dashDuration);  // 3x dot
  TEST_ASSERT_EQUAL(200, timings.symbolGap);     // 1x dot
  TEST_ASSERT_EQUAL(800, timings.letterGap);     // 4x dot
  TEST_ASSERT_EQUAL(1400, timings.wordGap);      // 7x dot
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_morse_slow_timings);
  RUN_TEST(test_morse_medium_timings);
  RUN_TEST(test_morse_fast_timings);

  return UNITY_END();
}