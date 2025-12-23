#ifndef POLKA_H
#define POLKA_H

#include <stack>
#include <string>
#include "expression.h"
#include "pair.h"

using namespace std;

int cast_to_int(Expression* e);
Pair* cast_to_pair(Expression* e);

class Polka {
  stack<Expression*> s;
public:
  Polka();
  ~Polka();
  int size();
  void push(int x);
  void push(Expression* a, Expression* b);
  void push(Expression* e);
  void push(std::string token);
  string to_string();
  Expression* get_value();
};

#endif
