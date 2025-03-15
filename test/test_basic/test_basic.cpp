#include <unity.h>

void test_basic_addition() { TEST_ASSERT_EQUAL(4, 2 + 2); }

void test_basic_subtraction() { TEST_ASSERT_EQUAL(2, 4 - 2); }

void setUp(void) {
  // Set up code if needed
}

void tearDown(void) {
  // Tear down code if needed
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_basic_addition);
  RUN_TEST(test_basic_subtraction);

  return UNITY_END();
}