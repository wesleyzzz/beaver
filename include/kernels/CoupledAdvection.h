//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef COUPLEDADVECTION_H
#define COUPLEDADVECTION_H

#include "Kernel.h"

class CoupledAdvection;

template <>
InputParameters validParams<CoupledAdvection>();


class CoupledAdvection : public Kernel
{
public:
  CoupledAdvection(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;


  const VariableGradient & _grad_var;
  const unsigned int _var;

  const MaterialProperty<Real> & _coef_coupled;
};

#endif // COUPLEDADVECTION_H
