#ifndef DERIVATIVES_HPP
#define DERIVATIVES_HPP

#include "Laud.hpp"
#include <map>
#include <ostream>

class Laud::Derivatives {
private:
  // contains both x and dx
  // void **x;
  Laud::Var *y;
  // std::vector<Laud::Var> derivatives;
  std::map<const void *, Laud::Var> derivs;

public:
  Derivatives(Laud::Var *y, int length, void *ddx);
  ~Derivatives();
  Laud::Var &find(Laud::Var &x);
  friend std::ostream &operator<<(std::ostream &o, const Derivatives &d);
};

#endif