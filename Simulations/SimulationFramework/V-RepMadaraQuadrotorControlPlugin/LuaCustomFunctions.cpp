/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

#include "MadaraQuadrotorControl.h"
#include "LuaFunctionRegistration.h"
#include "LuaExtensionsUtils.h"
#include "utilities/CommonMadaraVariables.h"
#include "utilities/Position.h"

#include <string>
using std::string;
using std::stringstream;

/**
 * simRegisterCustomLuaFunction(string luaFunctionName, string tooltip,
 *   int[] argNumberAndTypes, (void)(funcptr*)(SLuaCallBack*));
 */

// The controller used to manage the Madara stuff.
static MadaraQuadrotorControl* control = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraQuadrotorControlSetup command.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define the LUA function input parameters.
static int g_setupInArgs[] = {1, sim_lua_arg_int};    // The id of the drone.

// The actual callback function.
void simExtMadaraQuadrotorControlSetup(SLuaCallBack* p)
{
	simLockInterface(1);

	// Check we have to correct amount of parameters.
	bool paramsOk = checkInputArguments(p, g_setupInArgs, "simExtMadaraQuadrotorControlSetup");
	if(paramsOk)
	{
		int droneId = p->inputInt[0];
		if(control != NULL)
			delete control;
		control = new MadaraQuadrotorControl(droneId);
	}

	simLockInterface(0);
}

