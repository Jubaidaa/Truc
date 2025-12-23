#ifndef CST_H
#define CST_H

#include "expression.h"

class Constant : public Expression {
public:
  int value;
  Constant(int value);
  ~Constant();
  std::string to_string();
};
#endif
