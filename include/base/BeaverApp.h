//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#ifndef BEAVERAPP_H
#define BEAVERAPP_H

#include "MooseApp.h"

class BeaverApp;

template <>
InputParameters validParams<BeaverApp>();

class BeaverApp : public MooseApp
{
public:
  BeaverApp(InputParameters parameters);
  virtual ~BeaverApp();

  static void registerApps();
  static void registerAll(Factory & f, ActionFactory & af, Syntax & s);
};

#endif /* BEAVERAPP_H */
