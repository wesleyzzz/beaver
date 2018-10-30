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
  params.addCoupledVar("Cv","vacancy variables to couple");
  params.addCoupledVar("Ci","interstitial variables to couple");
  params.addCoupledVar("CA","interstitial variables to couple");
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
    _Omega(1.12e-11), //um^3 atomic volume of NiCr20 (VNi*0.8+VCr*0.2=6.59*0.8+7.23*0.2=cm^3/mol
    _lattice(3.53e-4),  //lattice param in um 
    //diffusivities
    _coef_dAv(declareProperty<Real>("dAvOmega")), //
    _coef_dBv(declareProperty<Real>("dBvOmega")), //
    _coef_dAi(declareProperty<Real>("dAiOmega")), //
    _coef_dBi(declareProperty<Real>("dBiOmega")), //
    _Kvi(declareProperty<Real>("vi_reaction_rate"))
{
}

void
RadiationNiCr::computeQpProperties()
{

//reference: https://arxiv.org/pdf/1607.04667.pdf
  Real nu = 1.0e13;//debye frequency /s
  Real k = 8.617e-5; //eV/K boltzmann constant
  Real dSAv = 0.0;//entropy
  Real dSBv = 0.0;//entropy
  Real EmAv = 0.7 ;//barrier
  Real EmBv = 1.2;//barrier
  Real dSAi = 0.0;//entropy
  Real dSBi = 0.0;//entropy
  Real EmAi = 0.26;//barrier
  Real EmBi = 0.11;//barrier

  Real lambda_v = 1.414/2 * _lattice; //jump distance
  Real z_v = 12; //number of nearest neighbours in fcc
  Real omega_Av = nu * std::exp(dSAv/k) * std::exp(-EmAv/k/_T);//jump frequency A <-> vacancy
  Real omega_Bv = nu * std::exp(dSBv/k) * std::exp(-EmBv/k/_T);;//jump frequency B <-> vacancy

  Real lambda_i = 1.0/2 * _lattice; //jump distance
  Real z_i = 12; //number of nearest neighbours in fcc
  Real omega_Ai = nu * std::exp(dSAi/k) * std::exp(-EmAi/k/_T);//jump frequency A <-> interstitial
  Real omega_Bi = nu * std::exp(dSBi/k) * std::exp(-EmBi/k/_T);//jump frequency B <-> interstitial

  Real dAv = 1.0/6*lambda_v*lambda_v*z_v*omega_Av;
  Real dBv = 1.0/6*lambda_v*lambda_v*z_v*omega_Bv;
  Real dAi = 1.0/6*lambda_i*lambda_i*z_i*omega_Ai;
  Real dBi = 1.0/6*lambda_i*lambda_i*z_i*omega_Bi;

  _coef_dAv[_qp] = dAv * _Omega;
  _coef_dBv[_qp] = dBv * _Omega;
  _coef_dAi[_qp] = dAi * _Omega;
  _coef_dBi[_qp] = dBi * _Omega;

  Real riv = _lattice;
  Real Dv = dAv * _CA[_qp]*_Omega + dBv * (1.0 - _CA[_qp]*_Omega);
  Real Di = dAi * _CA[_qp]*_Omega + dBi * (1.0 - _CA[_qp]*_Omega);
  _Kvi[_qp] = 4.0 * 3.1415926 * riv * (Di + Dv);
}
