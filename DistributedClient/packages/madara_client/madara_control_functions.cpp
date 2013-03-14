/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
 
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara_control_functions.h"

#include "control_functions.h"

#define TASK_COUNT		1
#define EVALUATE_CONTROL	0
#define TAKEOFF			1
#define LAND			2

static Madara::Knowledge_Engine::Compiled_Expression expressions [TASK_COUNT];

//Madara function to interface with takeoff()
Madara::Knowledge_Record control_functions_takeoff (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	printf("invoke_takeoff();\n");
	execute_takeoff();
	return "";
}

//Madara function to interface with takeoff()
Madara::Knowledge_Record control_functions_land (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	printf("invoke_land();\n");
	execute_land();
	return "";
}

Madara::Knowledge_Record evaluate_control_functions (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	printf("evaluate_control_functions();\n");
	return variables.evaluate(expressions[EVALUATE_CONTROL]);
}


//Define the functions provided by the control_functions module
void define_control_functions (Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{
	knowledge.define_function ("land", control_functions_land);
	knowledge.define_function ("takeoff", control_functions_takeoff);
	knowledge.define_function ("evaluate_control_functions", evaluate_control_functions);
}


//Precompile any expressions used by control_functions
void compile_control_function_expressions (Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{	
	expressions[EVALUATE_CONTROL] = knowledge.compile
	(
		// If the swarm or we were given takeoff command, clear it and takeoff
		"swarm.takeoff || drone{.id}.takeoff => (swarm.takeoff = 0; drone.{.id}.takeoff = 0; takeoff(););"

    		// If the swarm or we were given land command, clear it and land
    		"swarm.land || drone{.id}.land => (swarm.land = 0; drone.{.id}.land = 0; land();)"
	);
}

void init_control_functions (Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{
	define_control_functions(knowledge);
	compile_control_function_expressions(knowledge);
}
