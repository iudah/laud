#include <iostream>
#include <ostream>

extern "C" {
#include "../c/Var.h"
}

#include "Derivatives.hpp"
#include "Var.hpp"

using namespace Laud;

Var::Var() {}
Var::Var(const void *laud_var_handle_)
    : laud_var_handle(laud_copy(laud_var_handle_)) {}

static const size_t one = 1;
Var::Var(const float value) : laud_var_handle(laud_var(0, NULL, one, &value)) {}

Var::Var(const Var &lvar) : laud_var_handle(laud_copy(lvar.laud_var_handle)) {}

Var::~Var() { laud_delete((void *)laud_var_handle); }

float Var::setValue(float val, size_t index) {
  return laud_set_value((void *)laud_var_handle, index, val);
}
bool Var::isFloat() const { return laud_is_float(laud_var_handle); }
float Var::getValue(size_t index) const {
  return laud_get_value(laud_var_handle, index);
}
const void *Var::getLaudHandle() const { return this->laud_var_handle; }
bool Var::isContinous() const {
  return laud_is_continuous(laud_var_handle) == 1;
}

Var &Var::evaluate() {
  laud_evaluate((void *)laud_var_handle);
  return *this;
}
Derivatives Var::differentiate() {
  void *map_of_x_to_derivatives_of_this_wrt_each_x =
      laud_create_derivative_map();

  laud_differentiate(laud_var_handle, NULL,
                     map_of_x_to_derivatives_of_this_wrt_each_x);

  Derivatives derivatives =
      Derivatives(map_of_x_to_derivatives_of_this_wrt_each_x);

  laud_delete_derivative_map(map_of_x_to_derivatives_of_this_wrt_each_x);
  return derivatives;
}

std::ostream &Laud::operator<<(std::ostream &o, const Var &v) {
  if (v.isFloat())
    o << v.getValue(0);
  else
    o << "<Laud::Const " << v.getLaudHandle() << ">";
  return o;
}
Var Var::operator+(Var &v) {
  return Var(laud_add(this->laud_var_handle, v.laud_var_handle));
}
Var Var::operator-(Var &v) {
  return Var(laud_minus(this->laud_var_handle, v.laud_var_handle));
}
Var Var::operator*(Var &v) {
  return Var(laud_product(this->laud_var_handle, v.laud_var_handle));
}
Var Var::operator/(Var &v) {
  return Var(laud_qoutient(this->laud_var_handle, v.laud_var_handle));
}
