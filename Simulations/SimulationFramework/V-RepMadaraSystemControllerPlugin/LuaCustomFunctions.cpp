/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#include "MadaraSystemController.h"
#include "LuaFunctionRegistration.h"
#include "LuaExtensionsUtils.h"
#include "v_repLib.h"

#include <string>
#include <sstream>
#include <vector>

using namespace SMASH::Utilities;

// The controller used to manage the Madara stuff.
MadaraController* madaraController;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraSystemControllerSetup command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define the LUA function input parameters.
static int g_setupInArgs[] = {5, 
                    sim_lua_arg_int,                        // My ID.
                    sim_lua_arg_float,                        // Radio range.
                    sim_lua_arg_float,                        // Min altitude.
                    sim_lua_arg_float,                        // Line width.
                    sim_lua_arg_float,                        // Height diff.
                };

// The actual function.
void simExtMadaraSystemControllerSetup(SLuaCallBack* p)
{
    //WIN_AFX_MANAGE_STATE;
    simLockInterface(1);

    // Continue forward calling the external functions only if we have all parameters ok.
    bool paramsOk = checkInputArguments(p, g_setupInArgs, "simExtMadaraSystemControllerSetup");
    if(paramsOk)
    { 
        // Get the simple input values.
        int myId = p->inputInt[0];
        double commRange = p->inputFloat[0];
        double minAltitude = p->inputFloat[1];
        double lineWidth = p->inputFloat[2];
        double heightDiff = p->inputFloat[3];

        // For debugging, print out what we received.
        std::stringstream sstm; 
        sstm << "Values received inside simExtMadaraSystemControllerSetup function: myId: " << myId << ", commRange: " << commRange << "; minAltitude: " << minAltitude;
        sstm << "; lineWidth: " << lineWidth << "; heightDiff: " << heightDiff << std::endl;
        std::string message = sstm.str();
        simAddStatusbarMessage(message.c_str());

        // Simply create the madara controller.
        madaraController = new MadaraController(myId, commRange, minAltitude, lineWidth, heightDiff);
    }

    simLockInterface(0);
}

// Registers the Lua simExtMadaraSystemControllerSetup command.
void registerMadaraSystemControllerSetupLuaCallback()
{
    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraSystemControllerSetup",                         // The Lua function name.
                                 "simExtMadaraSystemControllerSetup(int myId, "               // A tooltip to be shown to help the user know how to call it.
                                                         "number radioRange,"
                                                         "number minAltitude,"
                                                         "number lineWidth,"
                                                         "number heightDiff)",
                                 g_setupInArgs,                                                  // The argument types.
                                 simExtMadaraSystemControllerSetup);                          // The C function that will be called by the Lua function.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraSystemControllerBridgeRequest command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define the LUA function input parameters.
static int g_bridgeRequestInArgs[] = {9, 
                    sim_lua_arg_int,                    // Bridge ID.
                    sim_lua_arg_string,                    // The X position of the top left corner of the source.
                    sim_lua_arg_string,                    // The Y position of the top left corner of the source.
                    sim_lua_arg_string,                    // The X position of the bottom right corner of the source.
                    sim_lua_arg_string,                    // The Y position of the bottom right corner of the source.
                    sim_lua_arg_string,                    // The X position of the top left corner of the sink.
                    sim_lua_arg_string,                    // The Y position of the top left corner of the sink.
                    sim_lua_arg_string,                    // The X position of the bottom right corner of the sink.
                    sim_lua_arg_string,                    // The Y position of the bottom right corner of the sink.

                };

