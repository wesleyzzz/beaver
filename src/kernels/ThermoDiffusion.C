//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ThermoDiffusion.h"

// MOOSE includes
#include "MooseVariable.h"

registerMooseObject("BeaverApp", ThermoDiffusion);

template <>
InputParameters
validParams<ThermoDiffusion>()
{
  InputParameters params = validParams<Kernel>();
  params.addClassDescription("Add Thermo effect to Split formulation Cahn-Hilliard Kernel");
  params.addRequiredCoupledVar("T", "Temperature");
  params.addRequiredCoupledVar("c", "Concentration");
  params.addRequiredParam<MaterialPropertyName>("MQ_name",
                                                "The thermotransport mobility used with the kernel");
  return params;
}

ThermoDiffusion::ThermoDiffusion(const InputParameters & parameters)
  : DerivativeMaterialInterface<Kernel>(parameters),
    _T_var(coupled("T")),
    _T(coupledValue("T")),
    _grad_T(coupledGradient("T")),
    _c_var(coupled("c")),
    _c(coupledValue("c")),
    _MQ(getMaterialProperty<Real>("MQ_name")),
    _dMQdc(getMaterialPropertyDerivative<Real>("MQ_name",getVar("c",0)->name()))
{
}

Real
ThermoDiffusion::computeQpResidual()
{
  return - _MQ[_qp] / _T[_qp]  * _grad_T[_qp] * _grad_test[_i][_qp];
}

Real
ThermoDiffusion::computeQpJacobian()
{
  return 0.0;
}

Real
ThermoDiffusion::computeQpOffDiagJacobian(unsigned int jvar)
{
  // c Off-Diagonal Jacobian
  if (_c_var == jvar)
    return computeQpCJacobian();

  // T Off-Diagonal Jacobian
  if (_T_var == jvar)
    return -_MQ[_qp] *  _grad_test[_i][_qp] * 
           (_grad_phi[_j][_qp] / _T[_qp] - _grad_T[_qp] * _phi[_j][_qp] / _T[_qp] / _T[_qp]);

  return 0.0;
}

Real
ThermoDiffusion::computeQpCJacobian()
{
  // Calculate the Jacobian for the c variable
  return - _dMQdc[_qp] * _phi[_j][_qp]  / _T[_qp]  * _grad_T[_qp] * _grad_test[_i][_qp];
}
