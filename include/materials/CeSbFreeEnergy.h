//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef CESBFREEENERGY_H
#define CESBFREEENERGY_H

#include "DerivativeParsedMaterialHelper.h"
#include "ExpressionBuilder.h"

// Forward Declarations
class CeSbFreeEnergy;

template <>
InputParameters validParams<CeSbFreeEnergy>();

/**
 * Material class that provides the free energy of an ideal gas with the expression builder
 * and uses automatic differentiation to get the derivatives.
 */
class CeSbFreeEnergy : public DerivativeParsedMaterialHelper, public ExpressionBuilder
{
public:
  CeSbFreeEnergy(const InputParameters & parameters);

protected:
  /// Coupled variable value for the concentration \f$ c \f$.
  const EBTerm _c;

  MooseEnum _stoi;

  /// unit conversion 
  const Real _T;
  const Real _k;
  const Real _m;

};

#endif 
