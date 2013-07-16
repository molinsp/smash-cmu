/********************************************************************* 
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#include "bridge/BridgeAlgorithm.h"
#include "utilities/Position.h"

#include "LuaFunctionRegistration.h"
#include "LuaExtensionsUtils.h"

using SMASH::Utilities::Position;
using SMASH::Bridge::BridgeAlgorithm;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtGetPositionInBridge command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define the LUA function input parameters.
int g_getPositionInBridgeinArgs[] = {6, 
					sim_lua_arg_int,						// My ID.
                    sim_lua_arg_float,						// Radio range.
                    sim_lua_arg_float|sim_lua_arg_table,	// X and Y source position. (size: 3)
                    sim_lua_arg_float|sim_lua_arg_table,	// X and Y sink position. (size: 3)
                    sim_lua_arg_int|sim_lua_arg_table,		// IDs of available drones. (size: num available drones)
                    sim_lua_arg_float|sim_lua_arg_table};	// X and Y posotions for all available drones. (size: num available drones x 2).

// The actual callback.
void simExtGetPositionInBridge(SLuaCallBack* p)
{
    simLockInterface(1);

    bool positionWasFound = false;

    float myNewX;
    float myNewY;

	// Continue forward calling the external functions only if we have all parameters ok.
	bool paramsOk = checkInputArguments(p, g_getPositionInBridgeinArgs, "simExtGetPositionInBridge");
    if(paramsOk)
    { 
        // Get the simple input values.
        int myId = p->inputInt[0];
        double commRange = p->inputFloat[0];
        Position sourcePosition = Position(p->inputFloat[1], p->inputFloat[2]); // 3 is the Z coord which we are not using.
        Position sinkPosition = Position(p->inputFloat[4], p->inputFloat[5]);   // 6 is the Z coord which we are not using.

        // Create the map with the available drone positions.
        int droneIdsIntArrayIdx = 1;
        int droneIdsAmount = p->inputArgTypeAndSize[4*2+1];
        int dronePositionsFloatArrayIdx = 7;
        int currDroneIdIntArrayIdx = droneIdsIntArrayIdx;
        int currDronePosFloatArrayIdx = dronePositionsFloatArrayIdx;
        std::map<int, Position> availableDronePositions = std::map<int, Position>();
        for(int currDroneIdx = 0; currDroneIdx < droneIdsAmount; currDroneIdx++)
        {
            // Update the current positions in the int and float arrays from which the id and (x,y) positions will be obtained, respectively.
            currDroneIdIntArrayIdx = droneIdsIntArrayIdx + currDroneIdx;
            currDronePosFloatArrayIdx = dronePositionsFloatArrayIdx + currDroneIdx * 2;	// Note that every position uses up 2 spaces in the float array, 1 for x and 2 for y.

            // Get the actual values from the array.
            int currDroneId = p->inputInt[currDroneIdIntArrayIdx];
            double currDronePositionX = p->inputFloat[currDronePosFloatArrayIdx];
            double currDronePositionY = p->inputFloat[currDronePosFloatArrayIdx + 1];

            // Create the position and add it to the map.
            Position currDronePosition = Position(currDronePositionX, currDronePositionY);
            availableDronePositions[currDroneId] = currDronePosition;
        }

        // For debugging, print out what we received.
        std::stringstream sstm; 
        sstm << "Values received inside C function: myId:" << myId << ", commRange:" << commRange <<
            ", sourcePosX:" << sourcePosition.x << ", sourcePosY:" << sourcePosition.y << 
            ", sinkPosX:" << sinkPosition.x << ", sinkPosY:W" << sinkPosition.y<< std::endl; 

        typedef std::map<int, Position>::iterator it_type;
	    for(it_type iterator = availableDronePositions.begin(); iterator != availableDronePositions.end(); iterator++) 
	    {
		    // Get info for current drone ID and position, and mark this drone as not assigned for now.
		    int currentDroneId = iterator->first;
		    Position currentDronePos = iterator->second;
            sstm << "Drone " << currentDroneId << " with pos " << currentDronePos.x << "," << currentDronePos.y << std::endl;
        }
        simAddStatusbarMessage(sstm.str().c_str());

        // Call the bridge algorithm to find out if I have to move to help witht the bridge.
        BridgeAlgorithm algorithm;
        Position* myNewPosition = algorithm.getPositionInBridge(myId, commRange, sourcePosition, sinkPosition, availableDronePositions);

        // Store the position to be returned, if any.
        if(myNewPosition != NULL)
        {
            positionWasFound = true;
            myNewX = (float) myNewPosition->x;
            myNewY = (float) myNewPosition->y;

            std::stringstream sstmpos; 
            sstmpos << "Position was found! " << myNewX << "," << myNewY << std::endl;
            simAddStatusbarMessage(sstmpos.str().c_str());
        }
    }

    // Now we prepare the return value(s):
    p->outputArgCount = 2; // 2 return values
    p->outputArgTypeAndSize = (simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer

    // Set the actual return values depending on whether we found a position or not.
    if (positionWasFound)
    {
        p->outputArgTypeAndSize[2*0+0] = sim_lua_arg_float;			 // The first return value is a float
        p->outputArgTypeAndSize[2*0+1] = 1;							 // Not used (table size if the return value was a table)

        p->outputArgTypeAndSize[2*1+0] = sim_lua_arg_float;			 // The second return value is a float
        p->outputArgTypeAndSize[2*1+1] = 1;							 // Not used (table size if the return value was a table)

        p->outputFloat=(simFloat*)simCreateBuffer(2*sizeof(myNewX)); // 2 float return value
        p->outputFloat[0] = myNewX;									 // This is the float value we want to return
        p->outputFloat[1] = myNewY;									 // This is the float value we want to return
    }
    else
    {
        // Both values will just be nil.
        p->outputArgTypeAndSize[2*0+0] = sim_lua_arg_nil;
        p->outputArgTypeAndSize[2*0+1] = 1;					// Not used (table size if the return value was a table)
        p->outputArgTypeAndSize[2*1+0] = sim_lua_arg_nil;
        p->outputArgTypeAndSize[2*1+1] = 1;					// Not used (table size if the return value was a table)
    }

    simLockInterface(0);
}

// Register the callback.
void registerGetPositionInBridgeLuaCallback()
{
    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtGetPositionInBridge",                       // The Lua function name.
                                 "number myNewX,number myNewY="                     // A tooltip to be shown to help the user know how to call it.
                                 "simExtGetPositionInBridge(int myId, "
                                                           "number radioRange, "
                                                           "table_3 sourcePos, "	// Note that "table_3" is only a tip to the reader about the table size.
                                                           "table_3 sinkPos, "
                                                           "table availableDroneIds, "
                                                           "table availableDronesPos)",
                                 g_getPositionInBridgeinArgs,                                           // The argument types.
                                 simExtGetPositionInBridge);                        // The C function that will be called by the Lua function.
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers all Lua extensions defined in this module.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void registerAllLuaExtensions()
{
    registerGetPositionInBridgeLuaCallback();
}
