//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "RadiationNiCr.h"

#include "Function.h"

registerMooseObject("BeaverApp", RadiationNiCr);

template <>
InputParameters
validParams<RadiationNiCr>()
{
  InputParameters params = validParams<Material>();
  params.addCoupledVar("Cv","vacancy variables to couple");
  params.addCoupledVar("Ci","interstitial variables to couple");
  params.addCoupledVar("CA","solute to couple");
  params.addCoupledVar("CB","solute to couple");
  params.addRequiredParam<Real>("temperature","System temperature (K)");
  params.addParam<Real>("dose_rate",1.0,"Dose rate (dpa/s)");
  params.addParam<FunctionName>("dose_rate_function", "1", "A function that describes dose rate");
  params.addParam<bool>("debug_mode",false,"Dose rate (dpa/s)");
  params.addClassDescription("This Material calculates necessary coefficients and derivatives");
  return params;
}

RadiationNiCr::RadiationNiCr(const InputParameters & parameters)
  :  DerivativeMaterialInterface<Material>(parameters),
    _Cv(coupledValue("Cv")),
    _Ci(coupledValue("Ci")),
    _CA(coupledValue("CA")),
    _CB(isCoupled("CB")? &coupledValue("CB"): NULL),
    //constants
    _T(getParam<Real>("temperature")),
    _dose_rate(getParam<Real>("dose_rate")),
    _dose_rate_function(getFunction("dose_rate_function")),
    _debug(getParam<bool>("debug_mode")),
    _Omega(1.12e-11), //um^3 atomic volume of NiCr20 (VNi*0.8+VCr*0.2=6.59*0.8+7.23*0.2=cm^3/mol
    _lattice(3.53e-4),  //lattice param in um 
    //diffusivities
    _coef_dAv(declareProperty<Real>("dAvOmega")), //
    _coef_dBv(declareProperty<Real>("dBvOmega")), //
    _coef_dAi(declareProperty<Real>("dAiOmega")), //
    _coef_dBi(declareProperty<Real>("dBiOmega")), //
    _Dv(declareProperty<Real>("VacancyDiffusivity")),
    _Di(declareProperty<Real>("InterstitialDiffusivity")),
    _DA(declareProperty<Real>("ElemADiffusivity")),
    _DB(declareProperty<Real>("ElemBDiffusivity")),
    _Kvi(declareProperty<Real>("vi_reaction_rate")),
    _K0(declareProperty<Real>("point_defect_rate"))
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
  Real EmAv = 0.5 ;//barrier
  Real EmBv = 0.6;//barrier
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
  if (!_CB)
  { 
  //constraint CB = 1-CA
    _Dv[_qp] = dAv * _CA[_qp]*_Omega + dBv * (1.0 - _CA[_qp]*_Omega);
    _Di[_qp] = dAi * _CA[_qp]*_Omega + dBi * (1.0 - _CA[_qp]*_Omega);
  }
  else
  {
    _Dv[_qp] = dAv * _CA[_qp]*_Omega + dBv * (*_CB)[_qp]*_Omega;
    _Di[_qp] = dAi * _CA[_qp]*_Omega + dBi * (*_CB)[_qp]*_Omega;
  }
  _DA[_qp] = _coef_dAv[_qp] * _Cv[_qp] + _coef_dAi[_qp] * _Ci[_qp];
  _DB[_qp] = _coef_dBv[_qp] * _Cv[_qp] + _coef_dBi[_qp] * _Ci[_qp];
  _Kvi[_qp] = 4.0 * 3.1415926 * riv * (_Di[_qp] + _Dv[_qp]);

  if (_debug)
  {
    std::cout << "Interstitial jump frequency via A: " << omega_Ai << std::endl;
    std::cout << "Interstitial jump frequency via B: " << omega_Bi << std::endl;
    std::cout << "Interstitial diffusivity via A: " << dAi << std::endl;
    std::cout << "Interstitial diffusivity via B: " << dBi << std::endl;
    std::cout << "Intersitial diffusivity: " << _Di[_qp] << std::endl;
    std::cout << "Vacancy jump frequency via A: " << omega_Av << std::endl;
    std::cout << "Vacancy jump frequency via B: " << omega_Bv << std::endl;
    std::cout << "Vacancy diffusivity via A: " << dAv << std::endl;
    std::cout << "Vacancy diffusivity via B: " << dBv << std::endl;
    std::cout << "Vacancy diffusivity: " << _Dv[_qp] << std::endl;
    std::cout << "A concentration: " << _CA[_qp] << std::endl;
    std::cout << "A fraction: " << _CA[_qp]*_Omega << std::endl;
  }

  _K0[_qp] = _dose_rate*_dose_rate_function.value(_t,_q_point[_qp])/_Omega;
}
