//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MultiFixedSmoothCircleIC.h"

// MOOSE includes
#include "MooseMesh.h"
#include "MooseVariable.h"
#include "MooseMesh.h"
#include "libmesh/utility.h"

registerMooseObject("BeaverApp", MultiFixedSmoothCircleIC);

template <>
InputParameters
validParams<MultiFixedSmoothCircleIC>()
{
  InputParameters params = validParams<InitialCondition>();
  params.addClassDescription("Specify location of circles and apply inside and outside values for each box");
  params.addRequiredParam<std::vector<Point>>("centers","centers of the bounding circles");
  params.addRequiredParam<std::vector<Real>>("radii", "Radii for all the circles");
  params.addRequiredParam<std::vector<Real>>("inside","The value of the variable in each boudning circle");
  params.addParam<Real>("int_width", 0.0, "The interfacial width of the void surface.  Defaults to sharp interface");
  params.addParam<Real>("outside",0.0,"The value of the variable outside all the circles");
  params.addParam<bool>("zero_gradient",
                        false,
                        "Set the gradient DOFs to zero. This can avoid "
                        "numerical problems with higher order shape "
                        "functions and overlapping circles.");
  MooseEnum profileType("COS TANH", "COS");
  params.addParam<MooseEnum>(
      "profile", profileType, "Functional dependence for the interface profile");
  return params;
}

MultiFixedSmoothCircleIC::MultiFixedSmoothCircleIC(const InputParameters & parameters)
  : InitialCondition(parameters),
    _mesh(_fe_problem.mesh()),
    _centers(getParam<std::vector<Point>>("centers")),
    _radii(getParam<std::vector<Real>>("radii")),
    _inside(getParam<std::vector<Real>>("inside")),
    _outside(getParam<Real>("outside")),
    _int_width(parameters.get<Real>("int_width")),
    _zero_gradient(parameters.get<bool>("zero_gradient")),
    _profile(getParam<MooseEnum>("profile").getEnum<ProfileType>())
{
  if (_centers.size() != _radii.size() && _centers.size() != _inside.size())
    mooseError("Vector inputs must be at the same size"); 
  for (unsigned int circ = 0; circ < _centers.size() ; ++circ)
  {
    if (std::fabs(_centers[circ](2))>1.0e-15)
      mooseError("Z coords must be zero"); 
  }
}

void
MultiFixedSmoothCircleIC::initialSetup()
{
  // Check if there is overlap circles
  for (unsigned int i = 0; i < _centers.size() ; ++i)
  {
    for (unsigned int j = i+1; j < _centers.size() ; ++j)
    {
      // Compute the distance between the current point and the center
      Real dist = _mesh.minPeriodicDistance(_var.number(), _centers[i], _centers[j]);
      if (dist<_radii[i]+_radii[j])
        mooseError("Overlapped circles are not allowed");
    }
  }
  if (_centers.size() != _radii.size())
    mooseError("_center and _radii vectors are not the same size in the Circle IC");

  if (_centers.size() < 1)
    mooseError("_center and _radii were not initialized in the Circle IC");
}

Real
MultiFixedSmoothCircleIC::value(const Point & p)
{
  Real dist = 1.0e10;
  unsigned int minCirc = 0;

  for (unsigned int circ = 0; circ < _centers.size(); ++circ)
  {
    Real d = _mesh.minPeriodicDistance(_var.number(), p, _centers[circ]);
    if (d < dist)
    {
      dist = d;
      minCirc = circ;
    }
  }
  Real value = computeCircleValue(p, _centers[minCirc], _radii[minCirc],_inside[minCirc],_outside);
  return value;
}

RealGradient
MultiFixedSmoothCircleIC::gradient(const Point & p)
{
  if (_zero_gradient)
    return 0.0;

  RealGradient gradient = 0.0;
  unsigned int minCirc = 0;

  Real dist = 1.0e10;
  for (unsigned int circ = 0; circ < _centers.size(); ++circ)
  {
    Real d = _mesh.minPeriodicDistance(_var.number(), p, _centers[circ]);
    if (d < dist)
    {
      dist = d;
      minCirc = circ;
    }
  }

  gradient = computeCircleGradient(p, _centers[minCirc], _radii[minCirc],_inside[minCirc],_outside);
  return gradient;
}

Real
MultiFixedSmoothCircleIC::computeCircleValue(const Point & p, const Point & center, const Real & radius, const Real & invalue, const Real & outvalue)
{
  Point l_center = center;
  Point l_p = p;
  // Compute the distance between the current point and the center
  Real dist = _mesh.minPeriodicDistance(_var.number(), l_p, l_center);

  switch (_profile)
  {
    case ProfileType::COS:
    {
      // Return value
      Real value = outvalue; // Outside circle

      if (dist <= radius - _int_width / 2.0) // Inside circle
        value = invalue;
      else if (dist < radius + _int_width / 2.0) // Smooth interface
      {
        Real int_pos = (dist - radius + _int_width / 2.0) / _int_width;
        value = outvalue + (invalue - outvalue) * (1.0 + std::cos(int_pos * libMesh::pi)) / 2.0;
      }
      return value;
    }

    case ProfileType::TANH:
      return (invalue - outvalue) * 0.5 *
                 (std::tanh(libMesh::pi * (radius - dist) / _int_width) + 1.0) +
             outvalue;

    default:
      mooseError("Internal error.");
  }
}

RealGradient
MultiFixedSmoothCircleIC::computeCircleGradient(const Point & p,
                                          const Point & center,
                                          const Real & radius, const Real & invalue, const Real & outvalue)
{
  Point l_center = center;
  Point l_p = p;

  // Compute the distance between the current point and the center
  Real dist = _mesh.minPeriodicDistance(_var.number(), l_p, l_center);

  // early return if we are probing the center of the circle
  if (dist == 0.0)
    return 0.0;

  Real DvalueDr = 0.0;
  switch (_profile)
  {
    case ProfileType::COS:
      if (dist < radius + _int_width / 2.0 && dist > radius - _int_width / 2.0)
      {
        const Real int_pos = (dist - radius + _int_width / 2.0) / _int_width;
        const Real Dint_posDr = 1.0 / _int_width;
        DvalueDr = Dint_posDr * (invalue - outvalue) *
                   (-std::sin(int_pos * libMesh::pi) * libMesh::pi) / 2.0;
      }
      break;

    case ProfileType::TANH:
      DvalueDr = -(invalue - outvalue) * 0.5 / _int_width * libMesh::pi *
                 (1.0 - Utility::pow<2>(std::tanh(4.0 * (radius - dist) / _int_width)));
      break;

    default:
      mooseError("Internal error.");
  }

  return _mesh.minPeriodicVector(_var.number(), center, p) * (DvalueDr / dist);
}
