#include <iostream>

extern "C" {
#include "../c/Var.h"
}

#include "Var.hpp"

using namespace Laud;
#include "Derivatives.hpp"

Derivatives::Derivatives(Var *y, int length, void *ddx)
    : derivs(std::map<const void *, Var>()) {

  LaudVarDerivativeMapIterStart(ddx);
  void **X;
  while ((X = LaudVarDerivativeMapNext(ddx))) {
    derivs[(const void *)X[0]] = Var(X[1]);
  }
  LaudVarDeleteDerivativeMap(ddx);
}
Derivatives::~Derivatives() { derivs.clear(); }
std::ostream &Laud::operator<<(std::ostream &o, const Derivatives &d) {
  for (const auto &[key, value] : d.derivs)
    o << '[' << LaudVarGetFloatValue(key) << "] = " << value << "; ";
  return o;
}

Laud::Var &Derivatives::find(Laud::Var &x) { return derivs[x.getMval()]; }