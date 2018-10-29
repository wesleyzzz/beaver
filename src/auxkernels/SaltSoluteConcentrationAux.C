//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SaltSoluteConcentrationAux.h"

registerMooseObject("BeaverApp", SaltSoluteConcentrationAux);

template <>
InputParameters
validParams<SaltSoluteConcentrationAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addCoupledVar("v", 0, "variable for solute concentration");
  params.addParam<Real>("solute_init", 0, "Initial concentration of solute in salt");
  params.addClassDescription("AuxKernel to calculate solute concentration in the salt");
  return params;
}

SaltSoluteConcentrationAux::SaltSoluteConcentrationAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _C(coupledValue("v")),
    _C_init(getParam<Real>("solute_init"))
{
}

Real
SaltSoluteConcentrationAux::computeValue()
{
  return _C_init;
}
