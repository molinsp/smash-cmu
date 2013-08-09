/********************************************************************* 
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * bridge_module.cpp - Defines the manager for bulding a bridge.
 *********************************************************************/

#include <vector>
#include <map>
#include "utilities/CommonMadaraVariables.h"
#include "bridge_module.h"
#include "BridgeAlgorithm.h"

using namespace SMASH::Bridge;
using namespace SMASH::Utilities;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Madara Variable Definitions
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions.
#define MF_MAIN_LOGIC				"bridge_doBridgeBuilding"					// Function that checks if there is bridge building to be done, and does it.
#define MF_UPDATE_AVAILABLE_DRONES	"bridge_updateAvailableDrones"				// Function that checks the amount and positions of drones ready for bridging.
#define MF_FIND_POS_IN_BRIDGE		"bridge_findPositionInBridge"				// Function that finds and sets the position in the bridge for this drone, if any.
#define MF_TURN_OFF_BRIDGE_REQUEST	"bridge_turnOffBridgeRequest"				// Function to turn off a bridge request locally.

// Internal variables.
#define MV_AVAILABLE_DRONES_AMOUNT	".bridge.devices.available.total"			    // The amount of available drones.
#define MV_AVAILABLE_DRONES_IDS		".bridge.devices.available.ids"			        // Array of the ids of the available drones.
#define MV_AVAILABLE_DRONES_LAT	    ".bridge.devices.available.latitudes"		    // Array of the lat part of the position of the drones indicated by MV_AVAILABLE_DRONES_IDS.
#define MV_AVAILABLE_DRONES_LON	    ".bridge.devices.available.longitudes"		    // Array of the long part of the position of the drones indicated by MV_AVAILABLE_DRONES_IDS.
#define MV_CURR_BRIDGE_ID	        ".bridge.curr_bridge_id"		                // Indicates the id of the bridge we are currently part of.
#define MV_BRIDGE_CHECKED(bridgeId) ".bridge." + std::string(bridgeId) + ".checked" // Indicates that we already checked if we had to be part of this bridge.

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private variables.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Define the ids for the expressions that will exist to accomplish different aspects of bridge building.
enum BridgeMadaraExpressionId 
{
    // Expression to call function to update the positions of the drones available for a bridge.
	BE_FIND_AVAILABLE_DRONES_POSITIONS,
};

// Map of Madara expressions used in bridge building.
static std::map<BridgeMadaraExpressionId, Madara::Knowledge_Engine::Compiled_Expression> m_expressions;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private function declarations.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void defineFunctions(Madara::Knowledge_Engine::Knowledge_Base &knowledge);
static void compileExpressions(Madara::Knowledge_Engine::Knowledge_Base &knowledge);
static Madara::Knowledge_Record madaraFindPositionInBridge (Madara::Knowledge_Engine::Function_Arguments &args,
             Madara::Knowledge_Engine::Variables &variables);
static Madara::Knowledge_Record madaraTurnOffBridgeRequest (Madara::Knowledge_Engine::Function_Arguments &args,
             Madara::Knowledge_Engine::Variables &variables);
