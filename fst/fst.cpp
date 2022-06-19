//
#include <stdexcept>
#include <cassert>
#include <iostream>

#include "fst.hpp"

using namespace std;
using namespace FstN;

Arc::Arc(node_key_t nodekey, node_value_t value)
  : _nodekey(nodekey),
    _value(value),
    _end_state(nullptr) {
  
}

Arc::~Arc() {}

void Arc::insert_state(std::string_view key, node_value_t value, Fst &fst) {
  if (key.size() == 1) {
    // this is the last input, set final state.
    auto final = fst.get_final_state();
    cout << "Final State(" << final->get_id() << ")" << endl;
    this->set_state(final);
  } else {
    auto state = make_shared<State>(fst.get_and_inc_next_id());
    this->set_state(state);
    auto substr = key.substr(1, key.size() - 1);
    state->insert_arc(substr, value, fst);
  }
}

State::State(state_id_t id)
  : _id(id),
    _is_final(false)
{
}

State::State(state_id_t id, bool is_final)
  : _id(id),
    _is_final(is_final){};

void State::insert_arc(string_view key, node_value_t value, Fst &fst) {
  if (key.size() == 0) {
    throw invalid_argument("empty key");
  }

  cout << "state(" << _id << ") --- ";

  // _arcs should be sorted, from low to high
  if (_arcs.size() > 0) {
    // do a binary search
    const auto innode = key[0];

    size_t start = 0;
    size_t end = _arcs.size() - 1;
    while(true) {
      auto middle_pos = start + (end - start) / 2;
      auto &mid_arc = _arcs[middle_pos];
      auto node = mid_arc->get_node_key();

      if (innode == node) {
        // follow this arc
        mid_arc->insert_state(key, value, fst);
        break;
      } else if (innode < node) {
        end = middle_pos;
      } else { // innode > node
        start = middle_pos + 1;
      }

      if (start >= end) { // no found
        auto begin = this->_arcs.begin();
        cout << "Arc(" << innode << ":" << value << ")--->" << endl;
        auto arc = make_shared<Arc>(innode, value);
        arc->insert_state(key, 0, fst);
        if (innode <= node) {
          if (end == 0) {
            this->_arcs.insert(begin, arc);
          } else {
            this->_arcs.insert(begin + end - 1, arc);
          }
        } else {
          this->_arcs.insert(begin + end + 1, arc);
        }

        break;
      } else {
        continue; // next search
      }
    }
  } else { // _arcs.size() == 0
    auto innode = key[0];
    cout << "Arc(" << innode << ":" << value << ")--->" << endl;
    auto arc = make_shared<Arc>(innode, value);
    arc->insert_state(key, 0, fst);
    this->_arcs.push_back(arc);
  }
}

Fst::Fst()
  : _next_id(2) {
  _root_state = make_unique<State>(0);
  _final_state = make_shared<State>(1);
};

Fst::~Fst() {};

void Fst::insert(string_view key, int value) {
  _root_state->insert_arc(key, value, *this);
}

state_id_t Fst::get_next_id() {
  return _next_id;
};
state_id_t Fst::get_and_inc_next_id() {
  auto next_id = get_next_id();
  inc_next_id();
  return next_id;
};
