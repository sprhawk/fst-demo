#include <CppUTest/TestHarness.h>

#include <iostream>
#include <memory>
#include "fst/fst.hpp"

using namespace std;
using namespace FstN;
TEST_GROUP(SimpleGroup) {
  Fst *fst = nullptr;
  void setup() {
    fst = new Fst();
  }
  void teardown() {
    delete fst;
  }
};

TEST(SimpleGroup, InsertOneTest) {
  fst->insert("mon", 1);
  auto next_id = fst->get_next_id();
  CHECK_COMPARE(next_id, ==, 4);
}

TEST(SimpleGroup, InsertTwoDaysNoPrefixNoSuffixTest) {
  fst->insert("mon", 1);  
  fst->insert("thurs", 2);
  auto next_id = fst->get_next_id();
  CHECK_COMPARE(next_id, ==, 8);
}

TEST(SimpleGroup, InsertThreeDaysTest) {
  fst->insert("mon", 1);  
  fst->insert("thurs", 4);
  fst->insert("tues", 2);
  auto next_id = fst->get_next_id();
  CHECK_COMPARE(next_id, ==, 9);
}

