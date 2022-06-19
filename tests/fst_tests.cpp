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

TEST(SimpleGroup, InsertMondayTest) {
  fst->insert("mon", 1);
  auto next_id = fst->get_next_id();
  CHECK_COMPARE(next_id, ==, 4);
}

TEST(SimpleGroup, InsertMoreDaysTest) {
  fst->insert("mon", 1);  
  fst->insert("tues", 2);
  fst->insert("thurs", 4);
  auto next_id = fst->get_next_id();
  CHECK_COMPARE(next_id, ==, 9);
}

