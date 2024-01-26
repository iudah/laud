#ifndef CONST_HPP
#define CONST_HPP

#include "Var.hpp"

class Laud::Const : public Laud::Var {
public:
  Const(const float value);
  Const(const float values[]);
  Const(Const &var);
  ~Const();
};

#endif