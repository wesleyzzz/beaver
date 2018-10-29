#include "BeaverApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

template <>
InputParameters
validParams<BeaverApp>()
{
  InputParameters params = validParams<MooseApp>();
  return params;
}

registerKnownLabel("BeaverApp");

BeaverApp::BeaverApp(InputParameters parameters) : MooseApp(parameters)
{
  BeaverApp::registerAll(_factory, _action_factory, _syntax);
}

BeaverApp::~BeaverApp() {}

void
BeaverApp::registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  ModulesApp::registerAll(f, af, s);
  Registry::registerObjectsTo(f, {"BeaverApp"});
  Registry::registerActionsTo(af, {"BeaverApp"});

  /* register custom execute flags, action syntax, etc. here */
}

void
BeaverApp::registerApps()
{
  registerApp(BeaverApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
BeaverApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  BeaverApp::registerAll(f, af, s);
}
extern "C" void
BeaverApp__registerApps()
{
  BeaverApp::registerApps();
}
