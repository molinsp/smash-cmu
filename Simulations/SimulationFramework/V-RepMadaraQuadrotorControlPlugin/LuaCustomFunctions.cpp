/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#include "MadaraQuadrotorControl.h"
#include "LuaCustomFunctions.h"
#include "utilities/CommonMadaraVariables.h"

#include <string>
using std::string;
using std::stringstream;

/**
 * simRegisterCustomLuaFunction(string luaFunctionName, string tooltip,
 *   int[] argNumberAndTypes, (void)(funcptr*)(SLuaCallBack*));
 */

// The controller used to manage the Madara stuff.
MadaraQuadrotorControl* control = NULL;

// Registers the Lua simExtMadaraQuadrotorControlSetup command.
void registerMadaraQuadrotorControlSetupLuaCallback()
{
    // Define the LUA function input parameters.
    int inArgs[] = {0};

    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraQuadrotorControlSetup",
                                 "simExtMadaraQuadrotorControlSetup()",
                                 inArgs, simExtMadaraQuadrotorControlSetup);
}

// Callback of the Lua simExtMadaraQuadrotorControlSetup command.
void simExtMadaraQuadrotorControlSetup(SLuaCallBack* p)
{
  simLockInterface(1);

  // Check we have to correct amount of parameters.
  if (p->inputArgCount != 0)
  {
    simSetLastError("simExtMadaraQuadrotorControlSetup",
                    "should have no arguments.");
  }
  else
  {
    delete control;
    control = new MadaraQuadrotorControl();
  }

  simLockInterface(0);
}

// Registers the Lua simExtMadaraQuadrotorControlUpdateStatus command.
void registerMadaraQuadrotorControlUpdateStatusLuaCallback()
{
    // Define the LUA function input parameters.
    int inArgs[] = {4, sim_lua_arg_int,     // the id of the drone
                       sim_lua_arg_float,   // The lat position of the drone
                       sim_lua_arg_float,   // The long position of the drone
                       sim_lua_arg_float};  // The alt position of the drone

    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraQuadrotorControlUpdateStatus",
                                 "simExtMadaraQuadrotorControlUpdateStatus"
                                                          "(int droneId, "
                                                           "number droneLat, "
                                                           "number droneLong, "
                                                           "number droneAlt)",
                                 inArgs,
                                 simExtMadaraQuadrotorControlUpdateStatus);
}

// Callback of the Lua simExtMadaraQuadrotorControlUpdateStatus command.
void simExtMadaraQuadrotorControlUpdateStatus(SLuaCallBack* p)
{
  simLockInterface(1);

  // Check we have to correct amount of parameters.
  if (p->inputArgCount != 4)
  { 
    simSetLastError("simExtMadaraQuadrotorControlUpdateStatus",
                    "Incorrect number of arguments.");
  }
  // Check we have the correct type of arguments.
  else if (p->inputArgTypeAndSize[0*2+0] != sim_lua_arg_int)
  {
    simSetLastError("simExtMadaraQuadrotorControlUpdateStatus",
      "The id of the drone is not an int.");
  }
  else if(p->inputArgTypeAndSize[1*2+0] != sim_lua_arg_float)
  {
    simSetLastError("simExtMadaraQuadrotorControlUpdateStatus",
      "The latitude of the drone is not a float.");
  }
  else if(p->inputArgTypeAndSize[2*2+0] != sim_lua_arg_float)
  {
    simSetLastError("simExtMadaraQuadrotorControlUpdateStatus",
      "The longitude of the drone is not a float.");
  }
  else if(p->inputArgTypeAndSize[3*2+0] != sim_lua_arg_float)
  {
    simSetLastError("simExtMadaraQuadrotorControlUpdateStatus",
      "The altitude of the drone is not a float.");
  }
  else // params checked out
  { 
    // Propagate the status information through the network.
    MadaraQuadrotorControl::Status status;
    status.m_id = p->inputInt[0];
    status.m_loc.m_lat = p->inputFloat[0];
    status.m_loc.m_long = p->inputFloat[1];
    status.m_loc.m_alt = p->inputFloat[2];
    control->updateQuadrotorStatus(status);
  }

  simLockInterface(0);
}

// Registers the Lua simExtMadaraQuadrotorControlGetNewCommand command.
void registerMadaraQuadrotorControlGetNewCmdLuaCallback()
{
    // Define the LUA function input parameters.
    int inArgs[] = {1, sim_lua_arg_int};

    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraQuadrotorControlGetNewCmd",
                                 "cmd, lat, long, alt = "
                                 "simExtMadaraQuadrotorControlGetNewCmd(int droneId)",
                                 inArgs, simExtMadaraQuadrotorControlGetNewCmd);
}

