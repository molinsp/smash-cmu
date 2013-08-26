/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#include "utilities/CommonMadaraVariables.h"

#include "drone_controller.h"

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
    knowledge->clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Returns an expression used to get summary of the status of the drone.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string SMASH::DroneController::getStatusSummaryExpression()
{
    std::string status = 
        "Drone {" MV_MY_ID "}\n"
		"Total:\t\t{" MV_TOTAL_DEVICES "}\n"
		"Position:\t{" MV_DEVICE_LAT("{.id}") "},{" MV_DEVICE_LON("{.id}") "}\n"
		"Mobile:\t\t{" MV_MOBILE("{.id}") "}\n"
		"Bridge ID:\t{" MV_BRIDGE_ID("{.id}") "}\n"
        "Search alg:\t{" MV_AREA_COVERAGE_REQUESTED("{.id}") "}\n"
		"Target pos:\t{" MV_MOVEMENT_TARGET_LAT "},{" MV_MOVEMENT_TARGET_LON "}\n"
        "Search end:\t{.area_coverage.cell.bottom_right.location.latitude},{.area_coverage.cell.bottom_right.location.longitude}\n\n"
		"Command:\t{" MV_MOVEMENT_REQUESTED "}: {" MV_MOVEMENT_TARGET_LAT "},{" MV_MOVEMENT_TARGET_LON "}\n"
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
    knowledge.set(MV_MY_ID, (Madara::Knowledge_Record::Integer) droneId);
    knowledge.set(knowledge.expand_statement(MV_MOBILE("{" MV_MY_ID "}")), 1.0);
    knowledge.set(knowledge.expand_statement(MV_BUSY("{" MV_MY_ID "}")), 0.0);

    // Setup the initial command for the first loop of the drone logic as "take off".
    // This is done through variables, similar to how a command would be sent by an external user.
    knowledge.set(MV_DEVICE_MOVE_REQUESTED(knowledge.expand_statement("{" MV_MY_ID "}")), MO_TAKEOFF_CMD);

    // Set madara variable to control human detection. This means the drone will always
    // try to detect human.
    knowledge.set(MV_HUMAN_DETECTION_REQUESTED(knowledge.expand_statement("{" MV_MY_ID "}")), HUMAN_DETECTION_BASIC);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Extra defined function just to force local update settings on global movement variables.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
static Madara::Knowledge_Record process_state_movement_commands (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
  /*TREAT_AS_LOCAL*/
  return variables.evaluate(expressions[PROCESS_STATE_MOVEMENT_COMMANDS],
    Madara::Knowledge_Engine::Knowledge_Update_Settings(true));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Setup of pre-compiled expressions.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SMASH::DroneController::compileExpressions (Madara::Knowledge_Engine::Knowledge_Base& knowledge)
{
	expressions[PROCESS_STATE_MOVEMENT_COMMANDS] = knowledge.compile
	(
		knowledge.expand_statement
		(
			".movement_command=0;"
			"swarm.movement_command  || device.{.id}.movement_command =>"
			"("
				"(( swarm.movement_command => "
				"("
					".movement_command = swarm.movement_command;"
					"copy_vector('swarm.movement_command.*', '.movement_command.');"
				"))"
				"||"
				"( device.{.id}.movement_command => "
				"(" 
					".movement_command = device.{.id}.movement_command;"
					"copy_vector('device.{.id}.movement_command.*', '.movement_command.');"
				")));"
				"swarm.movement_command = 0; device.{.id}.movement_command = 0;"
			")"
		)
	);
	knowledge.define_function("process_state_movement_commands", process_state_movement_commands);

	expressions[PROCESS_STATE] = knowledge.compile
	(
		knowledge.expand_statement
		(
			// TODO: remove this. Just for now, we are constantly setting the values for mobile and busy, to disseminate them.
            "("
                MV_BUSY("{" MV_MY_ID "}") "=" MV_BUSY("{" MV_MY_ID "}") ";"
                MV_MOBILE("{" MV_MY_ID "}") "=" MV_MOBILE("{" MV_MY_ID "}") ";"
            ");"

			"device.{.id}.location=.location;"
			".device.{.id}.location.altitude=.location.altitude;"
			"inflate_coords(.location, '.location');"
			"inflate_coord_array_to_local('device.*');"
			"inflate_coord_array_to_local('region.*');"
			"process_state_movement_commands();"
		)
	);
	
	knowledge.define_function("process_state", expressions[PROCESS_STATE]);

	std::string areaMainLogicCall = m_areaCoverageModule.get_core_function();
    std::string bridgeMainLogicCall = m_bridgeModule.get_core_function();	
	std::string movementMainLogicCall = m_movementModule.get_core_function();
    std::string sensorsMainLogicCall = m_sensorsModule.get_core_function();	
    std::string humanDetectionMainLogicCall = m_humanDetectionModule.get_core_function();  

	expressions[MAIN_LOGIC] = knowledge.compile
	(
		sensorsMainLogicCall + ";" +
		"process_state ();"
        "" + humanDetectionMainLogicCall + ";"
		"("
            ".movement_command"
		    "||"
            "(" + bridgeMainLogicCall + " )"
	        "||"
        	"(" + areaMainLogicCall + ")"
        ");"
		".movement_command => " + movementMainLogicCall + ";"
	);

}