// The actual callback function.
void simExtMadaraSystemControllerBridgeRequest(SLuaCallBack* p)
{
    //WIN_AFX_MANAGE_STATE;
    simLockInterface(1);

    // Continue forward calling the external functions only if we have all parameters ok.
    bool paramsOk = checkInputArguments(p, g_bridgeRequestInArgs, "simExtMadaraSystemControllerBridgeRequest");
    if(paramsOk)
    { 
        // Get the simple input values.
        int bridgeId = p->inputInt[0];

        // Get the values from the concatenated string with all of them.
        // NOTE: these parameters are sent as strings since there seems to be problems with large double numbers between Lua and C++ in Vrep.
        std::string sourceTopLeftX(p->inputChar);
        std::string sourceTopLeftY(p->inputChar+sourceTopLeftX.length()+1);
        std::string sourceBotRightX(p->inputChar+sourceTopLeftX.length()+sourceTopLeftY.length()+2);
        std::string sourceBotRightY(p->inputChar+sourceTopLeftX.length()+sourceTopLeftY.length()+sourceBotRightX.length()+3);

        int sinkPosStart = sourceTopLeftX.length()+sourceTopLeftY.length()+sourceBotRightX.length()+sourceBotRightY.length()+4;
        std::string sinkTopLeftX(p->inputChar+sinkPosStart);
        std::string sinkTopLeftY(p->inputChar+sinkPosStart+sinkTopLeftX.length()+1);
        std::string sinkBotRightX(p->inputChar+sinkPosStart+sinkTopLeftX.length()+sinkTopLeftY.length()+2);
        std::string sinkBotRightY(p->inputChar+sinkPosStart+sinkTopLeftX.length()+sinkTopLeftY.length()+sinkBotRightX.length()+3);

        // Create positions based on the received parameters, turning the strings into doubles.
        Region startRegion;
        startRegion.topLeftCorner.x = atof(sourceTopLeftX.c_str());
        startRegion.topLeftCorner.y = atof(sourceTopLeftY.c_str());
        startRegion.bottomRightCorner.x = atof(sourceBotRightX.c_str());
        startRegion.bottomRightCorner.y = atof(sourceBotRightY.c_str());

        Region endRegion;
        endRegion.topLeftCorner.x = atof(sinkTopLeftX.c_str());
        endRegion.topLeftCorner.y = atof(sinkTopLeftY.c_str());
        endRegion.bottomRightCorner.x = atof(sinkBotRightX.c_str());
        endRegion.bottomRightCorner.y = atof(sinkBotRightY.c_str());

        // For debugging, print out what we received.
        std::stringstream sstm; 
        sstm << "Values received inside simExtMadaraSystemControllerBridgeRequest function: bridgeId:" << bridgeId << ", " << std::setprecision(10)
            << " (" << startRegion.topLeftCorner.x << "," << startRegion.topLeftCorner.y << ")"
            << " (" << startRegion.bottomRightCorner.x << "," << startRegion.bottomRightCorner.y << ")"
            << " (" << endRegion.topLeftCorner.x << "," << endRegion.topLeftCorner.y << ")"
            << " (" << endRegion.bottomRightCorner.x << "," << endRegion.bottomRightCorner.y << ")"
            << std::endl;
        simAddStatusbarMessage(sstm.str().c_str());

        // Make the controller set up the bridge request through the knowledge base.
        madaraController->setupBridgeRequest(bridgeId, startRegion, endRegion);
    }

    simLockInterface(0);
}

// Registers the Lua simExtMadaraSystemControllerBridgeRequest command.
void registerMadaraSystemControllerBridgeRequestLuaCallback()
{
    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraSystemControllerBridgeRequest",                         // The Lua function name.
                                 "simExtMadaraSystemControllerBridgeRequest(int bridgeId, "          // A tooltip to be shown to help the user know how to call it.
                                                                 "float sourceTopleftX,"
                                                                 "float sourceTopleftY,"
                                                                 "float sourceBottomrightX,"
                                                                 "float sourceBottomrightY,"
                                                                 "float sinkTopleftX,"
                                                                 "float sinkTopleftY,"
                                                                 "float sinkBottomrightX,"
                                                                 "float sinkBottomrightY)",
                                 g_bridgeRequestInArgs,                                                    // The argument types.
                                 simExtMadaraSystemControllerBridgeRequest);                          // The C function that will be called by the Lua function.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraSystemControllerUpdateStatus command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define the LUA function input parameters.
static int g_updateStatusInArgs[] = {1, sim_lua_arg_int};                      // The number of drones.

