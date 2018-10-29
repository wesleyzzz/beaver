//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "RadiationNiCr.h"

registerMooseObject("BeaverApp", RadiationNiCr);

template <>
InputParameters
validParams<RadiationNiCr>()
{
  InputParameters params = validParams<Material>();
  params.addRequiredCoupledVar("Cv","vacancy variables to couple");
  params.addRequiredCoupledVar("Ci","interstitial variables to couple");
  params.addRequiredCoupledVar("CA","interstitial variables to couple");
  params.addRequiredParam<Real>("temperature","System temperature (K)");
  params.addClassDescription("This Material calculates necessary coefficients and derivatives");
  return params;
}

RadiationNiCr::RadiationNiCr(const InputParameters & parameters)
  :  DerivativeMaterialInterface<Material>(parameters),
    _Cv(coupledValue("Cv")),
    _Ci(coupledValue("Ci")),
    _CA(coupledValue("CA")),
    //constants
    _T(getParam<Real>("temperature")),
    _Omega(000), // atomic volume
    _kappa(000),  //thermodynamic factor 
    //diffusivities
    _Dv(declareProperty<Real>("vacancy_diffusivity")),
    _Di(declareProperty<Real>("interstitial_diffusivity")),
    _DA(declareProperty<Real>("solute_diffusivity")),
    _DAkappa(declareProperty<Real>("mod_solute_diffusivity")), //DA*kappa
    //number denotes order of derivative
    _coef_v0A1(declareProperty<Real>("coef_v0A1")), //-(dAv-dBv)*kappa*Omega
    _coef_i0A1(declareProperty<Real>("coef_i0A1")), //(dAi-dBi)*kappa*Omega
    _coef_v1A0(declareProperty<Real>("coef_v1A0")), //-dAv*Omega
    _coef_i1A0(declareProperty<Real>("coef_i1A0")), //dAi*Omega
    _Kvi(declareProperty<Real>("vi_reaction_rate"))
{
}

void
RadiationNiCr::computeQpProperties()
{
  _Dv[_qp] = 000; 
  _Di[_qp] = 000; 
  _DA[_qp] = 000; 
  _DAkappa[_qp] = _DA[_qp] * _kappa; 

  Real dAv = 000;
  Real dBv = 000;
  Real dAi = 000;
  Real dBi = 000;
  _coef_v0A1[_qp] = -(dAv-dBv) * _kappa * _Omega;
  _coef_i0A1[_qp] = (dAi-dBi) * _kappa * _Omega;
  _coef_v1A0[_qp] = dAv * _Omega;
  _coef_i1A0[_qp] = dAi * _Omega;
  _Kvi[_qp] = 000;
}
