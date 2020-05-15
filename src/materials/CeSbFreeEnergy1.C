//* Track Sb concentration

#include "CeSbFreeEnergy1.h"

registerMooseObject("BeaverApp", CeSbFreeEnergy1);

template <>
InputParameters
validParams<CeSbFreeEnergy1>()
{
  InputParameters params = validParams<DerivativeParsedMaterialHelper>();
  MooseEnum stoi("Ce2Sb Ce4Sb3 CeSb CeSb2 Liquid");
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
                        "the units of omega (default in [eV*s])");
  params.addParam<Real>("kB", 8.6173303e-5, "Boltzmann constant (default in [eV/K])");
  return params;
}

CeSbFreeEnergy1::CeSbFreeEnergy1(const InputParameters & parameters)
  : DerivativeParsedMaterialHelper(parameters),
    _c("c"),//Sb concentration
    _stoi(getParam<MooseEnum>("stoi")),
    _T(getParam<Real>("temperature")),
    _k(getParam<Real>("k")),
    _m(getParam<Real>("unit_conversion"))
{

  
  Real R = 8.3144598;// gas constant  J/mol/K
  Real g_offset = 0.0;//offset free energy to help converging issue

  if (_T<298.15)
    mooseError("Temperature should be higher than 298.15!");

  Real G_Ce = 0.0; //free energy in standard element reference state (SGTE) in FCC phase
  if (_T<1000 && _T>298.15)
    G_Ce = -7160.519 + 84.23022*_T - 22.3664* _T*log(_T)-6.7103e-3*pow(_T,2) - 0.320773e-6*pow(_T,3) - 18117/_T;
  else if (_T<2000)
    G_Ce = -79678.506 + 659.4604*_T - 101.3224803* _T*log(_T) + 26.046487e-3*pow(_T,2) - 1.930297e-6*pow(_T,3) + 11531707/_T;
  else if (_T<4000)
    G_Ce = -14198.639 + 190.370192*_T - 37.6978*_T*log(_T); 

  Real G_Sb = 0.0; //free energy in standard element reference state (SGTE) in Rhombo phase
  if (_T<903.78 && _T>298.15)
    G_Sb = -9242.858 + 156.154689*_T - 30.5130752*_T*log(_T) + 7.748768e-3*pow(_T,2)- 3.003415e-6*pow(_T,3) + 100625/_T;
  else if (_T<2000)
    G_Sb = -11738.83 + 169.485872*_T - 31.38*_T*log(_T) + 1.6168e27*pow(_T,-9);


  // Definition of the free energy for the expression builder
  EBFunction free_energy;

  if (_stoi == "Ce2Sb")
  {
    Real a = -3.0157345e5;
    Real b = 7.38640874e-1;
    free_energy(_c) = 1.0/3* _m * (g_offset + 2.0*G_Ce
                        + G_Sb 
                        + a + b * _T + _k * pow(_c - 1.0/3,2)); // last term to penalize any deviation from the intermetallic composition CeSb
    std::cout << "ce2sb const: " << _stoi << " " << 1.0/3* (g_offset+2.0*G_Ce + G_Sb + a + b * _T ) << std::endl;
  }
  else if (_stoi == "Ce4Sb3")
  {
    Real a = -8.23375293e5;
    Real b = -1.83498253e1;
    free_energy(_c) = 1.0/7* _m * (g_offset + 4.0*G_Ce
                        + 3.0*G_Sb
                        + a + b * _T + _k * pow(_c - 3.0/7,2)); // last term to penalize any deviation from the intermetallic composition CeSb
    std::cout << "ce4sb3 const: " << _stoi << " " << 1.0/7*(g_offset+4.0*G_Ce + 3.0*G_Sb + a + b * _T)  << std::endl;
  }
  else if (_stoi == "CeSb")
  {
    Real a = -2.55927469e5;
    Real b = -8.48454109;
    free_energy(_c) = 1.0/2* _m * (G_Ce
                        + G_Sb 
                        + a + b * _T + _k * pow(_c - 0.5,2)); // last term to penalize any deviation from the intermetallic composition CeSb

  }
  else if (_stoi == "CeSb2")
  {
    Real a = -2.67537672e5;
    Real b = -1.89049169e1;
    free_energy(_c) = 1.0/3* _m * (G_Ce
                        + 2.0*G_Sb 
                        + a + b * _T + _k * pow(_c - 2.0/3,2)); // last term to penalize any deviation from the intermetallic composition CeSb

  }
  else if (_stoi == "Liquid")
  {
    Real G1 = 0.0;//Ce molar gibbs energy in liquid
    if (_T<1000 && _T>298.15)
      G1 = 4117.865-11.423898*_T-7.5383948*_T*log(_T)-29.36407e-3*_T*_T+4.827734e-6*pow(_T,3)-198834/_T ;
    else if (_T<4000)
      G1 = -6730.605 + 183.023193*_T - 37.6978* _T*log(_T);

    Real G2 = 0.0;//Sb molar gibbs energy in liquid
    if (_T<903.78 && _T>298.15)
      G2 = 10579.47 + 134.231525 *_T - 30.5130752*_T*log(_T) + 7.748768e-3*pow(_T,2) - 3.003415e-6*pow(_T,3) + 100625/_T-1.7485e-20*pow(_T,7);
    else if (_T<2000)
      G2 = 8175.359 + 147.455986*_T - 31.38*_T*log(_T); 
    free_energy(_c) =  _m * ( (1-_c)*G1 
                        + _c*G2
                        + R * _T * ((1.0-_c)*log(1.0-_c) + _c * log(_c))
                        + (1.0-_c)*_c*((-1.30910371e5-1.43287294e2*_T) + (-1.61907843e4 + 1.87139283e1*_T)*(2*_c-1.0)));// last term for excess free energy of liquid phase 
    //std::cout << "liquid : " << G1 << " " << G2  << std::endl;
  }
  // Parse function for automatic differentiation
  functionParse(free_energy);
}