// The actual function callback.
void simExtMadaraSystemControllerUpdateStatus(SLuaCallBack* p)
{
    simLockInterface(1);

    // Continue forward calling the external functions only if we have all parameters ok.
    bool paramsOk = checkInputArguments(p, g_updateStatusInArgs, "simExtMadaraSystemControllerUpdateStatus");
    if(paramsOk)
    { 
        // Get the number of drones.
        int totalNumberOfDrones = p->inputInt[0];

        // Propagate the status information through the network.
        madaraController->updateNetworkStatus(totalNumberOfDrones);
    }

    simLockInterface(0);
}

// Registers the Lua simExtMadaraSystemControllerUpdateStatus command.
void registerMadaraSystemControllerUpdateStatusLuaCallback()
{
    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraSystemControllerUpdateStatus",                  // The Lua function name.
                                 "simExtMadaraSystemControllerUpdateStatus"                   // A tooltip to be shown to help the user know how to call it.
                                                          "(int numberOfDrones)",             
                                 g_updateStatusInArgs,                                        // The argument types.
                                 simExtMadaraSystemControllerUpdateStatus);                   // The C function that will be called by the Lua function.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraSystemControllerSetupSearchArea command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define the LUA function input parameters.
static int g_setupSearchAreaInArgs[] = {5, 
                    sim_lua_arg_int,                      // Area ID.
                    sim_lua_arg_string,                      // The X position of the top left corner.
                    sim_lua_arg_string,                      // The Y position of the top left corner.
                    sim_lua_arg_string,                      // The X position of the bottom right corner.
                    sim_lua_arg_string                      // The Y position of the bottom right corner.
                };

// The actual callback function.
void simExtMadaraSystemControllerSetupSearchArea(SLuaCallBack* p)
{
    simLockInterface(1);

    // Continue forward calling the external functions only if we have all parameters ok.
    bool paramsOk = checkInputArguments(p, g_setupSearchAreaInArgs, "simExtMadaraSystemControllerSetupSearchArea");
    if(paramsOk)
    { 
        // Get the simple input values.
        int searchAreaId = p->inputInt[0];

        // Get the values from the concatenated string with all of them.
        // NOTE: these parameters are sent as strings since there seems to be problems with large double numbers between Lua and C++ in Vrep.
        std::string topLeftX(p->inputChar);
        std::string topLeftY(p->inputChar+topLeftX.length()+1);
        std::string botRightX(p->inputChar+topLeftX.length()+topLeftY.length()+2);
        std::string botRightY(p->inputChar+topLeftX.length()+topLeftY.length()+botRightX.length()+3);

        // Create a region based on the received parameters, turning the strings into doubles.
        Region searchAreaRegion;
        searchAreaRegion.topLeftCorner.x = atof(topLeftX.c_str());
        searchAreaRegion.topLeftCorner.y = atof(topLeftY.c_str());
        searchAreaRegion.bottomRightCorner.x = atof(botRightX.c_str());
        searchAreaRegion.bottomRightCorner.y = atof(botRightY.c_str());

        // For debugging, print out what we received.
        char message[500];
        sprintf(message, "In SetupSearchArea: %d; (x,y) = %.20f,%.20f, x2,y2 = %.20f,%.20f \n", 
                searchAreaId,    searchAreaRegion.topLeftCorner.x,searchAreaRegion.topLeftCorner.y, 
                                searchAreaRegion.bottomRightCorner.x, searchAreaRegion.bottomRightCorner.y);
        simAddStatusbarMessage(message);

        // Tell the controller to actually set up this area.
        madaraController->setNewSearchArea(0, searchAreaRegion);
    }

    simLockInterface(0);
}

// Registers the Lua simExtMadaraSystemControllerSetupSearchArea command.
void registerMadaraSystemControllerSetupSearchAreaLuaCallback()
{
    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraSystemControllerSetupSearchArea",                       // The Lua function name.
                                 "simExtMadaraSystemControllerSetupSearchArea(int areaId,"
                                                                  " float x1, float y1,"
                                                                  " float x2, float y2)",   // A tooltip to be shown to help the user know how to call it.
                                 g_setupSearchAreaInArgs,                                                    // The argument types.
                                 simExtMadaraSystemControllerSetupSearchArea);                        // The C function that will be called by the Lua function.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraSystemControllerAreaCoverageRequest command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define the LUA function input parameters.
