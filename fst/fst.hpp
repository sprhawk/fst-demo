#include <memory>

namespace Fst {
  class FstNode {
  private:
    char node;
    int value;
  public:
    FstNode(char node, int value);
    FstNode(FstNode &fst);
    ~FstNode();

    char GetNode(){ return this->node; };
    int GetValue() { return this->value; };
  };

  class Fst {
  private:
    FstNode node;

  public:
    void AddNode(char node, int value);
  };
} // namespace Fst
