#ifndef EXPR_H
#define EXPR_H

#include <string>

class Expression{
 public:
  virtual ~Expression() = default;
  virtual std::string to_string()=0;
};

#endif
