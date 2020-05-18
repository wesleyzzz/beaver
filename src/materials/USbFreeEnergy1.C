//* Track Sb concentration

#include "USbFreeEnergy1.h"

registerMooseObject("BeaverApp", USbFreeEnergy1);

template <>
InputParameters
validParams<USbFreeEnergy1>()
{
  InputParameters params = validParams<DerivativeParsedMaterialHelper>();
  MooseEnum stoi("U4Sb3 USb U3Sb4 USb2 Liquid U");
  params.addRequiredParam<MooseEnum>("stoi", stoi, "stoichiometric");
  params.addRequiredCoupledVar("c", "Concentration variable of Sb");
  params.addRequiredParam<Real>("temperature", "Temperature");
  params.addParam<Real>("k",8.0e7,"J/mol, stoichiometric stabilizer");
  params.addParam<Real>("unit_conversion",
                         0.00029837208,
                        "Conversion from J/mol to eV/nm^3");
  params.addParam<Real>("h",
                        4.135667662e-15,
                        "Planck constant - units need to be consistent with "
                        "the units of omega (default in [eV*s])"); //use this??
  params.addParam<Real>("kB", 8.6173303e-5, "Boltzmann constant (default in [eV/K])");//use this??
  return params;
}

USbFreeEnergy1::USbFreeEnergy1(const InputParameters & parameters)
  : DerivativeParsedMaterialHelper(parameters),
    _c("c"),//Sb concentration
    _stoi(getParam<MooseEnum>("stoi")),
    _T(getParam<Real>("temperature")),
    _k(getParam<Real>("k")),
    _m(getParam<Real>("unit_conversion"))
{

  
  Real R = 8.3144598;// gas constant  J/mol/K


  if (_T<298.15)
    mooseError("Temperature should be higher than 298.15!");

  Real G_U = 0.0; //free energy in standard element reference state (SGTE) in Orthorhombic_A2O phase
  if (_T<955.00 && _T>298.15)
    G_U = -8407.734 + 130.955151*_T - 26.9182* _T*log(_T) + 1.25156e-3*pow(_T,2) - 4.426050e-6*pow(_T,3) + 38568/_T;
  else if (_T<3000)
    G_U = -22521.8 + 292.121093*_T - 48.66* _T*log(_T);


  Real G_Sb = 0.0; //free energy in standard element reference state (SGTE) in Rhombo phase
  if (_T<903.78 && _T>298.15)
    G_Sb = -9242.858 + 156.154689*_T - 30.5130752*_T*log(_T) + 7.748768e-3*pow(_T,2)- 3.003415e-6*pow(_T,3) + 100625/_T;
  else if (_T<2000)
    G_Sb = -11738.83 + 169.485872*_T - 31.38*_T*log(_T) + 1.6168e27*pow(_T,-9);


  // Definition of the free energy for the expression builder
  EBFunction free_energy;

  if (_stoi == "U4Sb3")
  {
    Real a = -5.0273e5;
    Real b = 2.28e1;
    free_energy(_c) = 1.0/7* _m * (4.0*G_U + 3.0*G_Sb + a + b * _T + _k * pow(_c - 3.0/7,2)); // last term to penalize any deviation from the intermetallic composition U4Sb3
    std::cout << "Free_energy of " << _stoi << " is " << 1.0/7* (4.0*G_U + 3.0*G_Sb + a + b * _T ) << std::endl; //print the free_energy in console, the unit is J/mol
  }

  else if (_stoi == "U3Sb4")
  {
    Real a = -4.832e5;
    Real b = 0.6;
    free_energy(_c) = 1.0/7* _m * (3.0*G_U + 4.0*G_Sb + a + b * _T + _k * pow(_c - 4.0/7,2));
	std::cout << "Free_energy of " << _stoi << " is " << 1.0/7* (3.0*G_U + 4.0*G_Sb + a + b * _T ) << std::endl;
  }

  else if (_stoi == "USb")
  {
    Real a = -1.4e5;
    Real b = 0;
    free_energy(_c) = 1.0/2* _m * (G_U + G_Sb + a + b * _T + _k * pow(_c - 0.5,2));
    std::cout << "Free_energy of" << _stoi << " is " << 1.0/2* (G_U + G_Sb + a + b * _T ) << std::endl;
  }

  else if (_stoi == "USb2")
  {
    Real a = -1.803e5;
    Real b = 2.01;
    free_energy(_c) = 1.0/3* _m * (G_U + 2.0*G_Sb + a + b * _T + _k * pow(_c - 2.0/3,2));
    std::cout << "Free_energy of " << _stoi << " is " << 1.0/3* (G_U + 2.0*G_Sb + a + b * _T ) << std::endl; 
  }

  else if (_stoi == "Liquid")
  {
    Real G1 = 0.0;//liquid U gibbs energy, from SGTE database
    if (_T<955.00 && _T>298.15)
      G1 = 3947.766+120.631251*_T-26.9182*_T*log(_T)-1.25156e-3*_T*_T-4.42605e-6*pow(_T,3)+38568/_T ;
    else if (_T<3000)
      G1 = -10166.3 + 281.797193*_T - 48.66* _T*log(_T);

    Real G2 = 0.0;//Sb molar gibbs energy in liquid
    if (_T<903.78 && _T>298.15)
      G2 = 10579.47 + 134.231525 *_T - 30.5130752*_T*log(_T) + 7.748768e-3*pow(_T,2) - 3.003415e-6*pow(_T,3) + 100625/_T-1.7485e-20*pow(_T,7);
    else if (_T<2000)
      G2 = 8175.359 + 147.455986*_T - 31.38*_T*log(_T); 
    free_energy(_c) =  _m * ( (1-_c)*G1 
                        + _c*G2
                        + R * _T * (_c * log(_c) + (1.0-_c)*log(1.0-_c))
                        + (1.0-_c)*_c*(6.6944e5-3.347e1*_T));
  }

  else if (_stoi == "U")
  { free_energy(_c) = _m * (G_U + _k * pow(_c - 0,2));
    std::cout << "Free_energy of " << _stoi << " is " << G_U << std::endl;
  }

  // Parse function for automatic differentiation
  functionParse(free_energy);
}
