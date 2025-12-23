#include "pair.h"

Pair::Pair(Expression* a, Expression* b) : fst(a), snd(b) {}

std::string Pair::to_string(){
  return "("+fst->to_string() + ","+snd->to_string()+")";
}


Pair::~Pair()
{
  delete fst;
  delete snd;
}