// Registers the Lua simExtMadaraQuadrotorControlSetup command.
void registerMadaraQuadrotorControlSetupLuaCallback()
{
	// Register the simExtGetPositionInBridge function.
	simRegisterCustomLuaFunction("simExtMadaraQuadrotorControlSetup",
		"simExtMadaraQuadrotorControlSetup(int droneId)",
		g_setupInArgs, simExtMadaraQuadrotorControlSetup);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraQuadrotorControlUpdateStatus command.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define the LUA function input parameters.
static int g_updateStatusInArgs[] = {4, 
	sim_lua_arg_int,      // The id of the drone.
	sim_lua_arg_string,   // The lat position of the drone.
	sim_lua_arg_string,   // The long position of the drone.
	sim_lua_arg_string};  // The alt position of the drone.


// The actual callback function.
void simExtMadaraQuadrotorControlUpdateStatus(SLuaCallBack* p)
{
	simLockInterface(1);

	// Check we have to correct amount of parameters.
	bool paramsOk = checkInputArguments(p, g_updateStatusInArgs, "simExtMadaraQuadrotorControlUpdateStatus");
	if(paramsOk)
	{
		// Get the values from the concatenated string with all of them.
		// NOTE: these parameters are sent as strings since there seems to be problems with large double numbers between Lua and C++ in Vrep.
		std::string lat(p->inputChar);
		std::string lon(p->inputChar+lat.length()+1);
		std::string alt(p->inputChar+lat.length()+lon.length()+2);

		// Propagate the status information through the network.
		MadaraQuadrotorControl::Status status;
		status.m_id = p->inputInt[0];
		status.m_loc.m_lat = atof(lat.c_str());
		status.m_loc.m_long = atof(lon.c_str());
		status.m_loc.m_alt = atof(alt.c_str());
		control->updateQuadrotorStatus(status);

		// For debugging, print out what we received.
		//std::stringstream sstm; 
		//sstm << "Values received inside simExtMadaraQuadrotorControlUpdateStatus function: bridgeId:" << status.m_id << ", "
		//    << " (" << std::setprecision(20) << status.m_loc.m_lat << "," <<  std::setprecision(20) << status.m_loc.m_long << "," << status.m_loc.m_alt << ")"
		//    << std::endl;
		//std::string message = sstm.str();
		//simAddStatusbarMessage(message.c_str());
	}

	simLockInterface(0);
}

// Registers the Lua simExtMadaraQuadrotorControlUpdateStatus command.
void registerMadaraQuadrotorControlUpdateStatusLuaCallback()
{
	// Register the simExtGetPositionInBridge function.
	simRegisterCustomLuaFunction("simExtMadaraQuadrotorControlUpdateStatus",
		"simExtMadaraQuadrotorControlUpdateStatus"
		"(int droneId, "
		"number droneLat, "
		"number droneLong, "
		"number droneAlt)",
		g_updateStatusInArgs,
		simExtMadaraQuadrotorControlUpdateStatus);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraQuadrotorControlGetNewCommand command.
// Will always return 4 parameters, though they will be nil by default.
// Returns:
// - Return 1: will always be the name of the command.
//   For the MOVE_TO_GPS command:
//		- Return 2: the latitude as a string (to avoid some Lua-C loss of precision)
//		- Return 3: the longitude as a string (to avoid some Lua-C loss of precision)
//   For the MOVE_TO_ALT command:
//		- Return 2: the altitude as a string (to avoid some Lua-C loss of precision)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define the LUA function input parameters.
static int g_getNewCmdInArgs[] = {1, sim_lua_arg_int};

// The actual callback function.
void simExtMadaraQuadrotorControlGetNewCmd(SLuaCallBack* p)
{
	simLockInterface(1);

	MadaraQuadrotorControl::Command *newCommand = NULL;

	// Check we have to correct amount of parameters.
	bool paramsOk = checkInputArguments(p, g_getNewCmdInArgs, "simExtMadaraQuadrotorControlGetNewCmd");
	if(paramsOk)
	{ 
		// We check if a new command has arrived for us.
		int droneId = p->inputInt[0];
		newCommand = control->getNewCommand(droneId);
	}

	// Prepare the output description, with nil types by default.
	int maxNumberOfOutputs = 3;
	setupDefaultOutput(p, maxNumberOfOutputs);

	// Set the actual return values depending on whether we found a position or not.
	if(newCommand != NULL)
	{
		// For debugging, to print out what we received.
		std::stringstream sstm;
		sstm << "Values received inside simExtMadaraQuadrotorControlGetNewCmd function: command: " << newCommand->m_command << std::endl;

		// First case: MOVE_TO_GPS
		if(strcmp(MO_MOVE_TO_GPS_CMD, newCommand->m_command.c_str()) == 0)
		{
			// For debugging, print out what we received.
			sstm << "Values received inside simExtMadaraQuadrotorControlGetNewCmd function: (" << std::setprecision(10) << newCommand->m_loc.m_lat << "," << newCommand->m_loc.m_long << ")"
				<< std::endl;

			// All commands will have at least the command name, though they may have different parameters.
			p->outputArgTypeAndSize[0] = sim_lua_arg_string; // cmd

			// Move to gps has lat and long parameters, which we will have to return.
			p->outputArgTypeAndSize[2*1+0] = sim_lua_arg_string; // lat
			p->outputArgTypeAndSize[2*2+0] = sim_lua_arg_string; // long

			// Turn floats into strings to avoid losing precision when transfering back to Lua.
			std::string lat(NUM_TO_STR(newCommand->m_loc.m_lat));
			std::string lon(NUM_TO_STR(newCommand->m_loc.m_long));

			// Put the values in the char output buffer.
			int numOutputs = 3;
			std::vector<std::string> outputStrings(numOutputs);
			outputStrings[0] = newCommand->m_command;
			outputStrings[1] = lat;
			outputStrings[2] = lon;
			setupStringOutputBuffer(p, outputStrings);
		}
		else if(strcmp(MO_MOVE_TO_ALTITUDE_CMD, newCommand->m_command.c_str()) == 0)
		{
			// For debugging, print out what we received.
			sstm << "Values received inside simExtMadaraQuadrotorControlGetNewCmd function: (" << std::setprecision(10) << newCommand->m_loc.m_alt << ")" << std::endl;

			// All commands will have at least the command name, though they may have different parameters.
			p->outputArgTypeAndSize[0] = sim_lua_arg_string; // cmd

			// The only parameter other than the command is the altitude.
			p->outputArgTypeAndSize[2*1+0] = sim_lua_arg_string; // altitude

			// Turn floats into strings to avoid losing precision when transfering back to Lua.
			std::string alt(NUM_TO_STR(newCommand->m_loc.m_alt));

			// Put the values in the char output buffer.
			int numOutputs = 2;
			std::vector<std::string> outputStrings(numOutputs);
			outputStrings[0] = newCommand->m_command;
			outputStrings[1] = alt;
			setupStringOutputBuffer(p, outputStrings);
		}

		// copy x, y, z coords
		//p->outputFloat = (simFloat*) simCreateBuffer(3 * sizeof(simFloat));
		//p->outputFloat[0] = (float) temp->m_loc.m_lat;
		//p->outputFloat[1] = (float) temp->m_loc.m_long;
		//p->outputFloat[2] = (float) temp->m_loc.m_alt;

		// For debugging, print out what we received.
		simAddStatusbarMessage(sstm.str().c_str());
	}

	simLockInterface(0);
}

// Registers the Lua simExtMadaraQuadrotorControlGetNewCommand command.
void registerMadaraQuadrotorControlGetNewCmdLuaCallback()
{
	// Register the simExtGetPositionInBridge function.
	simRegisterCustomLuaFunction("simExtMadaraQuadrotorControlGetNewCmd",
		"cmd, lat, long, alt = "
		"simExtMadaraQuadrotorControlGetNewCmd(int droneId)",
		g_getNewCmdInArgs, simExtMadaraQuadrotorControlGetNewCmd);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function used by all the "isXXCommand" functions to check a certain given command.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define the LUA function input parameters.
static int g_isCmdInArgs[] = {1, sim_lua_arg_string};

// The common function called by the callbacks.
void isGivenCommand(SLuaCallBack* p, const char* commandToTest)
{
	simLockInterface(1);

	// Check we have to correct parameters.
	bool paramsOk = checkInputArguments(p, g_isCmdInArgs, "isGivenCommand");
	if(paramsOk)
	{ 
		// The only param we received from Lua is a string to test for a certain command.
		char* receivedCommand =  p->inputChar;

		// Check if it is the command we are checking for.
		bool isExpectedCommand = (strcmp(commandToTest, receivedCommand) == 0);


		// Prepare the output description, with nil types by default.
		int numberOfOutputs = 1;
		setupDefaultOutput(p, numberOfOutputs);

		// Now we prepare the return value.
		p->outputArgTypeAndSize[2*0+0] = sim_lua_arg_bool;
		p->outputBool = (simBool*)simCreateBuffer(sizeof(simBool));
		p->outputBool[0] = isExpectedCommand;
	}

	simLockInterface(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// isGoToCmd
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// The actual callback function.
void simExtMadaraQuadrotorControlIsGoToCmd(SLuaCallBack* p)
{
	isGivenCommand(p, MO_MOVE_TO_GPS_CMD);
}

// Register test of isGoToCmd.
void registerMadaraQuadrotorControlIsGoToCmdLuaCallback()
{
	// Register the simExtGetPositionInBridge function.
	simRegisterCustomLuaFunction("simExtMadaraQuadrotorControlIsGoToCmd",
		"isGoTo = simExtMadaraQuadrotorControlIsGoToCmd(string cmd)",
		g_isCmdInArgs, simExtMadaraQuadrotorControlIsGoToCmd);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// isGoToCmdAlt.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// The actual callback function.
void simExtMadaraQuadrotorControlIsGoToAltCmd(SLuaCallBack* p)
{
	isGivenCommand(p, MO_MOVE_TO_ALTITUDE_CMD);
}

// Register test of isGoToAltCmd.
void registerMadaraQuadrotorControlIsGoToAltCmdLuaCallback()
{
	// Register the simExtGetPositionInBridge function.
	simRegisterCustomLuaFunction("simExtMadaraQuadrotorControlIsGoToAltCmd",
		"isGoTo = simExtMadaraQuadrotorControlIsGoToAltCmd(string cmd)",
		g_isCmdInArgs, simExtMadaraQuadrotorControlIsGoToAltCmd);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// isOffCmd.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// The actual callback function.
void simExtMadaraQuadrotorControlIsLandCmd(SLuaCallBack* p)
{
	isGivenCommand(p, MO_LAND_CMD);
}

// Register test of isOffCmd.
void registerMadaraQuadrotorControlIsLandCmdLuaCallback()
{
	// Register the simExtGetPositionInBridge function.
	simRegisterCustomLuaFunction("simExtMadaraQuadrotorControlIsLandCmd",
		"isLand = simExtMadaraQuadrotorControlIsLandCmd(string cmd)",
		g_isCmdInArgs, simExtMadaraQuadrotorControlIsLandCmd);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// isOnCmd
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// The actual callback function.
void simExtMadaraQuadrotorControlIsTakeoffCmd(SLuaCallBack* p)
{
	isGivenCommand(p, MO_TAKEOFF_CMD);
}

// Register test of isOnCmd
void registerMadaraQuadrotorControlIsTakeoffCmdLuaCallback()
{
	// Register the simExtGetPositionInBridge function.
	simRegisterCustomLuaFunction("simExtMadaraQuadrotorControlIsTakeoffCmd",
		"isTakeoff = simExtMadaraQuadrotorControlIsTakeoffCmd(string cmd)",
		g_isCmdInArgs, simExtMadaraQuadrotorControlIsTakeoffCmd);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraQuadrotorControlCleanup command.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define the LUA function input parameters.
static int g_cleanupInArgs[] = {0};

// The actual callback function.
void simExtMadaraQuadrotorControlCleanup(SLuaCallBack* /*p*/)
{
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

// Registers the Lua simExtMadaraQuadrotorControlCleanup command.
void registerMadaraQuadrotorControlCleanupLuaCallback()
{
	// Register the simExtGetPositionInBridge function.
	simRegisterCustomLuaFunction("simExtMadaraQuadrotorControlCleanup",
		"simExtMadaraQuadrotorControlCleanup()",
		g_cleanupInArgs, simExtMadaraQuadrotorControlCleanup);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers all Lua extensions defined in this module.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerAllLuaExtensions()
{
	registerMadaraQuadrotorControlSetupLuaCallback();
	registerMadaraQuadrotorControlCleanupLuaCallback();
  
	registerMadaraQuadrotorControlUpdateStatusLuaCallback();
	registerMadaraQuadrotorControlGetNewCmdLuaCallback();

	registerMadaraQuadrotorControlIsGoToCmdLuaCallback();
	registerMadaraQuadrotorControlIsGoToAltCmdLuaCallback();
	registerMadaraQuadrotorControlIsLandCmdLuaCallback();
	registerMadaraQuadrotorControlIsTakeoffCmdLuaCallback();
}