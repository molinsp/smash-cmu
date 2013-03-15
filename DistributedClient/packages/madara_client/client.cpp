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

#include "madara_control_functions.h"

#define NUM_TASKS 	1
#define MAIN_LOGIC 	0

//Inturupt handling
volatile bool terminated = false;
extern "C" void terminate (int)
{
  terminated = true;
}



// Compiled expressions that we expect to be called frequently
static Madara::Knowledge_Engine::Compiled_Expression expressions [NUM_TASKS];


void define_external_functions (Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{
	
}


//Setup of pre-compiled expressions
void compile_expressions (Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{
	
	expressions[MAIN_LOGIC] = knowledge.compile
	(
		EVALUATE_CONTROL_FUNCTIONS
	);

}

int main (int argc, char** argv)
{

	int local_debug_level = 0;
	if (argc > 1)
		local_debug_level = atoi(argv[1]);

	ACE_Sig_Action sa ((ACE_SignalHandler) terminate, SIGINT);
	MADARA_debug_level = local_debug_level;
	
	Madara::Transport::Settings settings;
	settings.hosts_.resize (1);
	settings.hosts_[0] = "228.5.6.7:5500";
	settings.type = Madara::Transport::MULTICAST;

	Madara::Knowledge_Engine::Knowledge_Base knowledge ("", settings);
	
	init_madara_control_functions(knowledge);
	compile_expressions(knowledge);

	Madara::Knowledge_Engine::Eval_Settings eval_settings;

	knowledge.evaluate(".id = 0;", eval_settings);

	while (!terminated)
	{
		knowledge.evaluate (expressions[MAIN_LOGIC], eval_settings);
		ACE_OS::sleep (1);
	}
	
	printf("\nExiting...\n");

	return 0;
}
