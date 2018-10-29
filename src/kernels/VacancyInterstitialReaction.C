//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "VacancyInterstitialReaction.h"

registerMooseObject("BeaverApp", VacancyInterstitialReaction);

template <>
InputParameters
validParams<VacancyInterstitialReaction>()
{
  InputParameters params = validParams<Kernel>();
  params.addCoupledVar("v", "The other variable to multiple with");
  params.addRequiredParam<MaterialPropertyName>(
      "mat_coef", "Property name of the coefficient of the kernel");
  return params;
}

VacancyInterstitialReaction::VacancyInterstitialReaction(const InputParameters & parameters)
  : Kernel(parameters),
    _C(coupledValue("v")),
    _C_var(coupled("v")),
    _Kvi(getMaterialProperty<Real>("mat_coef"))
{
}


Real
VacancyInterstitialReaction::computeQpResidual()
{
  return _Kvi[_qp] * _C[_qp] * _u[_qp] * _test[_i][_qp];
}

Real
VacancyInterstitialReaction::computeQpJacobian()
{
  return _Kvi[_qp] * _C[_qp] * _phi[_j][_qp] * _test[_i][_qp];
}

Real
VacancyInterstitialReaction::computeQpOffDiagJacobian(unsigned int jvar)
{
  return 0.0;
}
