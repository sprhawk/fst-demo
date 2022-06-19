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

shared_ptr<State> Arc::find_or_insert_state(const std::string_view key, const node_value_t value, Fst &fst) {
  cout << "Arc(" << key[0] << ":" << value << ")--->" << endl;

  if (key.size() == 1) {
    // this is the last input, set final state.
    auto final = fst.get_final_state();
    cout << "Final State(" << final->get_id() << ")" << endl;
    this->set_state(final);
    return final;
  } else { // key.size() > 1
    assert(key.size() > 1);
    if (nullptr != this->get_state()) {
      return this->get_state();
    }

    const auto substr = key.substr(1, key.size() - 1);
    shared_ptr<State> end_state = nullptr;
    shared_ptr<Arc> arc;
    
    const auto state = fst.find_or_insert_shared_state(substr, value, arc);
    this->set_state(state);

    // if arc is returned, then there is a shared state
    // exists, so here returns arc's end state instead
    // if (nullptr != arc) {
    //   return arc->get_state();
    // }
    return state;
  }
}

State::State(state_id_t id)
  : _id(id),
    _key('0'), // for root state, but '' char reported error
    _is_final(false)
{
}

State::State(state_id_t id, node_key_t key)
  : _id(id),
    _key(key),
    _is_final(false)
{
}

State::State(state_id_t id, bool is_final)
  : _id(id),
    _key('0'), // for final state actually
    _is_final(is_final){};

shared_ptr<Arc> State::find_or_insert_arc(const string_view key, const node_value_t value, const Fst &fst) {
  assert(key.size() > 0);
  const auto innode = key[0];

  cout << "state(" << _id << ") --- ";

  // _arcs should be sorted, from low to high
  if (_arcs.size() > 0) {
    // do a binary search
    size_t start = 0;
    size_t end = _arcs.size() - 1;
    while(true) {
      const auto middle_pos = start + (end - start) / 2;
      const auto &mid_arc = _arcs[middle_pos];
      const auto mid_key = mid_arc->get_node_key();

      if (innode == mid_key) {
        // follow this arc
        return mid_arc;
      } else if (innode < mid_key) {
        end = middle_pos;
      } else { // innode > node
        start = middle_pos + 1;
      }

      // need to handle 'end == middle_pos'
      if (start > end) { // no found
        auto begin = this->_arcs.begin();
        auto arc = make_shared<Arc>(innode, value);
        if (innode <= mid_key) {
          if (end == 0) {
            this->_arcs.insert(begin, arc);
          } else {
            this->_arcs.insert(begin + end - 1, arc);
          }
        } else {
          this->_arcs.insert(begin + end + 1, arc);
        }

        return arc;
      }
    }
  } else { // _arcs.size() == 0
    auto innode = key[0];
    auto arc = make_shared<Arc>(innode, value);
    this->_arcs.push_back(arc);
    return arc;
    
  }
}

Fst::Fst()
  : _next_id(2) {
  _root_state = make_shared<State>(0);
  _final_state = make_shared<State>(1);
};

Fst::~Fst() {};

void Fst::insert(const string_view key, const node_value_t value) {
  assert(key.size() > 0);
  
  shared_ptr<Arc> arc = nullptr;
  shared_ptr<State> state = _root_state;
  string_view subkey = key;
  node_key_t next_value = value;

  while (subkey.size() > 0) {
    arc = state->find_or_insert_arc(subkey, next_value, *this);
    state = arc->find_or_insert_state(subkey, next_value, *this);
    if (next_value > arc->get_value()) {
      next_value = next_value - arc->get_value();
    } else {
      next_value = 0;
    }
    subkey = subkey.substr(1, subkey.size() - 1);
  }
}

// find shared state or insert a new state
// a shared state means there is a state transition from
// key[0] to key[1] (or final state).
shared_ptr<State> Fst::find_or_insert_shared_state(const string_view key,
                                                   const node_value_t value,
                                                   shared_ptr<Arc> &arc) {
  assert(key.size() > 0);
    
  // finding start state
  shared_ptr<State> start_state;

  if (_intermediate_states.size() == 0) {
    start_state = make_shared<State>(get_and_inc_next_id(), key[0]);
    _intermediate_states.push_back(start_state);
  } else {
    // do a binary search
    size_t start = 0;
    size_t end = _intermediate_states.size() - 1;
    const auto state_key = key[0];
    while(true) {
      const auto middle_pos = start + (end - start) / 2;
      const auto &mid_state = _intermediate_states[middle_pos];
      const auto mid_key = mid_state->get_key();

      if (mid_key == state_key) {
        start_state = mid_state;
        break;
      } else if (mid_key < state_key) {
        if (middle_pos > 0) {
          end = middle_pos - 1;
        } else { // this is a flag that indicate finishing search
          end = middle_pos;
        }
      } else { // state_key > key
        start = middle_pos + 1;
      }
      if (start > end || end == middle_pos) { // no found
        start_state = make_shared<State>(get_and_inc_next_id(), state_key);
        auto begin = this->_intermediate_states.begin();
        if (state_key <= mid_key) {
          if (end == 0) {
            this->_intermediate_states.insert(begin, start_state);
          } else {
            this->_intermediate_states.insert(begin + end - 1, start_state);
          }
        } else {
          this->_intermediate_states.insert(begin + end + 1, start_state);
        }
        break;
      }
    } // while()
  }

  assert(nullptr != start_state);
  arc = start_state->find_or_insert_arc(key, value, *this);
  return start_state;

}

state_id_t Fst::get_next_id() {
  return _next_id;
}

state_id_t Fst::get_and_inc_next_id() {
  auto next_id = get_next_id();
  inc_next_id();
  return next_id;
}
