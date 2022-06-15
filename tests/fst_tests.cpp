#include <CppUTest/TestHarness.h>

#include <memory>
#include "fst/fst.hpp"

using namespace std;
using namespace Fst;
TEST_GROUP(SimpleGroup) {
  void setup() {
    unique_ptr<FstNode> ptr(new FstNode('a', 1));
  }
  void teardown() {}
};

TEST(SimpleGroup, FirstTest) { FAIL("Failed me"); }
