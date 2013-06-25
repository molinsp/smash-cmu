/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#include "MadaraController.h"
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
    int inArgs[] = {5, sim_lua_arg_float,					  // The X position of the controller.
                       sim_lua_arg_float,					  // The Y position of the controller.
                       sim_lua_arg_int  |sim_lua_arg_table,   // IDs of drones. (size: num drones)
                       sim_lua_arg_float|sim_lua_arg_table,   // X and Y posotions for all drones. (size: num drones x 2).
                       sim_lua_arg_bool |sim_lua_arg_table};  // Whether the drones are flying. (size: num drones)

    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraClientUpdateStatus",                  // The Lua function name.
                                 "simExtMadaraClientUpdateStatus"                   // A tooltip to be shown to help the user know how to call it.
                                                          "(number controllerPosx, "             
                                                           "number controllerPosy, "
                                                           "table droneIds, "
                                                           "table dronesPos, "
                                                           "table dronesFlying)",
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
    if (p->inputArgCount != 5)
    { 
        simSetLastError("simExtMadaraClientUpdateStatus", "Not enough arguments.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if (p->inputArgTypeAndSize[0*2+0] != sim_lua_arg_float)
    {
        simSetLastError("simExtMadaraClientUpdateStatus", "The x position of the controller is not a float.");
        paramsOk = false;
    }

    if(p->inputArgTypeAndSize[1*2+0] != sim_lua_arg_float)
    {
        simSetLastError("simExtMadaraClientUpdateStatus", "The y position of the controller is not a float.");
        paramsOk = false;
    }

    if(p->inputArgTypeAndSize[2*2+0] != (sim_lua_arg_int|sim_lua_arg_table) )
    {
        std::string errorMessage = "Drone IDs parameter is not an table of ints, it is a ";
        std::stringstream sstm; sstm << errorMessage << p->inputArgTypeAndSize[2*2+0] ; std::string fullErrorMessage = sstm.str();
        simSetLastError("simExtMadaraClientUpdateStatus", fullErrorMessage.c_str());
        paramsOk = false;
    }

    if(p->inputArgTypeAndSize[2*2+1] < 1)
    {
        simSetLastError("simExtMadaraClientUpdateStatus", "Drone IDs parameter is empty.");
        paramsOk = false;
    }

    if(p->inputArgTypeAndSize[3*2+0] != (sim_lua_arg_float|sim_lua_arg_table))
    {
        std::string errorMessage = "Drone Positions parameter is not an table of floats, it is a ";
        std::stringstream sstm; sstm << errorMessage << p->inputArgTypeAndSize[3*2+0] ; std::string fullErrorMessage = sstm.str();
        simSetLastError("simExtMadaraClientUpdateStatus", fullErrorMessage.c_str());
        paramsOk = false;
    }        
        
    if(p->inputArgTypeAndSize[3*2+1] < 2)
    {
        simSetLastError("simExtMadaraClientUpdateStatus", "Drone Positions parameter is empty.");
        paramsOk = false;
    }

    if(p->inputArgTypeAndSize[4*2+0] != (sim_lua_arg_bool|sim_lua_arg_table)) 
    {
        std::string errorMessage = "Drone Flying Status parameter is not an table of bools, it is a ";
        std::stringstream sstm; sstm << errorMessage << p->inputArgTypeAndSize[4*2+0]; std::string fullErrorMessage = sstm.str();
        simSetLastError("simExtMadaraClientUpdateStatus", fullErrorMessage.c_str());
        paramsOk = false;
    }             
        
    if(p->inputArgTypeAndSize[4*2+1] < 1)
    {
        simSetLastError("simExtMadaraClientUpdateStatus", "Drone Flying Status parameter is empty.");
        paramsOk = false;
    }

    // Continue forward calling the external functions only if we have all parameters ok.
    if(paramsOk)
    { 
        // Indexes for the arrays with all the data.
        int intArrayIdx = 0;
        int floatArrayIdx = 0;
        int boolArrayIdx = 0;

        // Get the controller's position.        
        double controllerPosx = p->inputFloat[floatArrayIdx++];
        double controllerPosy = p->inputFloat[floatArrayIdx++];

        // Create the vector with the drones' status.
        std::vector<DroneStatus> droneStatusList = std::vector<DroneStatus>();
        int totalNumberOfDrones = p->inputArgTypeAndSize[2*2+1];

        for(int currDroneIdx = 0; currDroneIdx < totalNumberOfDrones; currDroneIdx++)
        {
            // Update the current positions in the int, float and bool arrays from which the id, (x,y) positions and flying status will be obtained, respectively.
            int currDroneIdIntArrayIdx = intArrayIdx++;
            int currDronePosFloatArrayIdx = floatArrayIdx + currDroneIdx * 2;	// Note that every position uses up 2 spaces in the float array, 1 for x and 2 for y.
            int currDroneFlyingBoolArrayIdx = boolArrayIdx++;

            // Get the actual values from the array.
            DroneStatus currDroneStatus;
            currDroneStatus.id = p->inputInt[currDroneIdIntArrayIdx];  
            currDroneStatus.posx = p->inputFloat[currDronePosFloatArrayIdx];
            currDroneStatus.posy = p->inputFloat[currDronePosFloatArrayIdx + 1];
            currDroneStatus.flying = p->inputBool[currDroneFlyingBoolArrayIdx];

            // Add the status to the vector.
            droneStatusList.push_back(currDroneStatus);
        }

        // For debugging, print out what we received.
        std::stringstream sstm; 
        sstm << "Values received inside C function: controllerPosx:" << controllerPosx << ", controllerPosy:" << controllerPosy << std::endl; 

        for (std::vector<DroneStatus>::iterator it = droneStatusList.begin() ; it != droneStatusList.end(); ++it)
        {
		    // Get info for current drone ID and position, and mark this drone as not assigned for now.
            sstm << "Drone " << it->id << " with pos " << it->posx << "," << it->posy << ", flying:" << it->flying << std::endl;
        }
        std::string message = sstm.str();
        //simAddStatusbarMessage(message.c_str());

        // Propagate the status information through the network.
        madaraController->updateNetworkStatus(controllerPosx, controllerPosy, droneStatusList);
    }

    simLockInterface(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers the Lua simExtMadaraClientGetNewMovementCommand command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerMadaraClientGetNewMovementCommandLuaCallback()
{
    // Define the LUA function input parameters.
    int inArgs[] = {1, sim_lua_arg_int,					  // Drone ID for which we want the position in bridge, if any.
                   };

    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraClientGetNewMovementCommand",               // The Lua function name.
                                 "myNewX, myNewY, bridging = "                            // A tooltip to be shown to help the user know how to call it.
                                 "simExtMadaraClientGetNewMovementCommand(int droneId) ",     
                                 inArgs,                                                  // The argument types.
                                 simExtMadaraClientGetNewMovementCommand                  // The C function that will be called by the Lua function.
                                 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraClientGetNewMovementCommand command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void simExtMadaraClientGetNewMovementCommand(SLuaCallBack* p)
{
    //WIN_AFX_MANAGE_STATE;
    simLockInterface(1);

    bool paramsOk = true;

    // Check we have to correct amount of parameters.
    if (p->inputArgCount != 1)
    { 
        simSetLastError("simExtMadaraClientGetPositionInBridge", "Not enough arguments.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[0*2+0] != sim_lua_arg_int )
    {
        simSetLastError("simExtMadaraClientGetPositionInBridge", "SourceId parameter is not an int.");
        paramsOk = false;
    }

    // Continue forward calling the external functions only if we have all parameters ok.
    MovementCommand* movementCommand = NULL;
    int isBridgingValue = 0;
    if(paramsOk)
    { 
        // Get the simple input values.
        int droneId = p->inputInt[0];

        // For debugging, print out what we received.
        //std::stringstream sstm; 
        //sstm << "Values received inside simExtMadaraClientGetPositionInBridge function: droneId:" << droneId << std::endl;
        //std::string message = sstm.str();
        //simAddStatusbarMessage(message.c_str());

        // Check to see if there is a new position we want to move to.
        movementCommand = madaraController->getNewMovementCommand(droneId);
        
        // Check if it is bridging or not, to return status along with movement. (This is a shortcut to get more info with this call).
        bool isBridging = madaraController->isBridging(droneId);
        if(isBridging)
        {
            isBridgingValue = 1;
        }
    }

    // Now we prepare the return value(s):
    int numReturnValues = 3;
    p->outputArgCount = numReturnValues; // 3 return values
    p->outputArgTypeAndSize = (simInt*)simCreateBuffer(p->outputArgCount*(2*sizeof(simInt))); // x return values takes x*2 simInt for the type and size buffer

    // Set the actual return values depending on whether we found a position or not.
    bool positionWasFound = movementCommand != NULL;
    if (positionWasFound)
    {
        p->outputArgTypeAndSize[2*0+0] = sim_lua_arg_float;			// The first return value is a float.
        p->outputArgTypeAndSize[2*0+1] = 1;							// Not used (table size if the return value was a table).

        p->outputArgTypeAndSize[2*1+0] = sim_lua_arg_float;			// The second return value is a float
        p->outputArgTypeAndSize[2*1+1] = 1;							// Not used (table size if the return value was a table).

        p->outputArgTypeAndSize[2*2+0] = sim_lua_arg_int;			// The second return value is a float
        p->outputArgTypeAndSize[2*2+1] = 1;							// Not used (table size if the return value was a table).

        p->outputFloat = (simFloat*) simCreateBuffer(2*sizeof(movementCommand->position.x)); // 2 float return values.
        p->outputFloat[0] = (float) movementCommand->position.x;				    // The X part of the target position.
        p->outputFloat[1] = (float) movementCommand->position.y;				    // The Y part of the target position.

        p->outputInt = (simInt*) simCreateBuffer(sizeof(isBridgingValue));          // 1 int return value.
        p->outputInt[0] = isBridgingValue;				                            // The current status of bridging, 1 if it is.
    }
    else
    {
        // All values will just be nil.
        p->outputArgTypeAndSize[2*0+0] = sim_lua_arg_nil;
        p->outputArgTypeAndSize[2*0+1] = 1;					// Not used (table size if the return value was a table)
        p->outputArgTypeAndSize[2*1+0] = sim_lua_arg_nil;
        p->outputArgTypeAndSize[2*1+1] = 1;					// Not used (table size if the return value was a table)
        p->outputArgTypeAndSize[2*2+0] = sim_lua_arg_nil;
        p->outputArgTypeAndSize[2*2+1] = 1;					// Not used (table size if the return value was a table)
    }

    // Free up the memory.
    if(movementCommand != NULL)
    {
        delete movementCommand;
    }

    simLockInterface(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers the Lua simExtMadaraClientStopDrone command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerMadaraClientStopDroneLuaCallback()
{
    // Define the LUA function input parameters.
    int inArgs[] = {1, sim_lua_arg_int					  // Drone ID.
                   };

    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraClientStopDrone",                             // The Lua function name.
                                 "simExtMadaraClientStopDrone(int droneId)",                // A tooltip to be shown to help the user know how to call it.
                                 inArgs,                                                    // The argument types.
                                 simExtMadaraClientStopDrone);                              // The C function that will be called by the Lua function.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraClientStopDrone command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void simExtMadaraClientStopDrone(SLuaCallBack* p)
{
    //WIN_AFX_MANAGE_STATE;
    simLockInterface(1);

    bool paramsOk = true;

    // Check we have to correct amount of parameters.
    if (p->inputArgCount != 1)
    { 
        simSetLastError("simExtMadaraClientStopDrone", "Not enough arguments.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[0*2+0] != sim_lua_arg_int )
    {
        simSetLastError("simExtMadaraClientStopDrone", "DroneId parameter is not an int.");
        paramsOk = false;
    }

    // Continue forward calling the external functions only if we have all parameters ok.
    if(paramsOk)
    { 
        // Get the simple input values.
        int droneId = p->inputInt[0];

        // For debugging, print out what we received.
        std::stringstream sstm; 
        sstm << "Values received inside simExtMadaraClientStopDrone function: droneId:" << droneId << std::endl;
        std::string message = sstm.str();
        simAddStatusbarMessage(message.c_str());

        // Make the controller set up the bridge request through the knowledge base.
        madaraController->stopDrone(droneId);
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