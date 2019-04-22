//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html




////// reference: Thermotransport in γ(bcc) U–Zr alloys: A phase-field model study
#ifndef THERMODIFFUSION_H
#define THERMODIFFUSION_H

#include "Kernel.h"
#include "DerivativeMaterialInterface.h"

// Forward Declaration
class ThermoDiffusion;

template <>
InputParameters validParams<ThermoDiffusion>();
/**
 * ThermoDiffusion adds the soret effect in the split form of the Cahn-Hilliard
 * equation.
 */
class ThermoDiffusion : public DerivativeMaterialInterface<Kernel>
{
public:
  ThermoDiffusion(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);
  virtual Real computeQpCJacobian();

  /// int label for temperature variable
  unsigned int _T_var;

  /// Coupled variable for the temperature
  const VariableValue & _T;

  /// Variable gradient for temperature
  const VariableGradient & _grad_T;

  /// int label for the Concentration
  unsigned int _c_var;

  /// Variable value for the concentration
  const VariableValue & _c;

  /// thermotransport mobility 
  const MaterialProperty<Real> & _MQ;
  const MaterialProperty<Real> & _dMQdc;

};

#endif // THERMODIFFUSION_H
