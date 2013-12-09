/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

#include "LuaFunctionRegistration.h"
#include "LuaExtensionsUtils.h"
#include "ISimplePlugin.h"

#include <string>
#include <sstream>

/**
* simRegisterCustomLuaFunction(string luaFunctionName, string tooltip,
*   int[] argNumberAndTypes, (void)(funcptr*)(SLuaCallBack*));
*/

// This is the plugin we will use.
VREP::ISimplePlugin* g_plugin;

///////////////////////////////////////////////////////////////////////////////
// Sets up the g_plugin.
///////////////////////////////////////////////////////////////////////////////
// Define the LUA function input parameters.
static int g_setupInArgs[] = {1, sim_lua_arg_int};    // The suffix of the script.

// The actual callback function.
void simExt_PluginSetup(SLuaCallBack* p)
{
  simLockInterface(1);

  // Check we have to correct amount of parameters.
  bool paramsOk = checkInputArguments(p, g_setupInArgs, "simExt_PluginSetup");
  if(paramsOk)
  {
    int suffix = p->inputInt[0];

    // For debugging, print out what we received.
    std::stringstream sstm; 
    sstm << "Values received inside simExt_PluginSetup function: suffix:" << 
      suffix << std::endl;
    simAddStatusbarMessage(sstm.str().c_str());

    // Call the plugins' initialization function.
    g_plugin->initialize(suffix);
  }

  simLockInterface(0);
}

// Registers the Lua simExt_PluginSetup command.
void registerPluginSetup()
{
  // Build the function name.
  std::string pluginFunctionPrefix = "simExt_PluginSetup_";
  std::string pluginName = g_plugin->getId();
  std::string functionName = pluginFunctionPrefix + pluginName;
  std::string functionSignature = functionName + "(int suffix)";

  // Register the function.
  simRegisterCustomLuaFunction(functionName.c_str(),
    functionSignature.c_str(),
    g_setupInArgs, 
    simExt_PluginSetup);
}

///////////////////////////////////////////////////////////////////////////////
// Cleans up the g_plugin.
///////////////////////////////////////////////////////////////////////////////
// Define the LUA function input parameters.
static int g_cleanupInArgs[] = {1, sim_lua_arg_int}; // The suffix of the script.

// The actual callback function.
void simExt_PluginCleanup(SLuaCallBack* p)
{
  simLockInterface(1);

  // Check we have to correct amount of parameters.
  bool paramsOk = checkInputArguments(p, g_cleanupInArgs, "simExt_PluginCleanup");
  if(paramsOk)
  {
    int suffix = p->inputInt[0];

    // Call the plugins' cleanup function.
    g_plugin->cleanup(suffix);
  }

  simLockInterface(0);
}

// Registers the Lua simExt_PluginCleanup command.
void registerPluginCleanup()
{
  // Build the function name.
  std::string pluginFunctionPrefix = "simExt_PluginCleanup_";
  std::string pluginName = g_plugin->getId();
  std::string functionName = pluginFunctionPrefix + pluginName;
  std::string functionSignature = functionName + "()";

  // Register the function.
  simRegisterCustomLuaFunction(functionName.c_str(),
    functionSignature.c_str(),
    g_cleanupInArgs, 
    simExt_PluginCleanup);
}

///////////////////////////////////////////////////////////////////////////////
// Executes a simulation step in the g_plugin.
///////////////////////////////////////////////////////////////////////////////
// Define the LUA function input parameters.
static int g_executeStepInArgs[] = {1, sim_lua_arg_int}; // The suffix of the script.

// The actual callback function.
void simExt_PluginExecuteStep(SLuaCallBack* p)
{
  simLockInterface(1);

  // Check we have to correct amount of parameters.
  bool paramsOk = checkInputArguments(p, g_executeStepInArgs, 
    "simExt_PluginExecuteStep");
  if(paramsOk)
  {
    int suffix = p->inputInt[0];

    // Call the plugins' step function.
    g_plugin->executeStep(suffix);
  }

  simLockInterface(0);
}

// Registers the Lua simExt_PluginExecuteStep command.
void registerPluginExecuteStep()
{
  // Build the function name.
  std::string pluginFunctionPrefix = "simExt_PluginExecuteStep_";
  std::string pluginName = g_plugin->getId();
  std::string functionName = pluginFunctionPrefix + pluginName;
  std::string functionSignature = functionName + "()";

  // Register the function.
  simRegisterCustomLuaFunction(functionName.c_str(),
    functionSignature.c_str(),
    g_executeStepInArgs, 
    simExt_PluginExecuteStep);
}

///////////////////////////////////////////////////////////////////////////////
// Registers all Lua extensions for a simple g_plugin.
///////////////////////////////////////////////////////////////////////////////
void registerAllLuaExtensions()
{
  // First create the actual g_plugin. Note that this function will have to be 
  // implemented by the actual
  // g_plugin.
  g_plugin = createPlugin();

  // Then register all its Lua extensions.
  registerPluginSetup();
  registerPluginCleanup();
  registerPluginExecuteStep();
}