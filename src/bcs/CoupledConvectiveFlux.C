//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "CoupledConvectiveFlux.h"

#include "Function.h"

registerMooseObject("BeaverApp", CoupledConvectiveFlux);

template <>
InputParameters
validParams<CoupledConvectiveFlux>()
{
  InputParameters params = validParams<IntegratedBC>();
  params.addRequiredCoupledVar("C_infinity", "Field holding far-field concentration");
  params.addRequiredParam<Real>("coefficient", "Heat transfer coefficient");

  return params;
}

CoupledConvectiveFlux::CoupledConvectiveFlux(const InputParameters & parameters)
  : IntegratedBC(parameters),
    _C_infinity(coupledValue("C_infinity")),
    _coefficient(getParam<Real>("coefficient"))
{
}

Real
CoupledConvectiveFlux::computeQpResidual()
{
  return _test[_i][_qp] * _coefficient * (_u[_qp] - _C_infinity[_qp]);
}

Real
CoupledConvectiveFlux::computeQpJacobian()
{
  return _test[_i][_qp] * _coefficient * _phi[_j][_qp];
}
