//
#include "fst.hpp"
using namespace Fst;

FstNode::FstNode(char node, int value):
  node(node),
  value(value){
  
}

FstNode::~FstNode() {}
