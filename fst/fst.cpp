//
#include <stdexcept>
#include <cassert>
#include <iostream>

#include "fst.hpp"

using namespace std;
using namespace FstN;

#define END_OF_STRING '\0'

Arc::Arc(node_key_t nodekey, node_value_t value, string_view path)
  : _nodekey(nodekey),
    _value(value),
    _path(path),
    _end_state(nullptr) {
  
}

Arc::~Arc() {}

void Arc::update_value_recursively(const node_value_t value) {
  
  if (this->get_value() > value && nullptr != this->get_state()) {
    const auto rem = this->get_value() - value;
    this->get_state()->update_value_recursively(_path, rem);
    this->update_value(value);
  }
}

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

    shared_ptr<Arc> arc;
    
    const auto state = fst.find_or_insert_shared_state(key, value, arc);
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

void State::update_value_recursively(const string_view key, const node_value_t value) {
  auto arc = find_or_insert_arc(key, value);
  arc->update_value_recursively(value);
}
shared_ptr<Arc> State::find_or_insert_arc(const string_view key, const node_value_t value) {
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
        mid_arc->update_value_recursively(value);
        return mid_arc;
      } else if (innode < mid_key) {
        if (middle_pos > 0) {
          end = middle_pos - 1;
        } else { // this is a flag that indicate finishing search
          end = middle_pos;
        }
      } else { // innode > node
        start = middle_pos + 1;
      }

      // need to handle 'end == middle_pos'
      if (start > end || end == middle_pos) { // no found
        auto begin = this->_arcs.begin();
        auto arc = make_shared<Arc>(innode, value, key);
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
    auto arc = make_shared<Arc>(innode, value, key);
    this->_arcs.push_back(arc);
    return arc;
    
  }
}

const shared_ptr<State> State::find_next_state(const node_key_t key) {
  size_t start = 0;
  size_t end = this->_arcs.size() - 1;
  const auto state_key = key;

  while(true) {
      const auto middle_pos = start + (end - start) / 2;
      const auto &mid_arc = this->_arcs[middle_pos];
      const auto mid_key = mid_arc->get_node_key();

      if (mid_key == state_key) {
        return mid_arc->get_state();
      } else if (mid_key > state_key) {
        if (middle_pos > 0) {
          end = middle_pos - 1;
        } else { // this is a flag that indicate finishing search
          end = middle_pos;
        }
      } else { // state_key < key
        start = middle_pos + 1;
      }
      if (start > end || end == middle_pos) { // no found
        return nullptr;
      }
    } // while()
}

// recursively calls
void State::search_key(const string_view key,
                       vector<shared_ptr<SearchResult>> &results,
                       string parent_key,
                       node_value_t parent_value) {
  if (key.size() > 0 && this->_arcs.size() > 0) {
    // bookkeeping candidates states to call recursively
    vector<shared_ptr<Arc>> next_arcs;
    node_key_t next_non_star_char = END_OF_STRING;
    size_t next_non_star_char_index = 0;
    bool wild_search;
    const auto innode = key[0];
    if ('*' == innode) {
      wild_search = true;
      auto index = 0;
      while (index < key.size() - 1) {
        if ('*' != key[index]) {
          next_non_star_char_index = index;
          next_non_star_char = key[index];
          
        }
      }
    } else {
      wild_search = false;
    }

    // if this is a wild search, bookkeeps all the children states
    if (wild_search) {
      for (auto i = 0; i < this->_arcs.size(); i++) {
        auto arc = this->_arcs[i];
        next_arcs.push_back(arc);
      }
    } else {
      // if this is not a wild search, just do a binary search
      // do a binary search
      size_t start = 0;
      size_t end = _arcs.size() - 1;
      while(true) {
        const auto middle_pos = start + (end - start) / 2;
        const auto &mid_arc = _arcs[middle_pos];
        const auto mid_key = mid_arc->get_node_key();

        if (innode == mid_key) {
          next_arcs.push_back(mid_arc);
          break;
        } else if (innode < mid_key) {
          if (middle_pos > 0) {
            end = middle_pos - 1;
          } else { // this is a flag that indicate finishing search
            end = middle_pos;
          }
        } else { // innode > node
          start = middle_pos + 1;
        }

        // need to handle 'end == middle_pos'
        if (start > end || end == middle_pos) { // no found
          break;
        }
      }
    }

    for (auto i = 0; i < next_arcs.size(); i++) {
      auto arc = next_arcs[i];
      if (END_OF_STRING != next_non_star_char
          && arc->get_node_key() == next_non_star_char) {
        // if this arc has the non_star_char, it will move without this * prefix
        const string_view subkey = key.substr(next_non_star_char_index, key.size() - next_non_star_char_index);
        arc->get_state()->search_key(subkey, results, parent_key + arc->get_node_key(), parent_value + arc->get_value());
      } else {
        if (wild_search) {
          // if this is a wild_search and not a final state, contiunue search
          if (is_final()) {
            auto result = make_shared<SearchResult>(parent_key, parent_value);
            results.push_back(result);
          } else {
            arc->get_state()->search_key(key, results,
                                         parent_key + arc->get_node_key(),
                                         parent_value + arc->get_value());
          }
        } else {
          const string_view subkey = key.substr(1, key.size() - 1);
          arc->get_state()->search_key(subkey, results,
                                       parent_key + arc->get_node_key(),
                                       parent_value + arc->get_value());
        }
      }
    }
  } else { // if(key.size() > 0 && _arcs.size() > 0)
    // if here is end of search
    if (is_final()) {
      auto result = make_shared<SearchResult>(parent_key, parent_value);
      results.push_back(result);
    }
  }
}

