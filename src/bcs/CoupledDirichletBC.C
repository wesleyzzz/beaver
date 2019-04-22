//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "CoupledDirichletBC.h"

registerMooseObject("BeaverApp", CoupledDirichletBC);

template <>
InputParameters
validParams<CoupledDirichletBC>()
{
  InputParameters params = validParams<DirichletBC>();
  params.addCoupledVar("v", "The coupled variables");
  params.addRequiredParam<Real>("value","value to be subtracted");
  return params;
}

CoupledDirichletBC::CoupledDirichletBC(const InputParameters & parameters)
  : DirichletBC(parameters),_c(getParam<Real>("value"))
{
  _v_var.resize(coupledComponents("v"));
  _v.resize(coupledComponents("v"));
  for (unsigned int j = 0; j < coupledComponents("v"); ++j)
  {
    _v_var[j] = coupled("v", j);
    _v[j] = &coupledValue("v", j);
  }
}

Real
CoupledDirichletBC::computeQpResidual()
{
  Real sum = 0.0;
  for (unsigned int j = 0; j < _v_var.size(); ++j)
    sum += (*_v[j])[_qp];

  return _c - sum;
  //return _c * _u[_qp] + _u[_qp] * _u[_qp] + _v[_qp] * _v[_qp] - _value;
}

Real
CoupledDirichletBC::computeQpJacobian()
{
  return 0.0; 
}

Real
CoupledDirichletBC::computeQpOffDiagJacobian(unsigned int jvar)
{
  
  for (unsigned int j = 0; j < _v_var.size(); ++j)
  {
    if (jvar == _v_var[j])
      return -1.0;
  }
  return 0.0;
}
