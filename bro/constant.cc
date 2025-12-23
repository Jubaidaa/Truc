#include "constant.h"

Constant::Constant(int value_) : value{ value_ } {}

std::string Constant::to_string(){
  return std::to_string(value);
}


Constant::~Constant(){}