// Callback of the Lua simExtMadaraQuadrotorControlGetNewCommand command.
void simExtMadaraQuadrotorControlGetNewCmd(SLuaCallBack* p)
{
  simLockInterface(1);

  MadaraQuadrotorControl::Command *temp = NULL;

  // Check we have to correct amount of parameters.
  if (p->inputArgCount != 1)
  { 
    simSetLastError("simExtMadaraQuadrotorControlGetNewCommand",
                    "Need drone ID arg");
  }
  // Check we have the correct type of arguments.
  else if ( p->inputArgTypeAndSize[0*2+0] != sim_lua_arg_int )
  {
    simSetLastError("simExtMadaraQuadrotorControlGetNewCommand",
                    "droneId parameter is not an int.");
  }
  else
  { 
    temp = control->getNewCommand(p->inputInt[0]);
  }

  // Now we prepare the return value(s):
  p->outputArgCount = 4;
  p->outputArgTypeAndSize = (simInt*)simCreateBuffer(
      p->outputArgCount * (2 * sizeof(simInt)));

  // Set the actual return values depending on whether we found a position or not.
  if(temp != NULL)
  {
    p->outputArgTypeAndSize[2*0+0] = sim_lua_arg_string; // cmd
    p->outputArgTypeAndSize[2*0+1] = 1;
    p->outputArgTypeAndSize[2*1+0] = sim_lua_arg_float; // lat
    p->outputArgTypeAndSize[2*1+1] = 1;
    p->outputArgTypeAndSize[2*2+0] = sim_lua_arg_float; // long
    p->outputArgTypeAndSize[2*2+1] = 1;
    p->outputArgTypeAndSize[2*3+0] = sim_lua_arg_float; // altitude
    p->outputArgTypeAndSize[2*3+1] = 1;

    // copy cmd
    p->outputChar = (simChar*) simCreateBuffer(
      (temp->m_command.length() + 1) * sizeof(simChar));
    unsigned int i = 0;
    for(; i < temp->m_command.length(); ++i)
      p->outputChar[i] = temp->m_command.at(i);
    p->outputChar[i] = '\0'; // null terminate

    // copy x, y, z coords
    p->outputFloat = (simFloat*) simCreateBuffer(3 * sizeof(simFloat));
    p->outputFloat[0] = (float) temp->m_loc.m_lat;
    p->outputFloat[1] = (float) temp->m_loc.m_long;
    p->outputFloat[2] = (float) temp->m_loc.m_alt;
  }
  else
  {
      // All values will just be nil.
      p->outputArgTypeAndSize[2*0+0] = sim_lua_arg_nil;
      p->outputArgTypeAndSize[2*0+1] = 1;
      p->outputArgTypeAndSize[2*1+0] = sim_lua_arg_nil;
      p->outputArgTypeAndSize[2*1+1] = 1;
      p->outputArgTypeAndSize[2*2+0] = sim_lua_arg_nil;
      p->outputArgTypeAndSize[2*2+1] = 1;
      p->outputArgTypeAndSize[2*3+0] = sim_lua_arg_nil;
      p->outputArgTypeAndSize[2*3+1] = 1;
  }

  simLockInterface(0);
}

// Register test of isGoToCmd
void registerMadaraQuadrotorControlIsGoToCmdLuaCallback()
{
  // Define the LUA function input parameters.
  int inArgs[] = {1, sim_lua_arg_string};

  // Register the simExtGetPositionInBridge function.
  simRegisterCustomLuaFunction("simExtMadaraQuadrotorControlIsGoToCmd",
    "isGoTo = simExtMadaraQuadrotorControlIsGoToCmd(string cmd)",
    inArgs, simExtMadaraQuadrotorControlIsGoToCmd);
}

// isGoToCmd
void simExtMadaraQuadrotorControlIsGoToCmd(SLuaCallBack* p)
{
  simLockInterface(1);

  // Check we have to correct amount of parameters.
  if (p->inputArgCount != 1)
  { 
    simSetLastError("simExtMadaraQuadrotorControlIsGoToCmd",
                    "Need command to test");
  }
  // Check we have the correct type of arguments.
  else if(p->inputArgTypeAndSize[0*2+0] != sim_lua_arg_string)
  {
    simSetLastError("simExtMadaraQuadrotorControlIsGoToCmd",
                    "command parameter is not a string");
  }
  else
  { 
    // Now we prepare the return value(s):
    p->outputArgCount = 1;
    p->outputArgTypeAndSize = (simInt*)simCreateBuffer(
      p->outputArgCount * (2 * sizeof(simInt)));
    p->outputArgTypeAndSize[2*0+0] = sim_lua_arg_bool;
    p->outputArgTypeAndSize[2*0+1] = 1;
    p->outputBool = (simBool*)simCreateBuffer(sizeof(simBool));
    p->outputBool[0] = (!strcmp(MO_MOVE_TO_GPS_CMD, p->inputChar));
  }

  simLockInterface(0);
}

