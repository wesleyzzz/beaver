//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "CoupledAdvection.h"


//*************Implementation based on Was book v2 Page 262 Eq. 6.29**************//

registerMooseObject("BeaverApp", CoupledAdvection);

template <>
InputParameters
validParams<CoupledAdvection>()
{
  InputParameters params = validParams<Kernel>();
  params.addRequiredCoupledVar("v", "Concentration of another variable");
  params.addRequiredParam<MaterialPropertyName>(
      "mat_coef", "Property name of the coefficient of the kernel");
  return params;
}

CoupledAdvection::CoupledAdvection(const InputParameters & parameters)
  : Kernel(parameters),
    _grad_var(coupledGradient("v")),
    _var(coupled("v")),
    _coef_coupled(getMaterialProperty<Real>("mat_coef"))//-(dAv-dBv)*kappa*Omega
{
}


Real
CoupledAdvection::computeQpResidual()
{
  return _coef_coupled[_qp] * _u[_qp] * _grad_var[_qp] * _grad_test[_i][_qp];
}

Real
CoupledAdvection::computeQpJacobian()
{
  return _coef_coupled[_qp] * _phi[_j][_qp] * _grad_var[_qp] * _grad_test[_i][_qp];
}

Real
CoupledAdvection::computeQpOffDiagJacobian(unsigned int jvar)
{
  return 0.0;
}
