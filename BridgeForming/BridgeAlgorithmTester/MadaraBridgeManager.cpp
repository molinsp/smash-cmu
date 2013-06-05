/********************************************************************* 
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * MadaraBridgeManager.cpp - Defines the manager for bulding a bridge.
 *
 * ASSUMPTIONS:
 * - The current logic assumes that when a drone doing area coverage
 *   finds a person, it stops there.
 * - The logic also assumes that when the user asks the network to form
 *   a bridge, the user will indicate the ID of the stopped drone that 
 *   is to be used as a source. The controller should also provide its
 *   ID, or let the user select one (see Main Input section below).
 *********************************************************************/

#include <vector>
#include <map>
#include "MadaraBridgeManager.h"
#include "BridgeAlgorithm.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Madara Variable Definitions
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
#define MF_MAIN_LOGIC				"doBridgeBuilding"							/* Function that checks if there is bridge building to be done, and does it.  */
#define MF_SEND_BRIDGE_STATE		"sendBridgeState"							/* Function that sends an update about internal bridge status, to allow new drones to always know this state about. */
#define MF_UPDATE_AVAILABLE_DRONES	"updateAvailableDrones"						/* Function that checks the amount and positions of drones ready for bridging.  */
#define MF_FIND_POS_IN_BRIDGE		"findPositionInBridge"						/* Function that finds and sets the position in the bridge for this drone, if any. */

// Main Input:
#define MV_USER_BRIDGE_REQUEST_ID	"user_bridge_request.request_id"			/* Identifies a unique bridge request, along with its bridge. */
#define MV_USER_BRIDGE_REQUEST_ON	"user_bridge_request.enabled"				/* Being true triggers the bridge behavior. */
#define MV_BRIDGE_SOURCE_ID			"user_bridge_request.source_id"				/* Contains the ID of the drone acting as source. */
#define MV_BRIDGE_SINK_ID			"user_bridge_request.sink_id"				/* Containts the ID of the controller acting as sink.*/

// Other Expected Input:
#define MV_TOTAL_DRONES		        "controller.max_drones"						/* The total amount of drones in the system. */
#define MV_COMM_RANGE				"controller.high_bw_comm_range"				/* The range of the high-banwidth radio, in meters. */
#define MV_SINK_X					"controller{" MV_BRIDGE_SINK_ID "}.pos.x"	/* The x position of controller with ID MV_BRIDGE_SINK_ID, in meters. */
#define MV_SINK_Y					"controller{" MV_BRIDGE_SINK_ID "}.pos.y"	/* The y position of controller with ID MV_BRIDGE_SINK_ID, in meters */

#define MV_MY_ID					".id"										/* The id of this drone. */
#define MV_SOURCE_X					"drone{" MV_BRIDGE_SOURCE_ID "}.pos.x"		/* The x position of the drone acting as a source, in meters. */
#define MV_SOURCE_Y					"drone{" MV_BRIDGE_SOURCE_ID "}.pos.y"		/* The y position of the drone acting as a source, in meters. */

#define MV_DRONE_POSX(i)			"drone" i ".pos.x"							/* The x position of a drone with ID i, in meters. */
#define MV_DRONE_POSY(i)			"drone" i ".pos.y"							/* The y position of a drone with ID i, in meters. */
#define MV_MOBILE(i)				"drone" i ".mobile"							/* True of drone with ID i is flying and available for bridging. */
#define MV_BRIDGING(i)				"drone" i ".bridging"						/* True if drone with ID i is bridging. */
#define MV_BRIDGE_ID(i)				"drone" i ".bride_id"						/* If bridging, indicates the ID of the associated request or bridge. */

// Output
// The following parameters are always modified:
// - user_bridge_request.enabled: it is turned to false locally, to ensure a request is processed only once.
// The following parameters are set only if the drone decides it has to become part of a bridge:
// - drone{.i}.bridging: set up as explained above.
// As well as:
#define MV_DRONE_TARGET_POSX(i)		"drone" i ".target_pos.x"					/* The x target position of a drone with ID i, where it should head to, in meters. */
#define MV_DRONE_TARGET_POSY(i)		"drone" i ".target_pos.y"					/* The y target position of a drone with ID i, where it should head to, in meters. */