Fst::Fst()
  : _next_id(2) {
  _root_state = make_shared<State>(0);
  _final_state = make_shared<State>(1, true);
};

Fst::~Fst() {};

void Fst::insert(const string_view key, const node_value_t value) {
  assert(key.size() > 0);
  cout << "inserting: \"" << key << "\" value: " << value << endl;
  shared_ptr<Arc> arc = nullptr;
  shared_ptr<State> state = _root_state;
  string_view subkey = key;
  node_value_t next_value = value;

  while (subkey.size() > 0) {
    arc = state->find_or_insert_arc(subkey, next_value);
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
      auto &mid_state = _intermediate_states[middle_pos];
      // mid_key will be kept during insertion, should not be changed.
      const auto mid_key = mid_state->get_key();

      if (mid_key == state_key) {
        // only find the same state that follow same
        // state(key[0] -- arc --> next_state(key[1] or final)
        const auto next_key = key.size() > 1 ? key[1] : '\0';
        shared_ptr<State> end_state;
        auto index = middle_pos;

        // backward search for states with same state_key
        do {
          end_state = mid_state->find_next_state(next_key);
          if (end_state != nullptr) {
            // found a shared state ( same start to end state )
            start_state = mid_state;
            break;
          }
          if (index == 0) {
            break;
          }
          index--;
          mid_state = _intermediate_states[index];

        }while(mid_state->get_key() == state_key);

        // not found state in backward search
        if(nullptr == end_state) {
          // then do a forward search after middle_pos
          for(auto index = middle_pos + 1; index < _intermediate_states.size(); index ++) {
            auto &mid_state = _intermediate_states[index];
            if (mid_state->get_key() != state_key) {
              // finishes forward search
              break;
            }

            end_state = mid_state->find_next_state(next_key);
            if (end_state != nullptr) {
              // found a shared state
              start_state = mid_state;
              break;
            }
          }
        }

      } else if (mid_key > state_key) {
        if (middle_pos > 0) {
          end = middle_pos - 1;
        } else { // this is a flag that indicate finishing search
          end = middle_pos;
        }
      } else { // state_key < key
        start = middle_pos + 1;
      }
      // when start_state is nullptr and mid_key == state_key execution goes here
      // already found shared state
      if (nullptr != start_state) {
        break;
      }
      // then there is no sharable state exists, then create a new start_state
      if ((nullptr == start_state && mid_key == state_key) ||
          (start > end || end == middle_pos)) { // no sharable state found
        start_state = make_shared<State>(get_and_inc_next_id(), state_key);
        auto begin = this->_intermediate_states.begin();

        if (state_key == mid_key) {
          this->_intermediate_states.insert(begin + middle_pos, start_state);
        } else if (state_key < mid_key) {
          auto insertion_pos = middle_pos;
          while (insertion_pos > 0) {
            auto prev = this->_intermediate_states[insertion_pos - 1];
            if (prev->get_key() != mid_key) {
              break;
            }
            insertion_pos--;
          }
          this->_intermediate_states.insert(begin + insertion_pos, start_state);
        } else { // state_key > mid_key
          auto insertion_pos = middle_pos + 1;
          while (insertion_pos < this->_intermediate_states.size()) {
            auto next = this->_intermediate_states[insertion_pos + 1];
            if (next->get_key() != mid_key) {
              break;
            }
            insertion_pos ++;
          }
          this->_intermediate_states.insert(begin + insertion_pos, start_state);
        }
        break;
      }
    } // while()
  }

  assert(nullptr != start_state);
  // const auto subkey = key.substr(1, key.size() - 1);
  // arc = start_state->find_or_insert_arc(subkey, value);
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

void Fst::search(const string_view search_key, vector<shared_ptr<SearchResult>> &results) {
  cout << "searching key: " << search_key << endl;
  string_view subkey = search_key;
  _root_state->search_key(search_key, results);
}

SearchResult::SearchResult(const string key, const node_value_t value)
  : _key(key),
    _value(value)
{}
