/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#include "LuaCustomFunctions.h"

// The actual controller to manage the Madara stuff.
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
    int inArgs[] = {1, sim_lua_arg_int,					  // Source ID.
                   };

    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraClientBridgeRequest",                         // The Lua function name.
                                 "simExtMadaraClientBridgeRequest(int sourceId) ",          // A tooltip to be shown to help the user know how to call it.
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
    if (p->inputArgCount != 1)
    { 
        simSetLastError("simExtMadaraClientBridgeRequest", "Not enough arguments.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[0*2+0] != sim_lua_arg_int )
    {
        simSetLastError("simExtMadaraClientBridgeRequest", "SourceId parameter is not an int.");
        paramsOk = false;
    }

    // Continue forward calling the external functions only if we have all parameters ok.
    if(paramsOk)
    { 
        // Get the simple input values.
        int sourceId = p->inputInt[0];

        // For debugging, print out what we received.
        std::stringstream sstm; 
        sstm << "Values received inside simExtMadaraClientBridgeRequest function: sourceId:" << sourceId << std::endl;
        std::string message = sstm.str();
        simAddStatusbarMessage(message.c_str());

        // Make the controller set up the bridge request through the knowledge base.
        madaraController->setupBridgeRequest(sourceId);
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
// Registers the Lua simExtMadaraClientGetPositionInBridge command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerMadaraClientGetPositionInBridge()
{
    // Define the LUA function input parameters.
    int inArgs[] = {1, sim_lua_arg_int,					  // Drone ID for which we want the position in bridge, if any.
                   };

    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtMadaraClientGetPositionInBridge",                   // The Lua function name.
                                 "myNewX, myNewY = "                                        // A tooltip to be shown to help the user know how to call it.
                                 "simExtMadaraClientGetPositionInBridge(int droneId) ",     
                                 inArgs,                                                    // The argument types.
                                 simExtMadaraClientGetPositionInBridge                      // The C function that will be called by the Lua function.
                                 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtMadaraClientGetPositionInBridge command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void simExtMadaraClientGetPositionInBridge(SLuaCallBack* p)
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
    Position* bridgePosition = NULL;
    if(paramsOk)
    { 
        // Get the simple input values.
        int droneId = p->inputInt[0];

        // For debugging, print out what we received.
        std::stringstream sstm; 
        sstm << "Values received inside simExtMadaraClientBridgeRequest function: droneId:" << droneId << std::endl;
        std::string message = sstm.str();
        simAddStatusbarMessage(message.c_str());

        // Make the controller set up the bridge request through the knowledge base.
        bridgePosition = madaraController->getBridgePosition(droneId);
    }

    // Now we prepare the return value(s):
    p->outputArgCount = 2; // 2 return values
    p->outputArgTypeAndSize = (simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer

    // Set the actual return values depending on whether we found a position or not.
    bool positionWasFound = bridgePosition != NULL;
    if (positionWasFound)
    {
        p->outputArgTypeAndSize[2*0+0] = sim_lua_arg_float;			 // The first return value is a float
        p->outputArgTypeAndSize[2*0+1] = 1;							 // Not used (table size if the return value was a table)

        p->outputArgTypeAndSize[2*1+0] = sim_lua_arg_float;			 // The second return value is a float
        p->outputArgTypeAndSize[2*1+1] = 1;							 // Not used (table size if the return value was a table)

        p->outputFloat=(simFloat*)simCreateBuffer(2*sizeof(bridgePosition->x)); // 2 float return value
        p->outputFloat[0] = (float) bridgePosition->x;				 // This is the float value we want to return
        p->outputFloat[1] = (float) bridgePosition->y;				 // This is the float value we want to return
    }
    else
    {
        // Both values will just be nil.
        p->outputArgTypeAndSize[2*0+0] = sim_lua_arg_nil;
        p->outputArgTypeAndSize[2*0+1] = 1;					// Not used (table size if the return value was a table)
        p->outputArgTypeAndSize[2*1+0] = sim_lua_arg_nil;
        p->outputArgTypeAndSize[2*1+1] = 1;					// Not used (table size if the return value was a table)
    }

    // Free up the memory.
    if(bridgePosition != NULL)
    {
        delete bridgePosition;
    }

    simLockInterface(0);
}
