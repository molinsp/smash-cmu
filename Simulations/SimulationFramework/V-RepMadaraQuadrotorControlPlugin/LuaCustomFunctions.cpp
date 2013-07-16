/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

#include "MadaraQuadrotorControl.h"
#include "LuaFunctionRegistration.h"
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
void simExtMadaraQuadrotorControlSetup(SLuaCallBack* p)
{
	simLockInterface(1);

	// Check we have to correct amount of parameters.
	if (p->inputArgCount != 1)
	{
		simSetLastError("simExtMadaraQuadrotorControlSetup",
			"Not enough arguments given.");
	}
	// Check we have the correct type of arguments.
	else if (p->inputArgTypeAndSize[0*2+0] != sim_lua_arg_int)
	{
		simSetLastError("simExtMadaraQuadrotorControlUpdateStatus",
			"The id of the drone is not an int.");
	}
	else
	{
		int droneId = p->inputInt[0];
		if(control != NULL)
			delete control;
		control = new MadaraQuadrotorControl(droneId);
	}

	simLockInterface(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers the Lua simExtMadaraQuadrotorControlSetup command.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerMadaraQuadrotorControlSetupLuaCallback()
{
	// Define the LUA function input parameters.
	int inArgs[] = {1, sim_lua_arg_int};    // the id of the drone

	// Register the simExtGetPositionInBridge function.
	simRegisterCustomLuaFunction("simExtMadaraQuadrotorControlSetup",
		"simExtMadaraQuadrotorControlSetup(int droneId)",
		inArgs, simExtMadaraQuadrotorControlSetup);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraQuadrotorControlUpdateStatus command.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
	else if(p->inputArgTypeAndSize[1*2+0] != sim_lua_arg_string)
	{
		simSetLastError("simExtMadaraQuadrotorControlUpdateStatus",
			"The latitude of the drone is not a float.");
	}
	else if(p->inputArgTypeAndSize[2*2+0] != sim_lua_arg_string)
	{
		simSetLastError("simExtMadaraQuadrotorControlUpdateStatus",
			"The longitude of the drone is not a float.");
	}
	else if(p->inputArgTypeAndSize[3*2+0] != sim_lua_arg_string)
	{
		simSetLastError("simExtMadaraQuadrotorControlUpdateStatus",
			"The altitude of the drone is not a float.");
	}
	else // params checked out
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers the Lua simExtMadaraQuadrotorControlUpdateStatus command.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerMadaraQuadrotorControlUpdateStatusLuaCallback()
{
	// Define the LUA function input parameters.
	int inArgs[] = {4, sim_lua_arg_int,     // the id of the drone
		sim_lua_arg_string,   // The lat position of the drone
		sim_lua_arg_string,   // The long position of the drone
		sim_lua_arg_string};  // The alt position of the drone

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
void simExtMadaraQuadrotorControlGetNewCmd(SLuaCallBack* p)
{
	simLockInterface(1);

	MadaraQuadrotorControl::Command *newCommand = NULL;

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
		// We check if a new command has arrived for us.
		int droneId = p->inputInt[0];
		newCommand = control->getNewCommand(droneId);
	}

	// Now we prepare the return value(s).
	p->outputArgCount = 4;			// Always return 4 arguments, even if some are nil.
	p->outputArgTypeAndSize = (simInt*)simCreateBuffer(p->outputArgCount * (2 * sizeof(simInt)));

	// By default set all results to nil.
	p->outputArgTypeAndSize[2*0+0] = sim_lua_arg_nil;
	p->outputArgTypeAndSize[2*0+1] = 1;

	p->outputArgTypeAndSize[2*1+0] = sim_lua_arg_nil;
	p->outputArgTypeAndSize[2*1+1] = 1;

	p->outputArgTypeAndSize[2*2+0] = sim_lua_arg_nil;
	p->outputArgTypeAndSize[2*2+1] = 1;

	p->outputArgTypeAndSize[2*3+0] = sim_lua_arg_nil;
	p->outputArgTypeAndSize[2*3+1] = 1;

	// Set the actual return values depending on whether we found a position or not.
	if(newCommand != NULL)
	{
		// For debugging, to print out what we received.
		std::stringstream sstm; 

		// All commands will have at least the command name, though they may have different parameters.
		p->outputArgTypeAndSize[2*0+0] = sim_lua_arg_string; // cmd
		p->outputArgTypeAndSize[2*0+1] = 1;

		// First case: MOVE_TO_GPS
		if(strcmp(MO_MOVE_TO_GPS_CMD, newCommand->m_command.c_str()) == 0)
		{
			// For debugging, print out what we received.
			sstm << "Values received inside simExtMadaraQuadrotorControlGetNewCmd function: command:" << newCommand->m_command << ", "
				<< " (" << std::setprecision(10) << newCommand->m_loc.m_lat << "," << newCommand->m_loc.m_long << ")"
				<< std::endl;

			// Move to gps has lat and long parameters, which we will have to return.
			p->outputArgTypeAndSize[2*1+0] = sim_lua_arg_string; // lat
			p->outputArgTypeAndSize[2*1+1] = 1;
			p->outputArgTypeAndSize[2*2+0] = sim_lua_arg_string; // long
			p->outputArgTypeAndSize[2*2+1] = 1;

			// Turn floats into strings to avoid losing precision when transfering back to Lua.
			std::string lat(NUM_TO_STR(newCommand->m_loc.m_lat));
			std::string lon(NUM_TO_STR(newCommand->m_loc.m_long));

			// Create a string buffer to return all return values in it.
			p->outputChar = (simChar*) simCreateBuffer(
				(newCommand->m_command.length() + 1 + lat.length()+1 + lon.length()+1) * sizeof(simChar));

			unsigned int pos = 0;
			for(unsigned int i = 0; i < newCommand->m_command.length(); ++i)
				p->outputChar[pos++] = newCommand->m_command.at(i);
			p->outputChar[pos++] = '\0'; // null terminate

			for(unsigned int i = 0; i < lat.length(); ++i)
				p->outputChar[pos++] = lat.at(i);
			p->outputChar[pos++] = '\0'; // null terminate

			for(unsigned int i = 0; i < lon.length(); ++i)
				p->outputChar[pos++] = lon.at(i);
			p->outputChar[pos++] = '\0'; // null terminate
		}
		else if(strcmp(MO_MOVE_TO_ALTITUDE_CMD, newCommand->m_command.c_str()) == 0)
		{
			// For debugging, print out what we received.
			sstm << "Values received inside simExtMadaraQuadrotorControlGetNewCmd function: command:" << newCommand->m_command << ", "
				<< " (" << std::setprecision(10) << newCommand->m_loc.m_alt << ")" << std::endl;

			// The only parameter other than the command is the altitude.
			p->outputArgTypeAndSize[2*1+0] = sim_lua_arg_string; // altitude
			p->outputArgTypeAndSize[2*1+1] = 1;

			// Turn floats into strings to avoid losing precision when transfering back to Lua.
			std::string alt(NUM_TO_STR(newCommand->m_loc.m_alt));

			// Create a string buffer to return all return values in it.
			p->outputChar = (simChar*) simCreateBuffer(
				(newCommand->m_command.length() + 1 + alt.length()+1) * sizeof(simChar));

			unsigned int pos = 0;
			for(unsigned int i = 0; i < newCommand->m_command.length(); ++i)
				p->outputChar[pos++] = newCommand->m_command.at(i);
			p->outputChar[pos++] = '\0'; // null terminate

			for(unsigned int i = 0; i < alt.length(); ++i)
				p->outputChar[pos++] = alt.at(i);
			p->outputChar[pos++] = '\0'; // null terminate
		}

		// copy x, y, z coords
		//p->outputFloat = (simFloat*) simCreateBuffer(3 * sizeof(simFloat));
		//p->outputFloat[0] = (float) temp->m_loc.m_lat;
		//p->outputFloat[1] = (float) temp->m_loc.m_long;
		//p->outputFloat[2] = (float) temp->m_loc.m_alt;

		// For debugging, print out what we received.
		std::string message = sstm.str();
		simAddStatusbarMessage(message.c_str());
	}

	simLockInterface(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers the Lua simExtMadaraQuadrotorControlGetNewCommand command.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function used by all the "isXXCommand" functions to check a certain given command.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void isGivenCommand(SLuaCallBack* p, const char* commandToTest)
{
	simLockInterface(1);

	// Check we have to correct amount of parameters.
	if (p->inputArgCount != 1)
	{ 
		simSetLastError("isGivenCommand",
			"Need command to test.");
	}
	// Check we have the correct type of arguments.
	else if(p->inputArgTypeAndSize[0*2+0] != sim_lua_arg_string)
	{
		simSetLastError("isGivenCommand",
			"Command parameter is not a string.");
	}
	else
	{ 
		// The only param we received from Lua is a string to test for a certain command.
		char* receivedCommand =  p->inputChar;

		// Check if it is the command we are checking for.
		bool isExpectedCommand = (strcmp(commandToTest, receivedCommand) == 0);

		// Now we prepare the return value.
		p->outputArgCount = 1;
		p->outputArgTypeAndSize = (simInt*)simCreateBuffer(
			p->outputArgCount * (2 * sizeof(simInt)));
		p->outputArgTypeAndSize[2*0+0] = sim_lua_arg_bool;
		p->outputArgTypeAndSize[2*0+1] = 1;
		p->outputBool = (simBool*)simCreateBuffer(sizeof(simBool));
		p->outputBool[0] = isExpectedCommand;
	}

	simLockInterface(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// isGoToCmd
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void simExtMadaraQuadrotorControlIsGoToCmd(SLuaCallBack* p)
{
	isGivenCommand(p, MO_MOVE_TO_GPS_CMD);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Register test of isGoToCmd
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerMadaraQuadrotorControlIsGoToCmdLuaCallback()
{
	// Define the LUA function input parameters.
	int inArgs[] = {1, sim_lua_arg_string};

	// Register the simExtGetPositionInBridge function.
	simRegisterCustomLuaFunction("simExtMadaraQuadrotorControlIsGoToCmd",
		"isGoTo = simExtMadaraQuadrotorControlIsGoToCmd(string cmd)",
		inArgs, simExtMadaraQuadrotorControlIsGoToCmd);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// isGoToCmdAlt
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void simExtMadaraQuadrotorControlIsGoToAltCmd(SLuaCallBack* p)
{
	isGivenCommand(p, MO_MOVE_TO_ALTITUDE_CMD);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Register test of isGoToAltCmd
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerMadaraQuadrotorControlIsGoToAltCmdLuaCallback()
{
	// Define the LUA function input parameters.
	int inArgs[] = {1, sim_lua_arg_string};

	// Register the simExtGetPositionInBridge function.
	simRegisterCustomLuaFunction("simExtMadaraQuadrotorControlIsGoToAltCmd",
		"isGoTo = simExtMadaraQuadrotorControlIsGoToAltCmd(string cmd)",
		inArgs, simExtMadaraQuadrotorControlIsGoToAltCmd);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// isOffCmd
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void simExtMadaraQuadrotorControlIsLandCmd(SLuaCallBack* p)
{
	isGivenCommand(p, MO_LAND_CMD);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Regiest test of isOffCmd
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerMadaraQuadrotorControlIsLandCmdLuaCallback()
{
	// Define the LUA function input parameters.
	int inArgs[] = {1, sim_lua_arg_string};

	// Register the simExtGetPositionInBridge function.
	simRegisterCustomLuaFunction("simExtMadaraQuadrotorControlIsLandCmd",
		"isLand = simExtMadaraQuadrotorControlIsLandCmd(string cmd)",
		inArgs, simExtMadaraQuadrotorControlIsLandCmd);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// isOnCmd
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void simExtMadaraQuadrotorControlIsTakeoffCmd(SLuaCallBack* p)
{
	isGivenCommand(p, MO_TAKEOFF_CMD);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Register test of isOnCmd
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerMadaraQuadrotorControlIsTakeoffCmdLuaCallback()
{
	// Define the LUA function input parameters.
	int inArgs[] = {1, sim_lua_arg_string};

	// Register the simExtGetPositionInBridge function.
	simRegisterCustomLuaFunction("simExtMadaraQuadrotorControlIsTakeoffCmd",
		"isTakeoff = simExtMadaraQuadrotorControlIsTakeoffCmd(string cmd)",
		inArgs, simExtMadaraQuadrotorControlIsTakeoffCmd);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraQuadrotorControlCleanup command.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers the Lua simExtMadaraQuadrotorControlCleanup command.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerMadaraQuadrotorControlCleanupLuaCallback()
{
	// Define the LUA function input parameters.
	int inArgs[] = {0};

	// Register the simExtGetPositionInBridge function.
	simRegisterCustomLuaFunction("simExtMadaraQuadrotorControlCleanup",
		"simExtMadaraQuadrotorControlCleanup()",
		inArgs, simExtMadaraQuadrotorControlCleanup);
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