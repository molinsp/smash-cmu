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

static Madara::Knowledge_Engine::Compiled_Expression expressions [TASK_COUNT];

//Madara function to interface with takeoff()
Madara::Knowledge_Record control_functions_takeoff (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	takeoff();
	return "";
}

//Madara function to interface with land()
Madara::Knowledge_Record control_functions_land (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	land();
	return "";
}

//Madara function to interface with move_up()
Madara::Knowledge_Record control_functions_move_up (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	move_up();
	return "";
}

//Madara function to interface with move_down()
Madara::Knowledge_Record control_functions_move_down (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	move_down();
	return "";
}

//Madara function to interface with move_left()
Madara::Knowledge_Record control_functions_move_left (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	move_left();
	return "";
}

//Madara function to interface with move_right()
Madara::Knowledge_Record control_functions_move_right (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	move_right();
	return "";
}

Madara::Knowledge_Record evaluate_control_functions (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	printf("evaluate_control_functions();\n");
	return variables.evaluate(expressions[EVALUATE_CONTROL], Madara::Knowledge_Engine::TREAT_AS_LOCAL_UPDATE_SETTINGS);
}


//Define the functions provided by the control_functions module
void define_control_functions (Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{
	knowledge.define_function ("takeoff", control_functions_takeoff);
	knowledge.define_function ("land",    control_functions_land);
	knowledge.define_function ("move_up",      control_functions_move_up);
	knowledge.define_function ("move_down",    control_functions_move_down);
	knowledge.define_function ("move_left",    control_functions_move_left);
	knowledge.define_function ("move_right",   control_functions_move_right);
	knowledge.define_function ("evaluate_control_functions", evaluate_control_functions);
}


//Precompile any expressions used by control_functions
void compile_control_function_expressions (Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{	
	expressions[EVALUATE_CONTROL] = knowledge.compile
	(
		// If the swarm is given a command, clear the flag and call said command
		"swarm.takeoff || drone{.id}.takeoff => (swarm.takeoff = 0; drone.{.id}.takeoff = 0; takeoff(););" //Takeoff
    		"swarm.land || drone{.id}.land => (swarm.land = 0; drone.{.id}.land = 0; land(););" //Land
    		"swarm.up || drone{.id}.up => (swarm.up = 0; drone.{.id}.up = 0; move_up(););" //Up
    		"swarm.down || drone{.id}.down => (swarm.down = 0; drone.{.id}.down = 0; move_down(););" //Down
    		"swarm.left || drone{.id}.left => (swarm.left = 0; drone.{.id}.left = 0; move_left(););" //Left
    		"swarm.right || drone{.id}.right => (swarm.right = 0; drone.{.id}.right = 0; move_right(););" //Right
	);
}

void init_madara_control_functions (Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{
	init_control_functions();

	define_control_functions(knowledge);
	compile_control_function_expressions(knowledge);
}
