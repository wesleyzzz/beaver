//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#ifndef BEAVERTESTAPP_H
#define BEAVERTESTAPP_H

#include "MooseApp.h"

class BeaverTestApp;

template <>
InputParameters validParams<BeaverTestApp>();

class BeaverTestApp : public MooseApp
{
public:
  BeaverTestApp(InputParameters parameters);
  virtual ~BeaverTestApp();

  static void registerApps();
  static void registerAll(Factory & f, ActionFactory & af, Syntax & s, bool use_test_objs = false);
};

#endif /* BEAVERTESTAPP_H */
