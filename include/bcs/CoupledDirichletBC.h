//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef COUPLEDDIRICHLETBC_H
#define COUPLEDDIRICHLETBC_H

#include "DirichletBC.h"

class CoupledDirichletBC;

template <>
InputParameters validParams<CoupledDirichletBC>();

/**
 * Implements the Dirichlet boundary condition
 * c*u + u^2 + v^2 = _value
 * where "u" is the current variable, and "v" is a coupled variable.
 * Note: without the constant term, a zero initial guess gives you a
 * zero row in the Jacobian, which is a bad thing.
 */
class CoupledDirichletBC : public DirichletBC
{
public:
  CoupledDirichletBC(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  // The coupled variable
  std::vector<const VariableValue *> _v;
  /// The id of the coupled variable
  std::vector<unsigned int> _v_var;

  // The constant
  Real _c;
};

#endif /* COUPLEDDIRICHLETBC_H */
