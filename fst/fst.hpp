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
    void insert_state(std::string_view key, node_value_t value, Fst &fst);
    void set_state(shared_ptr<State> state) { _end_state = state; };
    shared_ptr<State> get_state() { return _end_state; };
  };

   typedef uint64_t state_id_t;
  class State {
    friend Fst;
    friend Arc;
  private:
    state_id_t _id;
    bool _is_final;
  private:
  protected:
    vector<shared_ptr<Arc>> _arcs;
    void insert_arc(std::string_view key, node_value_t value, Fst &fst);

  public:
    State(state_id_t id);
    State(state_id_t id, bool is_final);
    bool is_final() { return _is_final; }
    state_id_t get_id(){return _id;};
  };

  class Fst {
  private:
    unique_ptr<State> _root_state;
    shared_ptr<State> _final_state;
    state_id_t _next_id;

  private:
    void inc_next_id() { _next_id++; };

  public:
    Fst();
    ~Fst();
    void insert(std::string_view key, int value);
    shared_ptr<State> get_final_state() { return _final_state; };
    state_id_t get_next_id();
    state_id_t get_and_inc_next_id();
  };
} // namespace Fst
