//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "CoupledDiffusion.h"
#include "MooseMesh.h"

registerMooseObject("BeaverApp", CoupledDiffusion);

template <>
InputParameters
validParams<CoupledDiffusion>()
{
  InputParameters params = validParams<Diffusion>();
  params.addRequiredParam<MaterialPropertyName>(
      "mat_coef", "Property name of the coefficient of the kernel");
  params.addParam<MaterialPropertyName>(
      "dmat_coef_dvar","mat_derivative", "Property name of the derivative of the coefficient of the kernel");
  return params;
}

CoupledDiffusion::CoupledDiffusion(const InputParameters & parameters)
  : Diffusion(parameters),
    _D(getMaterialProperty<Real>("mat_coef")),
    _dD_dvar(hasMaterialProperty<Real>("dmat_coef_dvar")?&getMaterialProperty<Real>("dmat_coef_dvar"):NULL)
{
}


Real
CoupledDiffusion::computeQpResidual()
{
  return _D[_qp] * Diffusion::computeQpResidual();
}

Real
CoupledDiffusion::computeQpJacobian()
{
  Real jac = _D[_qp] * Diffusion::computeQpJacobian();
  if (_dD_dvar) 
    jac += (*_dD_dvar)[_qp] * _phi[_j][_qp] * Diffusion::computeQpResidual();
  return  jac; 
}

Real
CoupledDiffusion::computeQpOffDiagJacobian(unsigned int jvar)
{
  return 0.0;//Need input 
}
