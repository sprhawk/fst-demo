#include <CppUTest/TestHarness.h>

TEST_GROUP(SimpleGroup) {
  void setup() {}
  void teardown() {}
};

TEST(SimpleGroup, FirstTest) { FAIL("Failed me"); }
