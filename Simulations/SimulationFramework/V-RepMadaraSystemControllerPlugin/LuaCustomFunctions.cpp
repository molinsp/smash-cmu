/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#include "MadaraSystemController.h"
#include "LuaCustomFunctions.h"

using namespace SMASH::Utilities;

// The controller used to manage the Madara stuff.
MadaraController* madaraController;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers the Lua simExtMadaraSystemControllerSetup command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerMadaraSystemControllerSetupLuaCallback()
{
    // Define the LUA function input parameters.
    int inArgs[] = {3, sim_lua_arg_int,					      // My ID.
                       sim_lua_arg_float,					  // Radio range.
                       sim_lua_arg_float,					  // Min alti tude.
                   };

    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraSystemControllerSetup",                         // The Lua function name.
                                 "simExtMadaraSystemControllerSetup(int myId, "               // A tooltip to be shown to help the user know how to call it.
                                                         "number radioRange,"
                                                         "number minAltitude)",
                                 inArgs,                                           // The argument types.
                                 simExtMadaraSystemControllerSetup);                          // The C function that will be called by the Lua function.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraSystemControllerSetup command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void simExtMadaraSystemControllerSetup(SLuaCallBack* p)
{
    //WIN_AFX_MANAGE_STATE;
    simLockInterface(1);

    bool paramsOk = true;

    // Check we have to correct amount of parameters.
    if (p->inputArgCount != 3)
    { 
        simSetLastError("simExtMadaraSystemControllerSetup", "Not enough arguments.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[0*2+0] != sim_lua_arg_int )
    {
        simSetLastError("simExtMadaraSystemControllerSetup", "MyID parameter is not an int.");
        paramsOk = false;
    }

    if(p->inputArgTypeAndSize[1*2+0] != sim_lua_arg_float)
    {
        simSetLastError("simExtMadaraSystemControllerSetup", "RadioRange parameter is not a float.");
        paramsOk = false;
    }

    if(p->inputArgTypeAndSize[2*2+0] != sim_lua_arg_float)
    {
        simSetLastError("simExtMadaraSystemControllerSetup", "Min altitude parameter is not a float.");
        paramsOk = false;
    }

    // Continue forward calling the external functions only if we have all parameters ok.
    if(paramsOk)
    { 
        // Get the simple input values.
        int myId = p->inputInt[0];
        double commRange = p->inputFloat[0];
		double minAltitude = p->inputFloat[1];

        // For debugging, print out what we received.
        std::stringstream sstm; 
        sstm << "Values received inside simExtMadaraSystemControllerSetup function: myId:" << myId << ", commRange:" << commRange << "; minAltitude: " << minAltitude << std::endl;
        std::string message = sstm.str();
        simAddStatusbarMessage(message.c_str());

        // Simply create the madara controller.
        madaraController = new MadaraController(myId, commRange, minAltitude);
    }

    simLockInterface(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers the Lua simExtMadaraSystemControllerBridgeRequest command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerMadaraSystemControllerBridgeRequestLuaCallback()
{
    // Define the LUA function input parameters.
    int inArgs[] = {9, sim_lua_arg_int,					    // Bridge ID.
                       sim_lua_arg_string,					// The X position of the top left corner of the source.
                       sim_lua_arg_string,					// The Y position of the top left corner of the source.
                       sim_lua_arg_string,					// The X position of the bottom right corner of the source.
                       sim_lua_arg_string,					// The Y position of the bottom right corner of the source.
                       sim_lua_arg_string,					// The X position of the top left corner of the sink.
                       sim_lua_arg_string,					// The Y position of the top left corner of the sink.
                       sim_lua_arg_string,					// The X position of the bottom right corner of the sink.
                       sim_lua_arg_string,					// The Y position of the bottom right corner of the sink.

                   };

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
                                 inArgs,                                                    // The argument types.
                                 simExtMadaraSystemControllerBridgeRequest);                          // The C function that will be called by the Lua function.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraSystemControllerBridgeRequest command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void simExtMadaraSystemControllerBridgeRequest(SLuaCallBack* p)
{
    //WIN_AFX_MANAGE_STATE;
    simLockInterface(1);

    bool paramsOk = true;

    // Check we have to correct amount of parameters.
    if (p->inputArgCount != 9)
    { 
        simSetLastError("simExtMadaraSystemControllerBridgeRequest", "Not enough arguments.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[0*2+0] != sim_lua_arg_int )
    {
        simSetLastError("simExtMadaraSystemControllerBridgeRequest", "RequestId parameter is not an int.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[1*2+0] != sim_lua_arg_string )
    {
        simSetLastError("simExtMadaraSystemControllerBridgeRequest", "sourceTopleftX parameter is not an float.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[2*2+0] != sim_lua_arg_string )
    {
        simSetLastError("simExtMadaraSystemControllerBridgeRequest", "sourceTopleftY parameter is not an float.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[3*2+0] != sim_lua_arg_string )
    {
        simSetLastError("simExtMadaraSystemControllerBridgeRequest", "sourceBottomrightX parameter is not an float.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[4*2+0] != sim_lua_arg_string )
    {
        simSetLastError("simExtMadaraSystemControllerBridgeRequest", "sourceBottomrightY parameter is not an float.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[5*2+0] != sim_lua_arg_string )
    {
        simSetLastError("simExtMadaraSystemControllerBridgeRequest", "sinkTopleftX parameter is not an float.");
        paramsOk = false;
    } 
    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[6*2+0] != sim_lua_arg_string )
    {
        simSetLastError("simExtMadaraSystemControllerBridgeRequest", "sinkTopleftY parameter is not an float.");
        paramsOk = false;
    } 
    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[7*2+0] != sim_lua_arg_string )
    {
        simSetLastError("simExtMadaraSystemControllerBridgeRequest", "sinkBottomrightX parameter is not an float.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[8*2+0] != sim_lua_arg_float )
    {
        simSetLastError("simExtMadaraSystemControllerBridgeRequest", "sinkBottomrightY parameter is not an int.");
        paramsOk = false;
    }

    // Continue forward calling the external functions only if we have all parameters ok.
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
        endRegion.topLeftCorner.x = atof(sourceTopLeftX.c_str());
        endRegion.topLeftCorner.y = atof(sourceTopLeftY.c_str());
        endRegion.bottomRightCorner.x = atof(sourceBotRightX.c_str());
        endRegion.bottomRightCorner.y = atof(sourceBotRightY.c_str());

        // For debugging, print out what we received.
        std::stringstream sstm; 
        sstm << "Values received inside simExtMadaraSystemControllerBridgeRequest function: bridgeId:" << bridgeId << ", "
            << " (" << startRegion.topLeftCorner.x << "," << startRegion.topLeftCorner.y << ")"
            << " (" << startRegion.bottomRightCorner.x << "," << startRegion.bottomRightCorner.y << ")"
            << " (" << endRegion.topLeftCorner.x << "," << endRegion.topLeftCorner.y << ")"
            << " (" << endRegion.bottomRightCorner.x << "," << endRegion.bottomRightCorner.y << ")"
            << std::endl;
        std::string message = sstm.str();
        simAddStatusbarMessage(message.c_str());

        // Make the controller set up the bridge request through the knowledge base.
        madaraController->setupBridgeRequest(bridgeId, startRegion, endRegion);
    }

    simLockInterface(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers the Lua simExtMadaraSystemControllerUpdateStatus command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerMadaraSystemControllerUpdateStatusLuaCallback()
{
    // Define the LUA function input parameters.
    int inArgs[] = {1, sim_lua_arg_int};					  // The number of drones.

    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraSystemControllerUpdateStatus",                  // The Lua function name.
                                 "simExtMadaraSystemControllerUpdateStatus"                   // A tooltip to be shown to help the user know how to call it.
                                                          "(int numberOfDrones)",             
                                 inArgs,                                            // The argument types.
                                 simExtMadaraSystemControllerUpdateStatus);                   // The C function that will be called by the Lua function.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraSystemControllerUpdateStatus command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void simExtMadaraSystemControllerUpdateStatus(SLuaCallBack* p)
{
    //WIN_AFX_MANAGE_STATE;
    simLockInterface(1);

    bool paramsOk = true;

    // Check we have to correct amount of parameters.
    if (p->inputArgCount != 1)
    { 
        simSetLastError("simExtMadaraSystemControllerUpdateStatus", "Not enough arguments.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if (p->inputArgTypeAndSize[0*2+0] != sim_lua_arg_int)
    {
        simSetLastError("simExtMadaraSystemControllerUpdateStatus", "The number of drones is not an integer.");
        paramsOk = false;
    }

    // Continue forward calling the external functions only if we have all parameters ok.
    if(paramsOk)
    { 
        // Get the number of drones.
        int totalNumberOfDrones = p->inputInt[0];

        // Propagate the status information through the network.
        madaraController->updateNetworkStatus(totalNumberOfDrones);
    }

    simLockInterface(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers the Lua simExtMadaraSystemControllerSetupSearchArea command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerMadaraSystemControllerSetupSearchAreaLuaCallback()
{
    // Define the LUA function input parameters.
    int inArgs[] = {5, sim_lua_arg_int,					      // Area ID.
                       sim_lua_arg_string,					  // The X position of the top left corner.
                       sim_lua_arg_string,					  // The Y position of the top left corner.
                       sim_lua_arg_string,					  // The X position of the bottom right corner.
                       sim_lua_arg_string					  // The Y position of the bottom right corner.
                   };

    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraSystemControllerSetupSearchArea",                       // The Lua function name.
                                 "simExtMadaraSystemControllerSetupSearchArea(int areaId,"
                                                                  " float x1, float y1,"
                                                                  " float x2, float y2)",   // A tooltip to be shown to help the user know how to call it.
                                 inArgs,                                                    // The argument types.
                                 simExtMadaraSystemControllerSetupSearchArea);                        // The C function that will be called by the Lua function.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraSystemControllerSetupSearchArea command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void simExtMadaraSystemControllerSetupSearchArea(SLuaCallBack* p)
{
    //WIN_AFX_MANAGE_STATE;
    simLockInterface(1);

    bool paramsOk = true;

    // Check we have to correct amount of parameters.
    if (p->inputArgCount != 5)
    { 
        simSetLastError("simExtMadaraSystemControllerSetupSearchArea", "Not enough arguments.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[0*2+0] != sim_lua_arg_int )
    {
        simSetLastError("simExtMadaraSystemControllerSetupSearchArea", "SearchAreaId parameter is not an int.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[1*2+0] != sim_lua_arg_string )
    {
        simSetLastError("simExtMadaraSystemControllerSetupSearchArea", "X1 parameter is not a float.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[2*2+0] != sim_lua_arg_string )
    {
        simSetLastError("simExtMadaraSystemControllerSetupSearchArea", "Y1 parameter is not a float.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[3*2+0] != sim_lua_arg_string )
    {
        simSetLastError("simExtMadaraSystemControllerSetupSearchArea", "X2 parameter is not a float");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[4*2+0] != sim_lua_arg_string )
    {
        simSetLastError("simExtMadaraSystemControllerSetupSearchArea", "Y2 parameter is not a float.");
        paramsOk = false;
    }

    // Continue forward calling the external functions only if we have all parameters ok.
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
				searchAreaId,	searchAreaRegion.topLeftCorner.x,searchAreaRegion.topLeftCorner.y, 
								searchAreaRegion.bottomRightCorner.x, searchAreaRegion.bottomRightCorner.y);
        simAddStatusbarMessage(message);

        // Tell the controller to actually set up this area.
        madaraController->setNewSearchArea(0, searchAreaRegion);
    }

    simLockInterface(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers the Lua simExtMadaraSystemControllerSearchRequest command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerMadaraSystemControllerSearchRequestLuaCallback()
{
    // Define the LUA function input parameters.
    int inArgs[] = {2, sim_lua_arg_int,					      // Drone ID.
                       sim_lua_arg_int                        // Area ID.
                   };

    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraSystemControllerSearchRequest",                       // The Lua function name.
                                 "simExtMadaraSystemControllerSearchRequest(int droneId, areaId)",  // A tooltip to be shown to help the user know how to call it.
                                 inArgs,                                                  // The argument types.
                                 simExtMadaraSystemControllerSearchRequest);                        // The C function that will be called by the Lua function.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraSystemControllerSearchRequest command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void simExtMadaraSystemControllerSearchRequest(SLuaCallBack* p)
{
    //WIN_AFX_MANAGE_STATE;
    simLockInterface(1);

    bool paramsOk = true;

    // Check we have to correct amount of parameters.
    if (p->inputArgCount != 2)
    { 
        simSetLastError("simExtMadaraSystemControllerSearchRequest", "Not enough arguments.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[0*2+0] != sim_lua_arg_int )
    {
        simSetLastError("simExtMadaraSystemControllerSearchRequest", "DroneId parameter is not an int.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[1*2+0] != sim_lua_arg_int )
    {
        simSetLastError("simExtMadaraSystemControllerSearchRequest", "SearchAreaId parameter is not an int.");
        paramsOk = false;
    }

    // Continue forward calling the external functions only if we have all parameters ok.
    if(paramsOk)
    { 
        // Get the simple input values.
        int droneId = p->inputInt[0];
        int searchAreaId = p->inputInt[1];

        // For debugging, print out what we received.
        std::stringstream sstm; 
        sstm << "Values received inside simExtMadaraSystemControllerSearchRequest function: droneId:" << droneId << ", areaId:" << searchAreaId << std::endl;
        std::string message = sstm.str();
        simAddStatusbarMessage(message.c_str());

        // Tell the controller to actually set this drone to search this area.
        madaraController->requestAreaCoverage(droneId, searchAreaId);            
    }

    simLockInterface(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers the Lua simExtMadaraSystemControllerCleanup command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerMadaraSystemControllerCleanupLuaCallback()
{
    // Define the LUA function input parameters.
    int inArgs[] = {0
                   };

    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraSystemControllerCleanup",                         // The Lua function name.
                                 "simExtMadaraSystemControllerCleanup()",                       // A tooltip to be shown to help the user know how to call it.
                                 inArgs,                                              // The argument types.
                                 simExtMadaraSystemControllerCleanup);                            // The C function that will be called by the Lua function.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraSystemControllerCleanup command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void simExtMadaraSystemControllerCleanup(SLuaCallBack* p)
{
    //WIN_AFX_MANAGE_STATE;
    simLockInterface(1);

    // Simply cleanup the madara controller.
    simAddStatusbarMessage("simExtMadaraSystemControllerCleanup: cleaning up");
    if(madaraController != NULL)
    {
        madaraController->terminate();
        delete madaraController;
        madaraController = NULL;
    }

    simLockInterface(0);
}
