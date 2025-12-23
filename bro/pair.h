#ifndef PAIR_H
#define PAIR_H

#include "expression.h"

class Pair : public Expression {
 public:
  Expression* fst;
  Expression* snd;

  Pair(Expression* a, Expression* b);
  ~Pair();

  std::string to_string();
};
#endif
