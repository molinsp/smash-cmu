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

#define NUM_TASKS 	3
#define MAIN_LOGIC 	0
#define PROCESS_STATE   1
#define PROCESS_STATE_MOVEMENT_COMMANDS   2

// Compiled expressions that we expect to be called frequently
static Madara::Knowledge_Engine::Compiled_Expression expressions [NUM_TASKS];

//Extra defined function just to force local update settings on global movement variables
static Madara::Knowledge_Record process_state_movement_commands (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
  /*TREAT_AS_LOCAL*/
  return variables.evaluate(expressions[PROCESS_STATE_MOVEMENT_COMMANDS],
    Madara::Knowledge_Engine::Knowledge_Update_Settings(true));
}

// Returns the main logic expression.
Madara::Knowledge_Engine::Compiled_Expression main_get_main_expression()
{
	return expressions[MAIN_LOGIC];
}

//Setup of pre-compiled expressions
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
//            "("
//                MV_BUSY("{" MV_MY_ID "}") "=" MV_BUSY("{" MV_MY_ID "}") ";"
//                MV_MOBILE("{" MV_MY_ID "}") "=" MV_MOBILE("{" MV_MY_ID "}") ";"
//            ");"

			"device.{.id}.location=.location;"
			".device.{.id}.location.altitude=.location.altitude;"
			"inflate_coords(.location, '.location');"
			"inflate_coord_array_to_local('device.*');"
			"inflate_coord_array_to_local('region.*');"
			"process_state_movement_commands();"
		)
	);
	
	knowledge.define_function("process_state", expressions[PROCESS_STATE]);
	
	std::string areaMainLogicCall = SMASH::AreaCoverage::get_core_function();
    std::string bridgeMainLogicCall = SMASH::Bridge::get_core_function();	
	expressions[MAIN_LOGIC] = knowledge.compile
	(
		"read_sensors ();"
		"process_state ();"
		"(.movement_command"
		"||"
		//"(.needs_bridge =>" + SMASH::Bridge::Main_Function + ")"
        "(" + bridgeMainLogicCall + " )"
		"||"
		"" + areaMainLogicCall + ");"
		".movement_command => process_movement_commands();"
	);

}

