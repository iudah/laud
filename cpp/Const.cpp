extern "C" {
    #include "../c/Const.h"
}
#include "Const.hpp"

using namespace Laud;

const size_t one = 1;
Const::Const(float value): Laud::Var(laud_const(0, NULL, one, &value)) {
    //const size_t one = 1;
    //laudConfigureEmpty((void *)getLaudHandle(), 0, &one, one, &value, 0);
}
Const::~Const() {}