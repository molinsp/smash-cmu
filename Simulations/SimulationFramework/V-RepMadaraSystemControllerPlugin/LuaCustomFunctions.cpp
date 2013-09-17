/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#include "MadaraSystemController.h"
#include "LuaFunctionRegistration.h"
#include "LuaExtensionsUtils.h"
#include "utilities/string_utils.h"
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
        std::string sourceNorthWestLat(p->inputChar);
        std::string sourceNorthWestLong(p->inputChar+sourceNorthWestLat.length()+1);
        std::string sourceSouthEastLat(p->inputChar+sourceNorthWestLat.length()+sourceNorthWestLong.length()+2);
        std::string sourceSouthEastLong(p->inputChar+sourceNorthWestLat.length()+sourceNorthWestLong.length()+sourceSouthEastLat.length()+3);

        int sinkPosStart = sourceNorthWestLong.length()+sourceNorthWestLat.length()+sourceSouthEastLong.length()+sourceSouthEastLat.length()+4;
        std::string sinkNorthWestLat(p->inputChar+sinkPosStart);
        std::string sinkNorthWestLong(p->inputChar+sinkPosStart+sinkNorthWestLat.length()+1);
        std::string sinkSouthEastLat(p->inputChar+sinkPosStart+sinkNorthWestLat.length()+sinkNorthWestLong.length()+2);
        std::string sinkSouthEastLong(p->inputChar+sinkPosStart+sinkNorthWestLat.length()+sinkNorthWestLong.length()+sinkSouthEastLat.length()+3);

        // Create positions based on the received parameters, turning the strings into doubles.
        Position startNorthWest;
        Position startSouthEast;
        startNorthWest.longitude = atof(sourceNorthWestLong.c_str());
        startNorthWest.latitude = atof(sourceNorthWestLat.c_str());
        startSouthEast.longitude = atof(sourceSouthEastLong.c_str());
        startSouthEast.latitude = atof(sourceSouthEastLat.c_str());
        Region startRegion(startNorthWest, startSouthEast);

        Position endNorthWest;
        Position endSouthEast;
        endNorthWest.longitude = atof(sinkNorthWestLong.c_str());
        endNorthWest.latitude = atof(sinkNorthWestLat.c_str());
        endSouthEast.longitude = atof(sinkSouthEastLong.c_str());
        endSouthEast.latitude = atof(sinkSouthEastLat.c_str());
        Region endRegion(endNorthWest, endSouthEast);

        // For debugging, print out what we received.
        std::stringstream sstm; 
        sstm << "Values received inside simExtMadaraSystemControllerBridgeRequest function: bridgeId:" << bridgeId << ", " << std::setprecision(10)
            << "(lat, lon): "
            << " (" << startRegion.northWest.latitude << "," << startRegion.northWest.longitude << ")"
            << " (" << startRegion.southEast.latitude << "," << startRegion.southEast.longitude << ")"
            << " (" << endRegion.northWest.latitude << "," << endRegion.northWest.longitude << ")"
            << " (" << endRegion.southEast.latitude << "," << endRegion.southEast.longitude << ")"
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
                                                                 "float sourceNorthWestLong,"
                                                                 "float sourceNorthWestLat,"
                                                                 "float sourceBottomrightX,"
                                                                 "float sourceBottomrightY,"
                                                                 "float sinkNorthWestLong,"
                                                                 "float sinkNorthWestLat,"
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
        madaraController->updateGeneralParameters(totalNumberOfDrones);
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
                    sim_lua_arg_int,                         // Area ID.
                    sim_lua_arg_string,                      // The X position of the top left corner.
                    sim_lua_arg_string,                      // The Y position of the top left corner.
                    sim_lua_arg_string,                      // The X position of the bottom right corner.
                    sim_lua_arg_string                       // The Y position of the bottom right corner.
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
        std::string northWestLat(p->inputChar);
        std::string northWestLong(p->inputChar+northWestLat.length()+1);
        std::string southEastLat(p->inputChar+northWestLat.length()+northWestLong.length()+2);
        std::string southEastLong(p->inputChar+northWestLat.length()+northWestLong.length()+southEastLat.length()+3);

        // Create a region based on the received parameters, turning the strings into doubles.
        Position northWest;
        Position southEast;
        northWest.longitude = atof(northWestLong.c_str());
        northWest.latitude = atof(northWestLat.c_str());
        southEast.longitude = atof(southEastLong.c_str());
        southEast.latitude = atof(southEastLat.c_str());
        Region searchAreaRegion(northWest, southEast);

        // For debugging, print out what we received.
        char message[500];
        sprintf(message, "In SetupSearchArea: %d; NW(lat,long) = %.20f,%.20f, SE(lat,long) = %.20f,%.20f \n", 
                searchAreaId,   searchAreaRegion.northWest.latitude, searchAreaRegion.northWest.longitude, 
                                searchAreaRegion.southEast.latitude, searchAreaRegion.southEast.longitude);
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
                                                                  " float lat1, float long1,"
                                                                  " float lat2, float long2)",   // A tooltip to be shown to help the user know how to call it.
                                 g_setupSearchAreaInArgs,                                                    // The argument types.
                                 simExtMadaraSystemControllerSetupSearchArea);                        // The C function that will be called by the Lua function.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraSystemControllerAreaCoverageRequest command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define the LUA function input parameters.
