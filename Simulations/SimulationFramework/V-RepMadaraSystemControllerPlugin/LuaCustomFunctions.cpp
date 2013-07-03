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
// Registers the Lua simExtMadaraClientSetup command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerMadaraClientSetupLuaCallback()
{
    // Define the LUA function input parameters.
    int inArgs[] = {2, sim_lua_arg_int,					      // My ID.
                       sim_lua_arg_float,					  // Radio range.
                   };

    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraClientSetup",                         // The Lua function name.
                                 "simExtMadaraClientSetup(int myId, "               // A tooltip to be shown to help the user know how to call it.
                                                         "number radioRange)",
                                 inArgs,                                           // The argument types.
                                 simExtMadaraClientSetup);                          // The C function that will be called by the Lua function.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraClientSetup command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void simExtMadaraClientSetup(SLuaCallBack* p)
{
    //WIN_AFX_MANAGE_STATE;
    simLockInterface(1);

    bool paramsOk = true;

    // Check we have to correct amount of parameters.
    if (p->inputArgCount != 2)
    { 
        simSetLastError("simExtMadaraClientSetup", "Not enough arguments.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[0*2+0] != sim_lua_arg_int )
    {
        simSetLastError("simExtMadaraClientSetup", "MyID parameter is not an int.");
        paramsOk = false;
    }

    if(p->inputArgTypeAndSize[1*2+0] != sim_lua_arg_float)
    {
        simSetLastError("simExtMadaraClientSetup", "RadioRange parameter is not a float.");
        paramsOk = false;
    }

    // Continue forward calling the external functions only if we have all parameters ok.
    if(paramsOk)
    { 
        // Get the simple input values.
        int myId = p->inputInt[0];
        double commRange = p->inputFloat[0];

        // For debugging, print out what we received.
        std::stringstream sstm; 
        sstm << "Values received inside simExtMadaraClientSetup function: myId:" << myId << ", commRange:" << commRange << std::endl;
        std::string message = sstm.str();
        simAddStatusbarMessage(message.c_str());

        // Simply create the madara controller.
        madaraController = new MadaraController(myId, commRange);
    }

    simLockInterface(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers the Lua simExtMadaraClientBridgeRequest command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerMadaraClientBridgeRequestLuaCallback()
{
    // Define the LUA function input parameters.
    int inArgs[] = {9, sim_lua_arg_int,					    // Bridge ID.
                       sim_lua_arg_float,					// The X position of the top left corner of the source.
                       sim_lua_arg_float,					// The Y position of the top left corner of the source.
                       sim_lua_arg_float,					// The X position of the bottom right corner of the source.
                       sim_lua_arg_float,					// The Y position of the bottom right corner of the source.
                       sim_lua_arg_float,					// The X position of the top left corner of the sink.
                       sim_lua_arg_float,					// The Y position of the top left corner of the sink.
                       sim_lua_arg_float,					// The X position of the bottom right corner of the sink.
                       sim_lua_arg_float,					// The Y position of the bottom right corner of the sink.

                   };

    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraClientBridgeRequest",                         // The Lua function name.
                                 "simExtMadaraClientBridgeRequest(int bridgeId, "          // A tooltip to be shown to help the user know how to call it.
                                                                 "float sourceTopleftX,"
                                                                 "float sourceTopleftY,"
                                                                 "float sourceBottomrightX,"
                                                                 "float sourceBottomrightY,"
                                                                 "float sinkTopleftX,"
                                                                 "float sinkTopleftY,"
                                                                 "float sinkBottomrightX,"
                                                                 "float sinkBottomrightY)",
                                 inArgs,                                                    // The argument types.
                                 simExtMadaraClientBridgeRequest);                          // The C function that will be called by the Lua function.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraClientBridgeRequest command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void simExtMadaraClientBridgeRequest(SLuaCallBack* p)
{
    //WIN_AFX_MANAGE_STATE;
    simLockInterface(1);

    bool paramsOk = true;

    // Check we have to correct amount of parameters.
    if (p->inputArgCount != 9)
    { 
        simSetLastError("simExtMadaraClientBridgeRequest", "Not enough arguments.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[0*2+0] != sim_lua_arg_int )
    {
        simSetLastError("simExtMadaraClientBridgeRequest", "RequestId parameter is not an int.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[1*2+0] != sim_lua_arg_float )
    {
        simSetLastError("simExtMadaraClientBridgeRequest", "sourceTopleftX parameter is not an int.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[2*2+0] != sim_lua_arg_float )
    {
        simSetLastError("simExtMadaraClientBridgeRequest", "sourceTopleftY parameter is not an int.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[3*2+0] != sim_lua_arg_float )
    {
        simSetLastError("simExtMadaraClientBridgeRequest", "sourceBottomrightX parameter is not an int.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[4*2+0] != sim_lua_arg_float )
    {
        simSetLastError("simExtMadaraClientBridgeRequest", "sourceBottomrightY parameter is not an int.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[5*2+0] != sim_lua_arg_float )
    {
        simSetLastError("simExtMadaraClientBridgeRequest", "sinkTopleftX parameter is not an int.");
        paramsOk = false;
    } 
    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[6*2+0] != sim_lua_arg_float )
    {
        simSetLastError("simExtMadaraClientBridgeRequest", "sinkTopleftY parameter is not an int.");
        paramsOk = false;
    } 
    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[7*2+0] != sim_lua_arg_float )
    {
        simSetLastError("simExtMadaraClientBridgeRequest", "sinkBottomrightX parameter is not an int.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[8*2+0] != sim_lua_arg_float )
    {
        simSetLastError("simExtMadaraClientBridgeRequest", "sinkBottomrightY parameter is not an int.");
        paramsOk = false;
    }

    // Continue forward calling the external functions only if we have all parameters ok.
    if(paramsOk)
    { 
        // Get the simple input values.
        int bridgeId = p->inputInt[0];

        // Get the float values.
        int floatArrayIdx = 0;
        Position sourceTopLeft(p->inputFloat[floatArrayIdx+0], p->inputFloat[floatArrayIdx+1]);
        Position sourceBottomRight(p->inputFloat[floatArrayIdx+2], p->inputFloat[floatArrayIdx+3]);
        Position sinkTopLeft(p->inputFloat[floatArrayIdx+4], p->inputFloat[floatArrayIdx+5]);
        Position sinkBottomRight(p->inputFloat[floatArrayIdx+6], p->inputFloat[floatArrayIdx+7]);

        // For debugging, print out what we received.
        std::stringstream sstm; 
        sstm << "Values received inside simExtMadaraClientBridgeRequest function: bridgeId:" << bridgeId << ", "
            << " (" << sourceTopLeft.x << "," << sourceTopLeft.y << ")"
            << " (" << sourceBottomRight.x << "," << sourceBottomRight.y << ")"
            << " (" << sinkTopLeft.x << "," << sinkTopLeft.y << ")"
            << " (" << sinkBottomRight.x << "," << sinkBottomRight.y << ")"
            << std::endl;
        std::string message = sstm.str();
        simAddStatusbarMessage(message.c_str());

        // Make the controller set up the bridge request through the knowledge base.
        madaraController->setupBridgeRequest(bridgeId, sourceTopLeft, sourceBottomRight, sinkTopLeft, sinkBottomRight);
    }

    simLockInterface(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers the Lua simExtMadaraClientUpdateStatus command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerMadaraClientUpdateStatusLuaCallback()
{
    // Define the LUA function input parameters.
    int inArgs[] = {1, sim_lua_arg_int};					  // The number of drones.

    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraClientUpdateStatus",                  // The Lua function name.
                                 "simExtMadaraClientUpdateStatus"                   // A tooltip to be shown to help the user know how to call it.
                                                          "(number numberOfDrones)",             
                                 inArgs,                                            // The argument types.
                                 simExtMadaraClientUpdateStatus);                   // The C function that will be called by the Lua function.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraClientUpdateStatus command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void simExtMadaraClientUpdateStatus(SLuaCallBack* p)
{
    //WIN_AFX_MANAGE_STATE;
    simLockInterface(1);

    bool paramsOk = true;

    // Check we have to correct amount of parameters.
    if (p->inputArgCount != 1)
    { 
        simSetLastError("simExtMadaraClientUpdateStatus", "Not enough arguments.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if (p->inputArgTypeAndSize[0*2+0] != sim_lua_arg_int)
    {
        simSetLastError("simExtMadaraClientUpdateStatus", "The number of drones is not an integer.");
        paramsOk = false;
    }

    // Continue forward calling the external functions only if we have all parameters ok.
    if(paramsOk)
    { 
        // Get the number of drones.
        int totalNumberOfDrones = p->inputArgTypeAndSize[0];

        // Propagate the status information through the network.
        madaraController->updateNetworkStatus(totalNumberOfDrones);
    }

    simLockInterface(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers the Lua simExtMadaraClientSetupSearchArea command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerMadaraClientSetupSearchAreaLuaCallback()
{
    // Define the LUA function input parameters.
    int inArgs[] = {5, sim_lua_arg_int,					      // Area ID.
                       sim_lua_arg_float,					  // The X position of the top left corner.
                       sim_lua_arg_float,					  // The Y position of the top left corner.
                       sim_lua_arg_float,					  // The X position of the bottom right corner.
                       sim_lua_arg_float					  // The Y position of the bottom right corner.
                   };

    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraClientSetupSearchArea",                       // The Lua function name.
                                 "simExtMadaraClientSetupSearchArea(int areaId,"
                                                                  " float x1, float y1,"
                                                                  " float x2, float y2)",   // A tooltip to be shown to help the user know how to call it.
                                 inArgs,                                                    // The argument types.
                                 simExtMadaraClientSetupSearchArea);                        // The C function that will be called by the Lua function.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraClientSetupSearchArea command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void simExtMadaraClientSetupSearchArea(SLuaCallBack* p)
{
    //WIN_AFX_MANAGE_STATE;
    simLockInterface(1);

    bool paramsOk = true;

    // Check we have to correct amount of parameters.
    if (p->inputArgCount != 5)
    { 
        simSetLastError("simExtMadaraClientSetupSearchArea", "Not enough arguments.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[0*2+0] != sim_lua_arg_int )
    {
        simSetLastError("simExtMadaraClientSetupSearchArea", "SearchAreaId parameter is not an int.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[1*2+0] != sim_lua_arg_float )
    {
        simSetLastError("simExtMadaraClientSetupSearchArea", "X1 parameter is not a float.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[2*2+0] != sim_lua_arg_float )
    {
        simSetLastError("simExtMadaraClientSetupSearchArea", "Y1 parameter is not a float.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[3*2+0] != sim_lua_arg_float )
    {
        simSetLastError("simExtMadaraClientSetupSearchArea", "X2 parameter is not a float");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[4*2+0] != sim_lua_arg_float )
    {
        simSetLastError("simExtMadaraClientSetupSearchArea", "Y2 parameter is not a float.");
        paramsOk = false;
    }

    // Continue forward calling the external functions only if we have all parameters ok.
    if(paramsOk)
    { 
        // Get the simple input values.
        int searchAreaId = p->inputInt[0];
        Region searchAreaRegion;
        searchAreaRegion.topLeftCorner.x = p->inputFloat[0];
        searchAreaRegion.topLeftCorner.y = p->inputFloat[1];
        searchAreaRegion.bottomRightCorner.x = p->inputFloat[2];
        searchAreaRegion.bottomRightCorner.y = p->inputFloat[3];

        // For debugging, print out what we received.
        std::stringstream sstm; 
        sstm << "Values received inside simExtMadaraClientSetupSearchArea function: area id:" << searchAreaId << std::endl;
        std::string message = sstm.str();
        simAddStatusbarMessage(message.c_str());

        // Tell the controller to actually set up this area.
        madaraController->setNewSearchArea(searchAreaId, searchAreaRegion);
    }

    simLockInterface(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers the Lua simExtMadaraClientSearchRequest command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerMadaraClientSearchRequestLuaCallback()
{
    // Define the LUA function input parameters.
    int inArgs[] = {2, sim_lua_arg_int,					      // Drone ID.
                       sim_lua_arg_int                        // Area ID.
                   };

    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraClientSearchRequest",                       // The Lua function name.
                                 "simExtMadaraClientSearchRequest(int droneId, areaId)",  // A tooltip to be shown to help the user know how to call it.
                                 inArgs,                                                  // The argument types.
                                 simExtMadaraClientSearchRequest);                        // The C function that will be called by the Lua function.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraClientSearchRequest command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void simExtMadaraClientSearchRequest(SLuaCallBack* p)
{
    //WIN_AFX_MANAGE_STATE;
    simLockInterface(1);

    bool paramsOk = true;

    // Check we have to correct amount of parameters.
    if (p->inputArgCount != 2)
    { 
        simSetLastError("simExtMadaraClientSearchRequest", "Not enough arguments.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[0*2+0] != sim_lua_arg_int )
    {
        simSetLastError("simExtMadaraClientSearchRequest", "DroneId parameter is not an int.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[1*2+0] != sim_lua_arg_int )
    {
        simSetLastError("simExtMadaraClientSearchRequest", "SearchAreaId parameter is not an int.");
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
        sstm << "Values received inside simExtMadaraClientSearchRequest function: droneId:" << droneId << ", areaId:" << searchAreaId << std::endl;
        std::string message = sstm.str();
        simAddStatusbarMessage(message.c_str());

        // Tell the controller to actually set this drone to search this area.
        madaraController->requestAreaCoverage(droneId, searchAreaId);            
    }

    simLockInterface(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers the Lua simExtMadaraClientCleanup command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerMadaraClientCleanupLuaCallback()
{
    // Define the LUA function input parameters.
    int inArgs[] = {0
                   };

    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraClientCleanup",                         // The Lua function name.
                                 "simExtMadaraClientCleanup()",                       // A tooltip to be shown to help the user know how to call it.
                                 inArgs,                                              // The argument types.
                                 simExtMadaraClientCleanup);                            // The C function that will be called by the Lua function.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraClientCleanup command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void simExtMadaraClientCleanup(SLuaCallBack* p)
{
    //WIN_AFX_MANAGE_STATE;
    simLockInterface(1);

    // Simply cleanup the madara controller.
    simAddStatusbarMessage("simExtMadaraClientCleanup: cleaning up");
    if(madaraController != NULL)
    {
        madaraController->terminate();
        delete madaraController;
        madaraController = NULL;
    }

    simLockInterface(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers the Lua simExtMadaraClientIsBridging command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerMadaraClientIsBridgingLuaCallback()
{
    // Define the LUA function input parameters.
    int inArgs[] = {1, sim_lua_arg_int,					      // Drone ID.
                   };

    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraClientIsBridging",                       // The Lua function name.
                                 "simExtMadaraClientIsBridging(int droneId)",  // A tooltip to be shown to help the user know how to call it.
                                 inArgs,                                                  // The argument types.
                                 simExtMadaraClientSearchRequest);                        // The C function that will be called by the Lua function.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraClientIsBridging command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void simExtMadaraClientIsBridging(SLuaCallBack* p)
{
    //WIN_AFX_MANAGE_STATE;
    simLockInterface(1);

    bool paramsOk = true;

    // Check we have to correct amount of parameters.
    if (p->inputArgCount != 1)
    { 
        simSetLastError("simExtMadaraClientSearchRequest", "Not enough arguments.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[0*2+0] != sim_lua_arg_int )
    {
        simSetLastError("simExtMadaraClientSearchRequest", "DroneId parameter is not an int.");
        paramsOk = false;
    }

    // Continue forward calling the external functions only if we have all parameters ok.
    if(paramsOk)
    { 
        // Get the simple input values.
        int droneId = p->inputInt[0];

        // For debugging, print out what we received.
        std::stringstream sstm; 
        sstm << "Values received inside simExtMadaraClientSearchRequest function: droneId:" << droneId << std::endl;
        std::string message = sstm.str();
        simAddStatusbarMessage(message.c_str());

        // Check if this drone is bridging.
        bool isBridging = madaraController->isBridging(droneId);

        // Now we prepare the return value(s):
        p->outputArgCount = 1;
        p->outputArgTypeAndSize = (simInt*)simCreateBuffer(
              p->outputArgCount * (2 * sizeof(simInt)));

        p->outputArgTypeAndSize[2*0+0] = sim_lua_arg_bool;
        p->outputArgTypeAndSize[2*0+1] = 1;

        p->outputBool = (simBool*)simCreateBuffer(sizeof(simBool));
        p->outputBool[0] = isBridging;
    }

    simLockInterface(0);
}
