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
    cout << "Fst next_id: " << fst->get_next_id() << endl;
  }
  void teardown() {
    delete fst;
  }
};

TEST(SimpleGroup, InsertTest) {
  fst->insert("mon", 1);
  auto next_id = fst->get_next_id();
  CHECK_COMPARE(next_id, ==, 4);
}