static int g_areaCoverageRequestInArgs[] = {5, 
                                    sim_lua_arg_string,                 // Drone IDs, separated by a comma.
                                    sim_lua_arg_int,                    // Area ID.
                                    sim_lua_arg_string,                 // Area coverage algorithm to implement.
                                    sim_lua_arg_int,                    // Wait flag.
                                    sim_lua_arg_string                  // The human detection algorithm to use.
                };

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
        int waitFlag = p->inputInt[1];

        // Get the values from the concatenated string with all of them.		
		// First the concatenated list of drones.
		std::string droneIdsString = p->inputChar;

        // Now the search algorithm.
        std::string searchAlgorithm(p->inputChar + droneIdsString.length() + 1);

        // Now the human detection algorithm.
        std::string humanDetectionAlgorithm(p->inputChar + droneIdsString.length() + 1 + searchAlgorithm.length() + 1);

        // For debugging, print out what we received.
        std::stringstream sstm; 
        sstm << "Values received inside simExtMadaraSystemControllerAreaCoverageRequest(" <<
            "droneIds = " << droneIdsString << ", areaId= " << searchAreaId <<
            ", search algo: \"" << searchAlgorithm << "\", human algo: \"" << humanDetectionAlgorithm << "\", waitFlag: " << waitFlag << ")" << std::endl;
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
        madaraController->requestAreaCoverage(droneIdList, searchAreaId, searchAlgorithm, waitFlag, humanDetectionAlgorithm);
    }

    simLockInterface(0);
}

// Registers the Lua simExtMadaraSystemControllerSearchRequest command.
void registerMadaraSystemControllerAreaCoverageRequestLuaCallback()
{
    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraSystemControllerAreaCoverageRequest",                       // The Lua function name.
                                 "simExtMadaraSystemControllerAreaCoverageRequest(int droneId, "
                                            "areaId, searchAlgorithm, waitFlag, humanDetectionAlgorithm)",  // A tooltip to be shown to help the user know how to call it.
                                 g_areaCoverageRequestInArgs,                                             // The argument types.
                                 simExtMadaraSystemControllerAreaCoverageRequest);                        // The C function that will be called by the Lua function.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraSystemControllerGetCurrentLocation command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define the LUA function input parameters.
static int g_getCurrLocationInArgs[] = {0,
                };

// The actual callback function.
void simExtMadaraSystemControllerGetCurrentLocation(SLuaCallBack* p)
{
    //WIN_AFX_MANAGE_STATE;
    simLockInterface(1);

    // Tell the controller to actually set this drone to search this area.
    std::vector<SMASH::Utilities::Position> locations = madaraController->getCurrentLocations();

    // We want to return two tables, one with the lats and another with the longs.
    int numTables = 2;
   	int tableSize = locations.size();
    setupOutputsToTables(p, numTables, tableSize);

    //p->output

    simLockInterface(0);
}

// Registers the Lua simExtMadaraSystemControllerGetCurrentLocation command.
void registerMadaraSystemControllerGetCurrentLocationLuaCallback()
{
    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraSystemControllerGetCurrentLocation",                       // The Lua function name.
                                 "lat, long = simExtMadaraSystemControllerGetCurrentLocation()",         // A tooltip to be shown to help the user know how to call it.
                                 g_getCurrLocationInArgs,                                                // The argument types.
                                 simExtMadaraSystemControllerGetCurrentLocation);                        // The C function that will be called by the Lua function.
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

    registerMadaraSystemControllerGetCurrentLocationLuaCallback();
    //registerMadaraSystemControllerGetThermalsLuaCallback();
}
