extern "C" {
#include "../c/Const.h"
}
#include "Const.hpp"

using namespace Laud;
Const::Const(float value) : Laud::Var(LaudConst_(value)) {}
Const::~Const() {}