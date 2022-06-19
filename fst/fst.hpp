#include <memory>
#include <string>
#include <vector>

namespace FstN {
  typedef char node_key_t;
  typedef int node_value_t;
  
  using namespace std;

  class State;
  class Fst;
 
  class Arc {
  private:
    node_key_t _nodekey;
    node_value_t _value;

    shared_ptr<State> _end_state;
  public:
    Arc(node_key_t nodekey, node_value_t value);
    Arc(Arc &arc);
    ~Arc();

  public:
    char get_node_key(){ return this->_nodekey; };
    int get_value() { return this->_value; };
    shared_ptr<State> find_or_insert_state(const std::string_view key, const node_value_t value, Fst &fst);
    void set_state(const shared_ptr<State> state) { _end_state = state; };
    shared_ptr<State> get_state() { return _end_state; };
  };

   typedef uint64_t state_id_t;
  class State {
    friend Fst;
    friend Arc;
  private:
    state_id_t _id;
    node_key_t _key;
    bool _is_final;
  private:
  protected:
    vector<shared_ptr<Arc>> _arcs; 
    shared_ptr<Arc> find_or_insert_arc(const std::string_view key, const node_value_t value, const Fst &fst);
  public:
    State(state_id_t id);
    State(state_id_t id, node_key_t key);
    State(state_id_t id, bool is_final);
    bool is_final() { return _is_final; }
    state_id_t get_id(){return _id;};
    node_key_t get_key() { return _key; };
  };

  class Fst {
    friend Arc;
  private:
    shared_ptr<State> _root_state;
    shared_ptr<State> _final_state;
    state_id_t _next_id;
    vector<shared_ptr<State>> _intermediate_states;
  private:
    void inc_next_id() { _next_id++; };

  protected:
    // find a state represents from key[0] to key[1],
    // if found, a start_state and an end_state are returned, 
    // if not found, create and insert one into _intermediate_states
    // 
    shared_ptr<State> find_or_insert_shared_state(const string_view key, const node_value_t value, shared_ptr<Arc> &arc);
  public:
    Fst();
    ~Fst();
    void insert(const std::string_view key, const node_value_t value);
    shared_ptr<State> get_final_state() { return _final_state; };
    state_id_t get_next_id();
    state_id_t get_and_inc_next_id();
  };
} // namespace Fst
