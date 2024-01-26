#include "cpp/Const.hpp"
#include "cpp/Derivatives.hpp"
#include <cstdio>
#include <iostream>
#include <vector>

extern "C"
#ifdef _Win32
    __declspec(dllimport)
#endif
        int laud();

int main()
{
  printf("\n\n==============================================================\n"
         "%s\n"
         "==============================================================\n",
         __FILE__);

  std::vector<Laud::Var> variables = {Laud::Const(3), Laud::Const(9.02),
                                      Laud::Var(4.5), Laud::Var(8.445)};
  Laud::Var product1 = variables[3] * variables[1];

  Laud::Var product = variables[0] * variables[1];
  Laud::Var difference = product - variables[2];
  Laud::Var sum = difference + variables[3];
  std::cout << (Laud::Const(90.9) / variables[2]).evaluate() << "=90.9/4.5!\n";

  std::cout << "variables[0] (differentiation): "
            << variables[0].differentiate() << '\n';

  std::cout << "variables[1] (differentiation): "
            << variables[1].differentiate() << '\n';

  std::cout << "variables[3] (differentiation): "
            << variables[3].differentiate() << '\n';

  std::cout << "product1 (differentiation): "
            << product1.evaluate().differentiate() << '\n';

  std::cout << "difference (differentiation): " << product << " - "
            << variables[2] << " : " << difference.evaluate().differentiate()
            << '\n';

  std::cout << "sum (differentiation): " << difference << " - " << variables[3]
            << " : " << sum.evaluate().differentiate() << '\n';

  return laud();
}
