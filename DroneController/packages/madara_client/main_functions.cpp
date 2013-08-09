/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
 
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "utilities/CommonMadaraVariables.h"

#include "area_coverage/area_coverage_module.h"
#include "bridge/bridge_module.h"
#include "movement/movement_module.h"
#include "sensors/sensors_module.h"
#include "utilities/utilities_module.h"

#include "utilities/CommonMadaraVariables.h"
#include "platforms/platform.h"

#define NUM_TASKS 	3
#define MAIN_LOGIC 	0
#define PROCESS_STATE   1
#define PROCESS_STATE_MOVEMENT_COMMANDS   2

using namespace SMASH::AreaCoverage;
using namespace SMASH::Bridge;
using namespace SMASH::Movement;
using namespace SMASH::Sensors;
using namespace SMASH::Utilities;

// Compiled expressions that we expect to be called frequently
static Madara::Knowledge_Engine::Compiled_Expression expressions [NUM_TASKS];

// The modules.
static AreaCoverageModule m_areaCoverageModule;
static BridgeModule m_bridgeModule;
static MovementModule m_movementModule;
static SensorsModule m_sensorsModule;
static UtilitiesModule m_utilitiesModule;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void initializeModules(Madara::Knowledge_Engine::Knowledge_Base& knowledge)
{
	// Create the modules.
	m_areaCoverageModule = AreaCoverageModule();
	m_bridgeModule = BridgeModule();
	m_movementModule = MovementModule();
	m_sensorsModule = SensorsModule();
	m_utilitiesModule = UtilitiesModule();

	// Initialize them.
    m_utilitiesModule.initialize(knowledge);
    m_areaCoverageModule.initialize(knowledge);
    m_bridgeModule.initialize(knowledge);
    m_movementModule.initialize(knowledge);
    m_sensorsModule.initialize(knowledge);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void cleanupModules(Madara::Knowledge_Engine::Knowledge_Base& knowledge)
{
    m_areaCoverageModule.cleanup(knowledge);
    m_bridgeModule.cleanup(knowledge);
    m_movementModule.cleanup(knowledge);
    m_sensorsModule.cleanup(knowledge);
    m_utilitiesModule.cleanup(knowledge);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sets up basic drone variables.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void initializeDrone(int droneId, Madara::Knowledge_Engine::Knowledge_Base& knowledge)
{
    knowledge.set (".id", (Madara::Knowledge_Record::Integer) droneId);
    knowledge.set(MV_MOBILE("{" MV_MY_ID "}"), 1.0);
    knowledge.set(MV_BUSY("{" MV_MY_ID "}"), 0.0);

    // TODO: move this into control loop
    // Tell the drone to take off.
    knowledge.set(MV_DEVICE_MOVE_REQUESTED("{" MV_MY_ID "}"), MO_TAKEOFF_CMD);
    //knowledge.print_knowledge();
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
// Returns the main logic expression.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Engine::Compiled_Expression main_get_main_expression()
{
	return expressions[MAIN_LOGIC];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Setup of pre-compiled expressions.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main_compile_expressions (Madara::Knowledge_Engine::Knowledge_Base & knowledge)
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
            //"("
            //    MV_BUSY("{" MV_MY_ID "}") "=" MV_BUSY("{" MV_MY_ID "}") ";"
            //    MV_MOBILE("{" MV_MY_ID "}") "=" MV_MOBILE("{" MV_MY_ID "}") ";"
            //");"

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

	expressions[MAIN_LOGIC] = knowledge.compile
	(
		sensorsMainLogicCall + ";"
		"process_state ();"
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Calls all the required initialization procedures.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool initializeDroneController(int droneId, Madara::Knowledge_Engine::Knowledge_Base& knowledge)
{
    // Startup the modules.
	initializeModules(knowledge);

	// Setup and start the drone.
	initializeDrone(droneId, knowledge);

	// Compile all expressions.
    main_compile_expressions(knowledge);

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Calls all the required cleanup procedures.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void cleanupDroneController(Madara::Knowledge_Engine::Knowledge_Base& knowledge)
{
	// Cleanup all modules.
	cleanupModules(knowledge);

	// Cleanup Madara.
	knowledge.close_transport();
    knowledge.clear();
}