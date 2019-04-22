//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "DefectProductionRate.h"

// MOOSE
#include "Function.h"

registerMooseObject("BeaverApp", DefectProductionRate);

template <>
InputParameters
validParams<DefectProductionRate>()
{
  InputParameters params = validParams<Kernel>();
  params.addClassDescription("Direct production rate of point defects");
  params.addParam<Real>("value", 1.0, "Coefficent to multiply by the body force term");
  return params;
}

DefectProductionRate::DefectProductionRate(const InputParameters & parameters)
  : DerivativeMaterialInterface<Kernel>(parameters),
    _scale(getParam<Real>("value")),
    _rate(getMaterialProperty<Real>("point_defect_rate"))
{
}

Real
DefectProductionRate::computeQpResidual()
{
  Real factor = -_scale * _rate[_qp];
  return _test[_i][_qp] * factor;
}
