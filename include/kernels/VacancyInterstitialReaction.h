//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef VACANCYINTERSTITIALREACTION_H
#define VACANCYINTERSTITIALREACTION_H

#include "Kernel.h"

class VacancyInterstitialReaction;

template <>
InputParameters validParams<VacancyInterstitialReaction>();


class VacancyInterstitialReaction : public Kernel
{
public:
  VacancyInterstitialReaction(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  const VariableValue & _C;
  const unsigned int _C_var;

  const MaterialProperty<Real> & _Kvi;

};

#endif // VACANCYINTERSTITIALREACTION_H
