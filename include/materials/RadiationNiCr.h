//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef RADIATIONNICR_H
#define RADIATIONNICR_H

#include "Material.h"
#include "DerivativeMaterialInterface.h"

class RadiationNiCr;

template <>
InputParameters validParams<RadiationNiCr>();

/**
 * Material to provide a constant value of porosity. This can be specified
 * by either a constant value in the input file, or taken from an aux variable.
 * Note: this material assumes that the porosity remains constant throughout a
 * simulation, so the coupled aux variable porosity must also remain constant.
 */
class RadiationNiCr : public DerivativeMaterialInterface<Material>
{
public:
  RadiationNiCr(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;

  const VariableValue & _Cv;
  const VariableValue & _Ci;
  const VariableValue & _CA;

  const Real _T;
  const Real _Omega;
  const Real _lattice;

  MaterialProperty<Real> & _coef_dAv;
  MaterialProperty<Real> & _coef_dBv;
  MaterialProperty<Real> & _coef_dAi;
  MaterialProperty<Real> & _coef_dBi;
  MaterialProperty<Real> & _Kvi;
};

#endif // RADIATIONNICR_H
