#ifndef LAUD_HPP
#define LAUD_HPP

#include <iostream>

namespace Laud {
class Var;
class Const;
class Derivatives;
std::ostream &operator<<(std::ostream &o, const Var &v);
std::ostream &operator<<(std::ostream &o, const Derivatives &d);
}; // namespace Laud

#endif