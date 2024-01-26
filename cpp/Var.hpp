#ifndef VAR_HPP
#define VAR_HPP
#include <cstddef>
#include <iostream>

#include "Derivatives.hpp"

namespace Laud {

class Var {
private:
  /**
   * @brief Handle to LaudVar created in laud-c library
   */
  const void *laud_var_handle;

protected:
  Var(const void *mval);

public:
  Var();
  Var(const float value);
  Var(const float values[]);
  Var(const Var &var);
  ~Var();

  /**
   * @brief Check whether object is a float point value
   *
   * @return true
   * @return false
   */
  bool isFloat() const;

  /**
   * @brief Set the Value object
   *
   * @param value value to set to
   * @return value
   */
  float setValue(float value, size_t index);

  /**
   * @brief Get the Value object
   *
   * @return float value of object
   */
  float getValue(size_t index) const;

  /**
   * @brief Get the internal handle of object laud-c-object
   *
   * @return laud-c-object
   */
  const void *getLaudHandle() const;

  /**
   * @brief Check if object is a differentiable
   *
   * @return true
   * @return false
   */
  bool isContinous() const;

  /**
   * @brief Evaluate object graph
   *
   * @return Var& this
   */
  Var &evaluate();

  /**
   * @brief Differentiate the object graph
   *
   * @return Laud::Derivatives
   */
  Laud::Derivatives differentiate();
  /**
   * @brief Construct a new Laud:: Derivatives:: Derivatives object
   *
   * @param y reference to differentiated object
   * @param length number of object in ddx, it is 2 times the number of
   * differentiated independent variables
   * @param ddx LaudVarDerivativeMap of [independent x, derivatives wrt x] pair
   */
  friend Laud::Derivatives::Derivatives(void *ddx);
  /**
   * @brief Override stream insertion object
   *
   * @param o stream object
   * @param v object to display
   * @return std::ostream& o
   */
  friend std::ostream &operator<<(std::ostream &o, const Var &v);
  // Overloaded arithmetic operators for class Var.
  Var operator+(Var &v);
  Var operator-(Var &v);
  Var operator*(Var &v);
  Var operator/(Var &v);
};
}; // namespace Laud
#endif