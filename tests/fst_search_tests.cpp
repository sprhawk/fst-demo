#include <CppUTest/TestHarness.h>

#include <iostream>
#include <memory>
#include <vector>
#include "fst/fst.hpp"

using namespace std;
using namespace FstN;

TEST_GROUP(SearchGroup) {
  Fst *fst = nullptr;
  void setup() {
    fst = new Fst();
    fst->insert("monday", 1);  
    fst->insert("tuesday", 2);
    fst->insert("wednesday", 3);
    fst->insert("thursday", 4);
    fst->insert("friday", 5);
    fst->insert("saturday", 6);
    fst->insert("sunday", 7);
  }
  void teardown() {
    delete fst;
  }
};

TEST(SearchGroup, ExactSearch) {
  vector<shared_ptr<SearchResult>> results;

  fst->search("mon", results);
  CHECK_COMPARE(results.size(), ==, 0); // no found

  results.clear();
  fst->search("monday", results);
  CHECK_COMPARE(results.size(), ==, 1);

  auto monday = results[0];
  CHECK_COMPARE(monday->get_key().compare("monday"), ==, 0);
}
