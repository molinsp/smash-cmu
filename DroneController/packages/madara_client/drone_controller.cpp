/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#include "drone_controller.h"

#include "utilities/CommonMadaraVariables.h"
#include "utilities/string_utils.h"

#define NUM_TASKS 	3
#define MAIN_LOGIC 	0
#define PROCESS_STATE   1
#define PROCESS_STATE_MOVEMENT_COMMANDS   2

using namespace SMASH::AreaCoverage;
using namespace SMASH::Bridge;
using namespace SMASH::Movement;
using namespace SMASH::Sensors;
using namespace SMASH::Utilities;
using namespace SMASH::HumanDetection;

// Compiled expressions that we expect to be called frequently
static Madara::Knowledge_Engine::Compiled_Expression expressions [NUM_TASKS];

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Calls all the required initialization procedures.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SMASH::DroneController::initialize(int droneId, Madara::Knowledge_Engine::Knowledge_Base* knowledge)
{
    // Startup the modules.
    initializeModules(*knowledge);

    // Setup and start the drone.
    initializeDrone(droneId, *knowledge);

    // Compile all expressions.
    compileExpressions(*knowledge);

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Calls all the required cleanup procedures.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SMASH::DroneController::cleanup(Madara::Knowledge_Engine::Knowledge_Base* knowledge)
{
    // Cleanup all modules.
    cleanupModules(*knowledge);

    // Cleanup Madara.
    knowledge->close_transport();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Returns an expression used to get summary of the status of the drone.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string SMASH::DroneController::getStatusSummaryExpression()
{
    std::string status = 
        "Drone:\t\t{" MV_MY_ID "}\n"
        "Total:\t\t{" MV_TOTAL_DEVICES "}\n"
        "Mobile:\t\t{" MV_MOBILE("{.id}") "}\n"
        "Battery :\t{" MV_BATTERY "}\n"
        "Position:\t{" MV_DEVICE_LAT("{.id}") "},{" MV_DEVICE_LON("{.id}") "}\n"
        "Height:\t\t{" MV_DEVICE_ALT("{.id}") "}\n"
        "GPS Locks:\t{" MV_GPS_LOCKS "}\n"
        "\n"
        "Curr Command:\t{" MV_MOVEMENT_REQUESTED "}\n"
        "Last Command:\t{" MV_LAST_MOVEMENT_REQUESTED "}\n"
        "Curr target:\t{" MV_MOVEMENT_TARGET_LAT "},{" MV_MOVEMENT_TARGET_LON "}\n"
        "\n"
        "Search alg:\t{" MV_AREA_COVERAGE_REQUESTED("{.id}") "}\n"
        "Search end:\t{.area_coverage.cell.bottom_right.location.latitude},{.area_coverage.cell.bottom_right.location.longitude}\n"
        "Last targ:\t{" MV_CURRENT_COVERAGE_TARGET("{.id}") "}\n"
        "Waiting :\t{" ".area_coverage.my_area.waiting" "}\n"
        "\n"
        "Bridge ID:\t{" MV_BRIDGE_ID("{.id}") "}\n"
        ;

    return status;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Returns the main logic expression.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Engine::Compiled_Expression SMASH::DroneController::get_main_expression()
{
    return expressions[MAIN_LOGIC];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SMASH::DroneController::initializeModules(Madara::Knowledge_Engine::Knowledge_Base& knowledge)
{
    // Create the modules.
    m_areaCoverageModule = AreaCoverageModule();
    m_bridgeModule = BridgeModule();
    m_movementModule = MovementModule();
    m_sensorsModule = SensorsModule();
    m_utilitiesModule = UtilitiesModule();
    m_humanDetectionModule = HumanDetectionModule();

    // Initialize them.
    m_utilitiesModule.initialize(knowledge);
    m_areaCoverageModule.initialize(knowledge);
    m_bridgeModule.initialize(knowledge);
    m_movementModule.initialize(knowledge);
    m_sensorsModule.initialize(knowledge);
    m_humanDetectionModule.initialize(knowledge);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SMASH::DroneController::cleanupModules(Madara::Knowledge_Engine::Knowledge_Base& knowledge)
{
    m_areaCoverageModule.cleanup(knowledge);
    m_bridgeModule.cleanup(knowledge);
    m_movementModule.cleanup(knowledge);
    m_sensorsModule.cleanup(knowledge);
    m_utilitiesModule.cleanup(knowledge);
    m_humanDetectionModule.cleanup(knowledge);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sets up basic drone variables.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SMASH::DroneController::initializeDrone(int droneId, Madara::Knowledge_Engine::Knowledge_Base& knowledge)
{
    // Initialize common variables.
    knowledge.set(MV_MY_ID, (Madara::Knowledge_Record::Integer) droneId);
    knowledge.set(knowledge.expand_statement(MV_MOBILE("{" MV_MY_ID "}")), 1.0);
    knowledge.set(knowledge.expand_statement(MV_BUSY("{" MV_MY_ID "}")), 0.0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Extra defined function just to force local update settings on global movement variables.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
static Madara::Knowledge_Record process_state_movement_commands (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
  // Treat as local to avoid deleting commands for other devices when clearing those variables so that we do not execute them twice.
  return variables.evaluate(expressions[PROCESS_STATE_MOVEMENT_COMMANDS],
                            Madara::Knowledge_Engine::Knowledge_Update_Settings(true));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function to calculate the amount of devices we currently see in the network.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
static Madara::Knowledge_Record calculateNumDevices (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
    // Get all the variables with the "device" prefix.
	std::map<std::string, Madara::Knowledge_Record> map;
	variables.to_map("device.*", map);
	
    // Iterate over the map, and find the ones that correspond to location, just to choose one common variable.
    int numDevicesISee = 0;
	std::map<std::string, Madara::Knowledge_Record>::iterator iter;
	for (iter = map.begin(); iter != map.end(); ++iter)
	{
		if (STRING_ENDS_WITH(iter->first, std::string(".location")))
		{
            numDevicesISee++;
		}		
	}

    // Set the number of devices, locally.
    variables.set(MV_TOTAL_DEVICES_I_SEE, Madara::Knowledge_Record::Integer(numDevicesISee),
                  Madara::Knowledge_Engine::Eval_Settings(true, true));
    
    // Check if the total number of devices has been set globally.
    int globalNumDevices = (int) variables.get(MV_TOTAL_DEVICES_GLOBAL).to_integer();
    if(globalNumDevices != 0)
    {
        // If the number of devices has been set globally, overwrite our internal value with it.
        variables.set(MV_TOTAL_DEVICES, Madara::Knowledge_Record::Integer(globalNumDevices),
                      Madara::Knowledge_Engine::Eval_Settings(true, true));
    }
    else
    {
        // Otherwise use the number of devices I see.
        variables.set(MV_TOTAL_DEVICES, Madara::Knowledge_Record::Integer(numDevicesISee),
                      Madara::Knowledge_Engine::Eval_Settings(true, true));
    }

	return Madara::Knowledge_Record::Integer(1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Setup of pre-compiled expressions.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SMASH::DroneController::compileExpressions (Madara::Knowledge_Engine::Knowledge_Base& knowledge)
{
    // Expression (and function to execute this) to parse commands sent by other devices into our local commands.
    expressions[PROCESS_STATE_MOVEMENT_COMMANDS] = knowledge.compile
    (
        knowledge.expand_statement
        (
            // Reset our internal command variable.
            ".movement_command=0;"

            // Check if we have an external swarm or individual command.
            "(swarm.movement_command || device.{.id}.movement_command) =>"
            "("
                // Swarm commands have priority over regular ones.
                "(( swarm.movement_command => "
                "("
                    // Pass the swarm command plus its parameters into the local variables the movement module will check.
                    ".movement_command = swarm.movement_command;"
                    "copy_vector('swarm.movement_command.*', '.movement_command.');"
                "))"
                "||"
                // If there was no swarm command, we check if we were sent and individual command.
                "( device.{.id}.movement_command => "
                "(" 
                    // Pass the swarm command plus its parameters into the local variables the movement module will check.
                    ".movement_command = device.{.id}.movement_command;"
                    "copy_vector('device.{.id}.movement_command.*', '.movement_command.');"
                ")));"

                // Clear the commands so that we do not execute them again (note that when this is called, this will be done only locally,
                // so as not to propagate this deletion which could prevent other drones from seeing this command.
                "swarm.movement_command = 0; device.{.id}.movement_command = 0;"
            ")"
        )
    );
    knowledge.define_function("process_state_movement_commands", process_state_movement_commands);

    // Define an expression (and an associated function) to broadcast our current information and process external variables
    // tha we have received.
    expressions[PROCESS_STATE] = knowledge.compile
    (
        knowledge.expand_statement
        (
            // TODO: check if we really want to broadcast all of these. We are constantly setting these values, to disseminate them.
            "("
                MV_BUSY("{" MV_MY_ID "}") "=" MV_BUSY("{" MV_MY_ID "}") ";"
                MV_MOBILE("{" MV_MY_ID "}") "=" MV_MOBILE("{" MV_MY_ID "}") ";"
                MV_DEVICE_BATTERY("{" MV_MY_ID "}") "=" MV_BATTERY ";"
                MV_DEVICE_GPS_LOCKS("{" MV_MY_ID "}") "=" MV_GPS_LOCKS ";"
            ");"

            // This will actually broadcast my current location to the network.
            "device.{.id}.location=.location;"

            // Calculate the number of devices I see.
            "calculate_num_devices();"

            // Copy the location to the local variables to be used.
            ".device.{.id}.location.altitude=.location.altitude;"
            
            // Parse the location into separate variables.
            "inflate_coords(.location, '.location');"

            // Parse multiple locations from global variables.
            "inflate_coord_array_to_local('device.*');"
            "inflate_coord_array_to_local('region.*');"

            // Check if there are any movement commands, and process them for the movement module.
            "process_state_movement_commands();"
        )
    );    
    knowledge.define_function("process_state", expressions[PROCESS_STATE]);

    knowledge.define_function("calculate_num_devices", calculateNumDevices);

    // Get the main functions for each module.
    std::string areaMainLogicCall = m_areaCoverageModule.get_core_function();
    std::string bridgeMainLogicCall = m_bridgeModule.get_core_function();	
    std::string movementMainLogicCall = m_movementModule.get_core_function();
    std::string sensorsMainLogicCall = m_sensorsModule.get_core_function();	
    std::string humanDetectionMainLogicCall = m_humanDetectionModule.get_core_function();  

    // Main logic of the program, which will be called once in every iteration.
    expressions[MAIN_LOGIC] = knowledge.compile
    (
        // First get data from sensors.
        sensorsMainLogicCall + ";" +

        // Process external data we may have received.
        "process_state ();" +

        // Run the human detection algorithms.
        humanDetectionMainLogicCall + ";"

        // Choose between either executing a movement command (if any), or bridge building, or area coverage, in that order.
        "("
            ".movement_command"
            "||"
            "(" + bridgeMainLogicCall + " )"
            "||"
            "(" + areaMainLogicCall + ")"
        ");"

        // Execute movement commands, if any.
        ".movement_command => " + movementMainLogicCall + ";"
    );
}