static Position* calculateMiddlePoint(Madara::Knowledge_Engine::Variables &variables, std::string regionId);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Initializer, gets the refence to the knowledge base and compiles expressions.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SMASH::Bridge::BridgeModule::initialize(Madara::Knowledge_Engine::Knowledge_Base &knowledge)
{
    // Defines internal and external functions.
    defineFunctions(knowledge);

    // Registers all default expressions, to have them compiled for faster access.
    compileExpressions(knowledge);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Cleanup, not needed in this module.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SMASH::Bridge::BridgeModule::cleanup(Madara::Knowledge_Engine::Knowledge_Base &knowledge)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gets the main logic to be run. This returns a function call that can be included in another block of logic.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string SMASH::Bridge::BridgeModule::get_core_function()
{
    return MF_MAIN_LOGIC "()";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers functions with Madara.
// ASSUMPTION: Drone IDs are continuous, starting from 0.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void defineFunctions(Madara::Knowledge_Engine::Knowledge_Base &knowledge)
{
    // Function that can be included in main loop of another method to introduce bridge building. 
    // Only does the actual bridge calculations if the command to do so is on.
    // Assumes that MV_USER_BRIDGE_REQUEST_ON triggers the bridge building logic.
    knowledge.define_function(MF_MAIN_LOGIC, 
        "("
            MV_BRIDGE_REQUESTED " => "
            "("
                // We turn off the bridge request, but only locally.
               MF_TURN_OFF_BRIDGE_REQUEST "();"

               // If we are available and there are bridges to check, check if we can help.
                "(" MV_MOBILE("{" MV_MY_ID "}") " && (!" MV_BUSY("{" MV_MY_ID "}") ") && (" MV_TOTAL_BRIDGES " > 0)" ")"
                    " => " MF_FIND_POS_IN_BRIDGE "();"
            ")"
        ")"
    );

    // Function that actually performs bridge building for this drone.
    knowledge.define_function(MF_FIND_POS_IN_BRIDGE, madaraFindPositionInBridge);

    // Function that actually performs bridge building for this drone.
    knowledge.define_function(MF_TURN_OFF_BRIDGE_REQUEST, madaraTurnOffBridgeRequest);

    // Function to update the amound and positions of drones available for bridges.
    // @param .0    Bridge id, an integer indicating the bridge id for which we are finding available drones.
    knowledge.define_function(MF_UPDATE_AVAILABLE_DRONES, 
        // Set available drones to 0 and disregard its return (choose right).
        MV_AVAILABLE_DRONES_AMOUNT " = 0 ;>"

        // Loop over all the drones to find the available ones.
        ".i[0->" MV_TOTAL_DEVICES ")"
        "("
            // A drone is available if it is mobile and it is not busy, or if it is busy but busy helping with this same bridge.
            "(" MV_MOBILE("{.i}") " && (!" MV_BUSY("{.i}") " || "
                                        "(" MV_BUSY("{.i}") " && (" MV_BRIDGE_ID("{.i}") " == .0) )"
                                        ")"
            ")"
            " => "
            "("
                // If so, increase the amount of available drones, and store its id, x position, and y position in arrays.
                MV_AVAILABLE_DRONES_IDS "{" MV_AVAILABLE_DRONES_AMOUNT "} = .i;"
                MV_AVAILABLE_DRONES_LAT "{" MV_AVAILABLE_DRONES_AMOUNT "} = " MV_DEVICE_LAT("{.i}") ";"
                MV_AVAILABLE_DRONES_LON "{" MV_AVAILABLE_DRONES_AMOUNT "} = " MV_DEVICE_LON("{.i}") ";"
                "++" MV_AVAILABLE_DRONES_AMOUNT ";"
            ");"
        ");"
    );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Compiles all expressions to be used by this class.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void compileExpressions(Madara::Knowledge_Engine::Knowledge_Base &knowledge)
{
    // Expression to update the list of available drone positions, simply calls the predefined function.
    m_expressions[BE_FIND_AVAILABLE_DRONES_POSITIONS] = knowledge.compile(
        MF_UPDATE_AVAILABLE_DRONES "(" MV_CURR_BRIDGE_ID ");"
    );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Method to turn off a bridge request locally.
 * Will be called from an external Madara function.
 * @return  Returns true (1) always.
 **/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record madaraTurnOffBridgeRequest (Madara::Knowledge_Engine::Function_Arguments &args,
             Madara::Knowledge_Engine::Variables &variables)
{
    // Turn off the request by setting the global variable to 0 but only locally.
    variables.set(MV_BRIDGE_REQUESTED, (Madara::Knowledge_Record::Integer) 0.0, Madara::Knowledge_Engine::Eval_Settings(false, true));

    return Madara::Knowledge_Record(1.0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Method that invocates the functionality of finding our position in a bridge, which will be called from Madara when required.
 * Will be called from an external Madara function.
 * @return  Returns true (1) if it can calculate the bridge, or false (0) if it couldn't find all required data.
 **/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record madaraFindPositionInBridge (Madara::Knowledge_Engine::Function_Arguments &args,
	Madara::Knowledge_Engine::Variables &variables)
{
	// Get the basic info: local id, range, and bridge id.
	int myId = (int) variables.get(MV_MY_ID).to_integer();
	double commRange = variables.get(MV_COMM_RANGE).to_double();
	int numBridges = (int) variables.get(MV_TOTAL_BRIDGES).to_integer();

	// Loop over all potential bridges to find one we can help with.
	Position* myNewPosition = NULL;
	BridgeAlgorithm algorithm;
	for(int bridgeId = 0; bridgeId < numBridges; bridgeId++)
	{
		// Store the current bridge ID in the base to be used by the next evaluations.
		variables.set(MV_CURR_BRIDGE_ID, (Madara::Knowledge_Record::Integer) bridgeId);

		// Check if we have already checked if we belonged to this bridge, and if so skip it.
		int currBridgeChecked = (int) variables.get(MV_BRIDGE_CHECKED("{" MV_CURR_BRIDGE_ID "}")).to_integer();
		if(currBridgeChecked == 1)
		{
			continue;
		}

		// Simplify the source region for now, treating it as a point by calculating the middle point.
		std::string bridgeSourceRegionId = variables.get(MV_BRIDGE_SOURCE_REGION_ID("{" MV_CURR_BRIDGE_ID "}")).to_string();
		Position* sourcePosition = calculateMiddlePoint(variables, bridgeSourceRegionId);

		// Simplify the sink region for now, treating it as a point by calculating the middle point.
		std::string bridgeSinkRegionId = variables.get(MV_BRIDGE_SINK_REGION_ID("{" MV_CURR_BRIDGE_ID "}")).to_string();
		Position* sinkPosition = calculateMiddlePoint(variables, bridgeSinkRegionId);

		// Check if we couldn't find the required positions, returning immediately with false.
		if(sourcePosition == NULL || sinkPosition == NULL)
		{
			if(sourcePosition != NULL)
			{
				delete sourcePosition;
			}

			if(sinkPosition != NULL)
			{
				delete sinkPosition;
			}

			return Madara::Knowledge_Record(0.0);
		}

		// Find all the available drones and their positions, called here to ensure atomicity and we have the most up to date data.
		variables.evaluate(m_expressions[BE_FIND_AVAILABLE_DRONES_POSITIONS], Madara::Knowledge_Engine::Eval_Settings(true, true));

		// Obtain the ids, and x and y parts of the available drone's positions from Madara.
		int availableDrones = (int) variables.get(MV_AVAILABLE_DRONES_AMOUNT).to_integer();
		std::vector<Madara::Knowledge_Record> availableDronesIds;
		std::vector<Madara::Knowledge_Record> availableDronesPosX;
		std::vector<Madara::Knowledge_Record> availableDronesPosY;
		variables.to_vector(MV_AVAILABLE_DRONES_IDS, 0, availableDrones, availableDronesIds);
		variables.to_vector(MV_AVAILABLE_DRONES_LAT, 0, availableDrones, availableDronesPosX);
		variables.to_vector(MV_AVAILABLE_DRONES_LON, 0, availableDrones, availableDronesPosY);

		// Move the ids and position coordinates from the three arrays into a combined map.
		std::map<int, Position> availableDronePositions;
		for(int i=0; i < availableDrones; i++)
		{
			int currDroneId = (int) availableDronesIds[i].to_integer();
			double currDronePosX = availableDronesPosX[i].to_double();
			double currDronePosY = availableDronesPosY[i].to_double();
			availableDronePositions[currDroneId] = Position(currDronePosX, currDronePosY);
		}

		// Call the bridge algorithm to find out if I have to move to help witht the bridge.
		myNewPosition = algorithm.getPositionInBridge(myId, commRange, *sourcePosition, *sinkPosition, availableDronePositions);

		// Indicate that we have checked this bridge.
		variables.set(MV_BRIDGE_CHECKED("{" MV_CURR_BRIDGE_ID "}"), 1.0, Madara::Knowledge_Engine::Eval_Settings(true));

		// Cleanup.
		delete sourcePosition;
		delete sinkPosition;

		// If I have to move, tell Madara that I am in bridging mode, and set my final destination.
		bool iHaveToGoToBridge = myNewPosition != NULL;
		if(iHaveToGoToBridge)
		{
			// Update the drone status now that we are going to build a bridge.
			variables.set(MV_BUSY("{" MV_MY_ID "}"), 1.0,
				Madara::Knowledge_Engine::Eval_Settings(true));
			variables.set(MV_BRIDGE_ID("{" MV_MY_ID "}"), (Madara::Knowledge_Record::Integer) bridgeId,
				Madara::Knowledge_Engine::Eval_Settings(true));

            // Set all the command parameters.
            variables.set(MV_MOVEMENT_TARGET_LAT, myNewPosition->x,
				Madara::Knowledge_Engine::Eval_Settings(true));
			variables.set(MV_MOVEMENT_TARGET_LON, myNewPosition->y,
				Madara::Knowledge_Engine::Eval_Settings(true));
			variables.set(MV_MOVEMENT_REQUESTED, std::string(MO_MOVE_TO_GPS_CMD));
		}
	}

	// Cleanup.
	if(myNewPosition != NULL)
	{
		delete myNewPosition;
	}

	return Madara::Knowledge_Record(1.0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Test method used to setup drones in certain locations and issue a bridging request.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Position* calculateMiddlePoint(Madara::Knowledge_Engine::Variables &variables, std::string regionId)
{
    Position* middlePoint = NULL;

    // Calculate the mid point depending on the region type obtained from Madara.
    int regionType = (int) variables.get(MV_REGION_TYPE(regionId)).to_integer();   
    switch(regionType)
    {
        case 0: // Rectangle.
        {
            // Get the bounding box from Madara.
            double topLeftX = variables.get(MV_REGION_TOPLEFT_LAT(regionId)).to_double();
            double topLeftY = variables.get(MV_REGION_TOPLEFT_LON(regionId) ).to_double();
            double bottomRightX = variables.get(MV_REGION_BOTRIGHT_LAT(regionId)).to_double();
            double bottomRightY = variables.get(MV_REGION_BOTRIGHT_LON(regionId) ).to_double();

            // Just find the middle point of the diagonal between the two border points.
            middlePoint = new Position();
            middlePoint->x = (bottomRightX - topLeftX)/2.0 + topLeftX;
            middlePoint->y = (topLeftY - bottomRightY)/2.0 + bottomRightY;
        }
    }

    return middlePoint;
}