// Regiest test of isOffCmd
void registerMadaraQuadrotorControlIsLandCmdLuaCallback()
{
  // Define the LUA function input parameters.
  int inArgs[] = {1, sim_lua_arg_string};

  // Register the simExtGetPositionInBridge function.
  simRegisterCustomLuaFunction("simExtMadaraQuadrotorControlIsLandCmd",
    "isLand = simExtMadaraQuadrotorControlIsLandCmd(string cmd)",
    inArgs, simExtMadaraQuadrotorControlIsLandCmd);
}

// isOffCmd
void simExtMadaraQuadrotorControlIsLandCmd(SLuaCallBack* p)
{
  simLockInterface(1);

  // Check we have to correct amount of parameters.
  if (p->inputArgCount != 1)
  { 
    simSetLastError("simExtMadaraQuadrotorControlIsLandCmd",
                    "need command to test");
  }
  // Check we have the correct type of arguments.
  else if(p->inputArgTypeAndSize[0*2+0] != sim_lua_arg_string)
  {
    simSetLastError("simExtMadaraQuadrotorControlIsLandCmd",
                    "command parameter is not a string");
  }
  else
  { 
    // Now we prepare the return value(s):
    p->outputArgCount = 1;
    p->outputArgTypeAndSize = (simInt*)simCreateBuffer(
      p->outputArgCount * (2 * sizeof(simInt)));
    p->outputArgTypeAndSize[2*0+0] = sim_lua_arg_bool;
    p->outputArgTypeAndSize[2*0+1] = 1;
    p->outputBool = (simBool*)simCreateBuffer(sizeof(simBool));
    p->outputBool[0] = (!strcmp(MO_LAND_CMD, p->inputChar));
  }

  simLockInterface(0);
}

// Register test of isOnCmd
void registerMadaraQuadrotorControlIsTakeoffCmdLuaCallback()
{
  // Define the LUA function input parameters.
  int inArgs[] = {1, sim_lua_arg_string};

  // Register the simExtGetPositionInBridge function.
  simRegisterCustomLuaFunction("simExtMadaraQuadrotorControlIsTakeoffCmd",
    "isTakeoff = simExtMadaraQuadrotorControlIsTakeoffCmd(string cmd)",
    inArgs, simExtMadaraQuadrotorControlIsTakeoffCmd);
}

// isOnCmd
void simExtMadaraQuadrotorControlIsTakeoffCmd(SLuaCallBack* p)
{
  simLockInterface(1);

  // Check we have to correct amount of parameters.
  if (p->inputArgCount != 1)
  { 
    simSetLastError("simExtMadaraQuadrotorControlIsTakeoffCmd",
                    "need command to test");
  }
  // Check we have the correct type of arguments.
  else if(p->inputArgTypeAndSize[0*2+0] != sim_lua_arg_string)
  {
    simSetLastError("simExtMadaraQuadrotorControlIsTakeoffCmd",
                    "command parameter is not a string");
  }
  else
  { 
    // Now we prepare the return value(s):
    p->outputArgCount = 1;
    p->outputArgTypeAndSize = (simInt*)simCreateBuffer(
      p->outputArgCount * (2 * sizeof(simInt)));
    p->outputArgTypeAndSize[2*0+0] = sim_lua_arg_bool;
    p->outputArgTypeAndSize[2*0+1] = 1;
    p->outputBool = (simBool*)simCreateBuffer(sizeof(simBool));
    p->outputBool[0] = (!strcmp(MO_TAKEOFF_CMD, p->inputChar));
  }

  simLockInterface(0);
}

// Registers the Lua simExtMadaraQuadrotorControlCleanup command.
void registerMadaraQuadrotorControlCleanupLuaCallback()
{
    // Define the LUA function input parameters.
    int inArgs[] = {0};

    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraQuadrotorControlCleanup",
                                 "simExtMadaraQuadrotorControlCleanup()",
                                 inArgs, simExtMadaraQuadrotorControlCleanup);
}

// Callback of the Lua simExtMadaraQuadrotorControlCleanup command.
void simExtMadaraQuadrotorControlCleanup(SLuaCallBack* /*p*/)
{
    //WIN_AFX_MANAGE_STATE;
    simLockInterface(1);

    // Simply cleanup the madara control.
    simAddStatusbarMessage("simExtMadaraQuadrotorControlCleanup: cleaning up");
    if(control != NULL)
    {
        control->terminate();
        delete control;
        control = NULL;
    }

    simLockInterface(0);
}
