#include <iostream>

extern "C" {
#include "../c/Var.h"
}

#include "Var.hpp"

using namespace Laud;
#include "Derivatives.hpp"

Derivatives::Derivatives(void *ddx) : derivs(std::map<const void *, Var>()) {

  laud_start_derivative_map_iteration(ddx);
  void **X;
  while ((X = laud_yield_derivative_map_value(ddx))) {
    derivs.insert({X[0], Var(X[1])});
    // derivs[(const void *)X[0]] = Var(X[1]);
  }
}
Derivatives::~Derivatives() { derivs.clear(); }

std::ostream &Laud::operator<<(std::ostream &o, const Derivatives &d) {
  printf("%i\n", d.derivs.size());
  for (const auto &[key, value] : d.derivs)
    o << '[' << laud_get_value(key, 0) << "] = " << value << "; ";
  return o;
}

Laud::Var &Derivatives::find(Laud::Var &x) { return derivs[x.getLaudHandle()]; }