static int g_areaCoverageRequestInArgs[] = {3, 
                                    sim_lua_arg_string,                 // Drone IDs, separated by a comma.
                                    sim_lua_arg_int,                    // Area ID.
                                    sim_lua_arg_string                  // Area coverage algorithm to implement.
                };

std::vector<std::string> &stringSplit(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> stringSplit(const std::string &s, char delim) {
    std::vector<std::string> elems;
    stringSplit(s, delim, elems);
    return elems;
}

// The actual callback function.
void simExtMadaraSystemControllerAreaCoverageRequest(SLuaCallBack* p)
{
    //WIN_AFX_MANAGE_STATE;
    simLockInterface(1);

    // Continue forward calling the external functions only if we have all parameters ok.
    bool paramsOk = checkInputArguments(p, g_areaCoverageRequestInArgs, "simExtMadaraSystemControllerAreaCoverageRequest");
    if(paramsOk)
    { 
        // Get the simple input values.
        int searchAreaId = p->inputInt[0];

        // Get the values from the concatenated string with all of them.		
		// First the concatenated list of drones.
		std::string droneIdsString = p->inputChar;

        // Now the algorithm.
        std::string algorithm(p->inputChar+droneIdsString.length()+1);

        // For debugging, print out what we received.
        std::stringstream sstm; 
        sstm << "Values received inside simExtMadaraSystemControllerAreaCoverageRequest(" <<
            "droneIds = " << droneIdsString << ", areaId= " << searchAreaId <<
            ", algorithm: \"" << algorithm << "\")" << std::endl;
        simAddStatusbarMessage(sstm.str().c_str());

		// Parse the drone ids.
		std::vector<std::string> droneIdParsedList = stringSplit(droneIdsString, ',');
		
		// Turn the drone ids into ints.
		std::vector<int> droneIdList = std::vector<int>(droneIdParsedList.size());
		for(unsigned int i=0; i<droneIdParsedList.size(); i++)
		{
			droneIdList[i] = atoi(droneIdParsedList[i].c_str());
		}

        // Tell the controller to actually set this drone to search this area.
        madaraController->requestAreaCoverage(droneIdList, searchAreaId, algorithm);
    }

    simLockInterface(0);
}

// Registers the Lua simExtMadaraSystemControllerSearchRequest command.
void registerMadaraSystemControllerAreaCoverageRequestLuaCallback()
{
    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraSystemControllerAreaCoverageRequest",                       // The Lua function name.
                                 "simExtMadaraSystemControllerAreaCoverageRequest(int droneId, areaId, algorithm)",  // A tooltip to be shown to help the user know how to call it.
                                 g_areaCoverageRequestInArgs,                                             // The argument types.
                                 simExtMadaraSystemControllerAreaCoverageRequest);                        // The C function that will be called by the Lua function.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraSystemControllerCleanup command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define the LUA function input parameters.
static int g_cleanUpInArgs[] = {0};

// The actual callback function.
void simExtMadaraSystemControllerCleanup(SLuaCallBack* p)
{
    simLockInterface(1);

    // Simply cleanup the madara controller.
    simAddStatusbarMessage("simExtMadaraSystemControllerCleanup: cleaning up");
    if(madaraController != NULL)
    {
        delete madaraController;
        madaraController = NULL;
    }

    simLockInterface(0);
}

// Registers the Lua simExtMadaraSystemControllerCleanup command.
void registerMadaraSystemControllerCleanupLuaCallback()
{
    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraSystemControllerCleanup",                         // The Lua function name.
                                 "simExtMadaraSystemControllerCleanup()",                       // A tooltip to be shown to help the user know how to call it.
                                 g_cleanUpInArgs,                                               // The argument types.
                                 simExtMadaraSystemControllerCleanup);                          // The C function that will be called by the Lua function.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers all Lua extensions defined in this module.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerAllLuaExtensions()
{
    registerMadaraSystemControllerSetupLuaCallback();
    registerMadaraSystemControllerCleanupLuaCallback();

    registerMadaraSystemControllerUpdateStatusLuaCallback();
    registerMadaraSystemControllerBridgeRequestLuaCallback();
    registerMadaraSystemControllerSetupSearchAreaLuaCallback();
    registerMadaraSystemControllerAreaCoverageRequestLuaCallback();
}
