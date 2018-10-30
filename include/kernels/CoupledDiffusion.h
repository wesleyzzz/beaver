//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef COUPLEDDIFFUSION_H
#define COUPLEDDIFFUSION_H

#include "Kernel.h"
#include "Diffusion.h"

class CoupledDiffusion;

template <>
InputParameters validParams<CoupledDiffusion>();


class CoupledDiffusion : public Diffusion
{
public:
  CoupledDiffusion(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  const MaterialProperty<Real> & _D;
  const MaterialProperty<Real> * const _dD_dvar;
  const Real _coef;
};

#endif // COUPLEDDIFFUSION_H
