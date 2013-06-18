/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
 
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <stdlib.h>

#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/utility/Log_Macros.h"
#include "ace/Signal.h"
#include "ace/High_Res_Timer.h"
#include "ace/OS_NS_Thread.h"

#include "platforms/platform.h"

#include "movement/movement_module.h"
#include "sensors/sensors_module.h"
#include "utilities/utilities_module.h"

#define NUM_TASKS 	3
#define MAIN_LOGIC 	0
#define PROCESS_STATE   1


//Inturupt handling
volatile bool terminated = false;
extern "C" void terminate (int)
{
  terminated = true;
}

// Compiled expressions that we expect to be called frequently
static Madara::Knowledge_Engine::Compiled_Expression expressions [NUM_TASKS];

//Setup of pre-compiled expressions
void compile_expressions (Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{
	expressions[PROCESS_STATE] = knowledge.compile
	(
		knowledge.expand_statement
		(
			"device.{.id}.location=.location;"
			"inflate_coords(.location, '.location');"
			"inflate_coord_array_to_local('device.*');"
			"inflate_coord_array_to_local('region.*');"


			"((swarm.movement_command || device.{.id}.movement_command) =>"
				"(.movement_command=swarm.movement_command; .movement_command => .movement_command=device.{.id}.movement_command;" 
				"swarm.movement_command = 0; device.{.id}.movement_command=0));"
		)
	);
	knowledge.define_function("process_state", expressions[PROCESS_STATE]);
	
	
	expressions[MAIN_LOGIC] = knowledge.compile
	(
		"read_sensors ();"
		"process_state ();"
		"(.movement_command"
		"||"
		//"(.needs_bridge =>" + SMASH::Bridge::Main_Function + ")"
		"(.needs_bridge => process_bridge_building ())"
		"||"
		"process_area_coverage ());"
		//".movement_command => " + SMASH::Movement::main_logic() + ";"
	);

}

int main (int argc, char** argv)
{
	
	ACE_Sig_Action sa ((ACE_SignalHandler) terminate, SIGINT);
	
	int local_debug_level = 0;
	int id = 0;
	if (argc > 1)
		id = atoi(argv[1]);
	else
	{
		printf("You must supply an ID as the first arguement\n");
		return 0;
	}
	if (argc > 2)
		local_debug_level = atoi(argv[2]);
		
		
	//We call platform init early incase it has to fork()
	if (!init_platform())
	{
		printf("Failed to initialize the platform\n");
		return 1;
	}
	//ACE_OS::sleep(5);

	
	MADARA_debug_level = local_debug_level;
	
	
	Madara::Transport::Settings settings;
	settings.hosts_.resize (1);
	settings.hosts_[0] = "192.168.1.255:15000";
	settings.type = Madara::Transport::BROADCAST;
	settings.queue_length = 1024; //Smaller queue len to preserve memory
	Madara::Knowledge_Engine::Knowledge_Base knowledge ("", settings);
	
	//First thing we do is set our ID, this needs to be changed to actually set it
	//Set our ID
	knowledge.set(".id", Madara::Knowledge_Record::Integer(id));

	
	SMASH::Movement::initialize(knowledge);
	SMASH::Sensors::initialize(knowledge);
	SMASH::Utilities::initialize(knowledge);
	//SMASH::Bridge::initialize(knowledge);

	
	//Compile the main logic
	compile_expressions(knowledge);

	Madara::Knowledge_Engine::Eval_Settings eval_settings;

	while (!terminated)
	{
		knowledge.evaluate (expressions[MAIN_LOGIC], eval_settings);
		knowledge.print_knowledge();
		ACE_OS::sleep (1);
	}
	
	printf("\nExiting...\n");

	return 0;
}
