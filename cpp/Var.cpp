#include <iostream>
#include <ostream>

extern "C" {
#include "../c/Var.h"
}

#include "Derivatives.hpp"
#include "Var.hpp"

using namespace Laud;

Var::Var(const void *mval_) : laud_var_handle(LaudVarCopy(mval_)) {}
Var::Var(const float val) : laud_var_handle(LaudVarVar(val)) {}
Var::Var(const Var &lvar)
    : laud_var_handle(LaudVarCopy(lvar.laud_var_handle)) {}

Var::~Var() { LaudVarDelete((void *)laud_var_handle); }

float Var::setValue(float val) {
  return LaudVarSetFloatValue((void *)laud_var_handle, val);
}
bool Var::isFloat() const { return LaudVarIsFloat(laud_var_handle); }
float Var::getValue() const { return LaudVarGetFloatValue(laud_var_handle); }
const void *Var::getMval() const { return this->laud_var_handle; }
bool Var::isContinous() const {
  return LaudVarIsContinous(laud_var_handle) == 1;
}

Var &Var::evaluate() {
  LaudVarEvaluate((void *)laud_var_handle);
  return *this;
}
Derivatives Var::differentiate() {
  void **ddxs;
  return Derivatives(this, LaudVarDifferentiate(laud_var_handle, NULL, &ddxs),
                     ddxs);
}

std::ostream &Laud::operator<<(std::ostream &o, const Var &v) {
  if (v.isFloat())
    o << v.getValue();
  else
    o << "<Laud::Const " << v.getMval() << ">";
  return o;
}
Var Var::operator+(Var &v) {
  return Var(LaudVarAdd(this->laud_var_handle, v.laud_var_handle));
}
Var Var::operator-(Var &v) {
  return Var(LaudVarMinus(this->laud_var_handle, v.laud_var_handle));
}
Var Var::operator*(Var &v) {
  return Var(LaudVarProduct(this->laud_var_handle, v.laud_var_handle));
}
Var Var::operator/(Var &v) {
  return Var(LaudVarQoutient(this->laud_var_handle, v.laud_var_handle));
}
