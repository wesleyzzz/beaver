//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef MULTIFIXEDSMOOTHCIRCLEIC_H
#define MULTIFIXEDSMOOTHCIRCLEIC_H

#include "InitialCondition.h"

// Forward Declarations
class MultiFixedSmoothCircleIC;

template <>
InputParameters validParams<MultiFixedSmoothCircleIC>();

/**
 * MultiFixedsmoothCircleIC creates multiple SmoothCircles (number = numbub) that are 
 * positioned at the given locations
 */
class MultiFixedSmoothCircleIC : public InitialCondition
{
public:
  MultiFixedSmoothCircleIC(const InputParameters & parameters);

  virtual Real value(const Point & p);
  virtual RealGradient gradient(const Point & p);

  virtual void initialSetup();


protected:
  virtual Real computeCircleValue(const Point & p, const Point & center, const Real & radius, const Real & invalue, const Real & outvalue);
  virtual RealGradient
  computeCircleGradient(const Point & p, const Point & center, const Real & radius, const Real & invalue, const Real & outvalue);

  MooseMesh & _mesh;
  std::vector<Point> _centers;
  std::vector<Real> _radii;
  std::vector<Real> _inside;
  Real _outside;
  Real _int_width;
  bool _zero_gradient;

  enum class ProfileType
  {
    COS,
    TANH
  } _profile;

};

#endif // MULTIFIXEDSMOOTHCIRCLEIC_H
