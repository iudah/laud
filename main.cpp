#include "cpp/Const.hpp"
#include "cpp/Derivatives.hpp"
#include <iostream>
#include <vector>

extern "C" int laud();

int main() {
  std::vector<Laud::Var> variables = {Laud::Const(3), Laud::Const(9.02),
                                      Laud::Var(4.5), Laud::Var(8.445)};
  Laud::Var product = variables[0] * variables[1];
  Laud::Var product1 = variables[3] * variables[1];
  Laud::Var difference = product - variables[2];
  Laud::Var sum = difference + variables[3];
  std::cout << (Laud::Const(90.9) / variables[2]) << " !\n";

  std::cout << "variables[0] (differentiation): "
            << variables[0].differentiate() << '\n';

  std::cout << "variables[0] (differentiation): "
            << variables[0].differentiate() << '\n';

  std::cout << "variables[3] (differentiation): "
            << variables[3].differentiate() << '\n';

  std::cout << "product1 (differentiation): " << product1.differentiate()
            << '\n';
  std::cout << "sum (differentiation): " << sum.differentiate() << '\n';

  std::cout << "sum (differentiation): " << sum.differentiate() << '\n';

  return laud();
}