// Internal variables:
#define MV_AVAILABLE_DRONES_AMOUNT	".available_drones"					// The amount of available drones.
#define MV_AVAILABLE_DRONES_IDS		".available_drones_ids"				// Array of the ids of the available drones.
#define MV_AVAILABLE_DRONES_POSX	".available_drones_position_x"		// Array of the x part of the position of the drones indicated by .available_drones_ids.
#define MV_AVAILABLE_DRONES_POSY	".available_drones_position_y"		// Array of the y part of the position of the drones indicated by .available_drones_ids.

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wrapper non-member function to be used as an entry point by the external Madara function.
// NOTE: This is a C function, not a method of the class. This is because Madara only maps to C functions.
// This is the reason why the MadaraBridgeManager class is a singleton, to allow this function to use
// the same object.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record madaraFindPositionInBridge (Madara::Knowledge_Engine::Function_Arguments &args,
             Madara::Knowledge_Engine::Variables &variables)
{
    // This calls the singleton object MadaraBrigeManager from this external C function.
    return MadaraBridgeManager::getInstance().findPositionInBridge(args, variables);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Method to get singleton.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
MadaraBridgeManager& MadaraBridgeManager::getInstance()
{
    static MadaraBridgeManager instance;
    return instance;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor, gets the refence to the knowledge base and compiles exprsesions.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MadaraBridgeManager::initialize(Madara::Knowledge_Engine::Knowledge_Base &knowledge)
{
    m_knowledge = knowledge;

    // Defines internal and external functions.
    defineFunctions();

    // Registers all default expressions, to have them compiled for faster access.
    compileExpressions();

    // Indicate we start moving.
    m_knowledge.set(MV_MOBILE("{" MV_MY_ID "}"), 1.0, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gets the main logic to be run. This returns a function call that can be included in another block of logic.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string MadaraBridgeManager::getMainLogicMadaraCall()
{
    return MF_MAIN_LOGIC "()";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers functions with Madara.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MadaraBridgeManager::defineFunctions()
{
    // Function that can be included in main loop of another method to introduce bridge building. 
    // Only does the actual bridge calculations if the command to do so is on.
    // Assumes that MV_USER_BRIDGE_REQUEST_ON triggers the bridge building logic.
    m_knowledge.define_function(MF_MAIN_LOGIC, 
        "("
        MF_SEND_BRIDGE_STATE "();"
        MF_UPDATE_AVAILABLE_DRONES "();"
        MV_USER_BRIDGE_REQUEST_ON " => " MF_FIND_POS_IN_BRIDGE "();"
        ")"
    );

    // Function to broadcast the bridge-related state of this drone to the network.
    m_knowledge.define_function(MF_SEND_BRIDGE_STATE, 
        // Dummy variable set for the bridging flag to ensure it is continously propagated, even to new drones.
        "("
        MV_BRIDGING("{" MV_MY_ID "}") "=" MV_BRIDGING("{" MV_MY_ID "}") ";"
        MV_DRONE_TARGET_POSX("{" MV_MY_ID "}") "=" MV_DRONE_TARGET_POSX("{" MV_MY_ID "}") ";"
        MV_DRONE_TARGET_POSY("{" MV_MY_ID "}") "=" MV_DRONE_TARGET_POSY("{" MV_MY_ID "}") ";"
        ")"
    );

    // Function that actually performs bridge building for this drone.
    m_knowledge.define_function(MF_FIND_POS_IN_BRIDGE, madaraFindPositionInBridge);

    // Function to update the amound and positions of drones available for bridges.
    m_knowledge.define_function(MF_UPDATE_AVAILABLE_DRONES, 
        // Set available drones to 0 and disregard its return (choose right).
        MV_AVAILABLE_DRONES_AMOUNT " = 0 ;>"

        // Loop over all the drones to find the available ones.
        ".i[0->" MV_TOTAL_DRONES ")"
        "("
            // A drone is available if it is mobile and it is not bridging THIS bridge 
            // (include drones that are actually bridging this bridge because they responded faster).
            "(((" MV_MOBILE("{.i}") " && !" MV_BRIDGING("{.i}") ") || "
              "(" MV_BRIDGING("{.i}") " && (" MV_BRIDGE_ID("{.i}") " == " MV_USER_BRIDGE_REQUEST_ID ") )"
             ") => ("
                // If so, increase the amount of available drones, and store its id, x position, and y position in arrays.
                MV_AVAILABLE_DRONES_IDS "{" MV_AVAILABLE_DRONES_AMOUNT "} = .i;"
                MV_AVAILABLE_DRONES_POSX "{" MV_AVAILABLE_DRONES_AMOUNT "} = " MV_DRONE_POSX("{.i}") ";"
                MV_AVAILABLE_DRONES_POSY "{" MV_AVAILABLE_DRONES_AMOUNT "} = " MV_DRONE_POSY("{.i}") ";"
                "++" MV_AVAILABLE_DRONES_AMOUNT ";"
            "));"
        ")"
    );

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Compiles all expressions to be used by this class.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MadaraBridgeManager::compileExpressions()
{
    // Expression to update the list of available drone positions, simply calls the predefine function.
    m_expressions[BE_FIND_AVAILABLE_DRONES_POSITIONS] = m_knowledge.compile(
        MF_UPDATE_AVAILABLE_DRONES "();"
    );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Assumes that, when it is called, the following Madara values have already been set in the bridge request by the sink:
//  - MV_COMM_RANGE: the comm range between drones.
//  - MV_BRIDGE_SOURCE_ID: the ID of the drone acting as the source.
//  - MV_BRIDGE_SINK_ID: the ID of the device acting as the controller which wants to be the sink.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record MadaraBridgeManager::findPositionInBridge (Madara::Knowledge_Engine::Function_Arguments &args,
             Madara::Knowledge_Engine::Variables &variables)
{
    // Get the basic info: source and sink positions, my id and the comm range.
    int myId = (int) variables.get(MV_MY_ID).to_integer();
    double commRange = variables.get(MV_COMM_RANGE).to_double();
    int bridgeRequestId = (int) variables.get(MV_USER_BRIDGE_REQUEST_ID).to_integer();
    double sourceX = variables.get(MV_SOURCE_X).to_double();
    double sourceY = variables.get(MV_SOURCE_Y).to_double();
    Position sourcePosition(sourceX, sourceY);
    double sinkX = variables.get(MV_SINK_X).to_double();
    double sinkY = variables.get(MV_SINK_Y).to_double();
    Position sinkPosition(sinkX, sinkY);

    // Find all the available drones and their positions, called here to ensure atomicity and we have the most up to date data.
    variables.evaluate(m_expressions[BE_FIND_AVAILABLE_DRONES_POSITIONS], Madara::Knowledge_Engine::DELAY_AND_TREAT_AS_LOCAL_EVAL_SETTINGS);

    // Obtain the ids, and x and y parts of the available drone's positions from Madara.
    int availableDrones = (int) variables.get(MV_AVAILABLE_DRONES_AMOUNT).to_integer();
    std::vector<Madara::Knowledge_Record> availableDronesIds;
    std::vector<Madara::Knowledge_Record> availableDronesPosX;
    std::vector<Madara::Knowledge_Record> availableDronesPosY;
    variables.to_vector(MV_AVAILABLE_DRONES_IDS, 0, availableDrones, availableDronesIds);
    variables.to_vector(MV_AVAILABLE_DRONES_POSX, 0, availableDrones, availableDronesPosX);
    variables.to_vector(MV_AVAILABLE_DRONES_POSY, 0, availableDrones, availableDronesPosY);

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
    BridgeAlgorithm algorithm;
    Position* myNewPosition = algorithm.getPositionInBridge(myId, commRange, sourcePosition, sinkPosition, availableDronePositions);

    // If I have to move, tell Madara that I am in bridging mode, and set my final destination.
    bool iHaveToGoToBridge = myNewPosition != NULL;
    if(iHaveToGoToBridge)
    {
        // Update the drone status now that we are going to build a bridge.
        variables.set(MV_BRIDGING("{" MV_MY_ID "}"), 1.0, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);
        variables.set(MV_BRIDGE_ID("{" MV_MY_ID "}"), (Madara::Knowledge_Record::Integer) bridgeRequestId, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);
        variables.set(MV_DRONE_TARGET_POSX("{" MV_MY_ID "}"), myNewPosition->x, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);
        variables.set(MV_DRONE_TARGET_POSY("{" MV_MY_ID "}"), myNewPosition->y);
    }

    // Indicate, locally, that we already addressed this request, so we don't get stuck recalculating everything each cycle.
    variables.set(MV_USER_BRIDGE_REQUEST_ON, 0.0, Madara::Knowledge_Engine::TREAT_AS_LOCAL_UPDATE_SETTINGS);

    return Madara::Knowledge_Record(1.0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Test method used to setup drones in certain locations and issue a bridging request.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MadaraBridgeManager::setupBridgeTest()
{
    // Setup the algorithm inputs.
    int id = 0;
    double commRange = 4.0;
    Position sourcePosition = Position(9.0, 8.8);
    Position sinkPosition =  Position(0.5,0.5);
    std::map<int, Position> availableDrones;
    availableDrones[0] = Position(3.48578,6.9078);
    availableDrones[5] = Position(8.99198,3.6877);
    availableDrones[8] = Position(3.98820,3.6670);

    //BridgeAlgorithm bridgeAlg;
    //Position* result = bridgeAlg.getPositionInBridge(id, commRange, sourcePosition, sinkPosition, availableDrones);

    // Generate information about my position and the position of others, also indicating we are mobile.
    m_knowledge.set(MV_DRONE_POSX("0"), availableDrones[0].x);
    m_knowledge.set(MV_DRONE_POSY("0"), availableDrones[0].y);
    m_knowledge.set(MV_MOBILE("0"), 1.0);
    
    m_knowledge.set(MV_DRONE_POSX("5"), availableDrones[5].x);	
    m_knowledge.set(MV_DRONE_POSY("5"), availableDrones[5].y);
    m_knowledge.set(MV_MOBILE("5"), 1.0);
    
    m_knowledge.set(MV_DRONE_POSX("8"), availableDrones[8].x);
    m_knowledge.set(MV_DRONE_POSY("8"), availableDrones[8].y);
    m_knowledge.set(MV_MOBILE("8"), 1.0);

    m_knowledge.set(MV_DRONE_POSX("1"), sourcePosition.x);
    m_knowledge.set(MV_DRONE_POSY("1"), sourcePosition.y);
    m_knowledge.set(MV_MOBILE("1"), 0.0);

    // Generate information that should be set by sink when sending command for bridge.
    m_knowledge.set(MV_TOTAL_DRONES, 9.0);
    m_knowledge.set(MV_COMM_RANGE, commRange);
    m_knowledge.set(MV_USER_BRIDGE_REQUEST_ID, (Madara::Knowledge_Record::Integer) 1);
    m_knowledge.set(MV_BRIDGE_SOURCE_ID, (Madara::Knowledge_Record::Integer) 1);
    m_knowledge.set(MV_BRIDGE_SINK_ID, (Madara::Knowledge_Record::Integer) 1);

    // Generate pos for sink (id = 1)
    m_knowledge.set(m_knowledge.expand_statement(MV_SINK_X), sinkPosition.x);
    m_knowledge.set(m_knowledge.expand_statement(MV_SINK_Y), sinkPosition.y);

    // Simulate the sink actually sending the command to bridge.
    m_knowledge.set(MV_USER_BRIDGE_REQUEST_ON, 1.0);
}
