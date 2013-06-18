/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
 

#include "movement/movement_module.h"
#include "platform_movement.h"

#define TASK_COUNT		1
#define EVALUATE_CONTROL	0

static Madara::Knowledge_Engine::Compiled_Expression expressions [TASK_COUNT];

//Madara function to interface with takeoff()
Madara::Knowledge_Record control_functions_takeoff (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	printf("In Madara::takeoff()\n");
	takeoff();
	return Madara::Knowledge_Record::Integer(1);
}

//Madara function to interface with land()
Madara::Knowledge_Record control_functions_land (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	land();
	return Madara::Knowledge_Record::Integer(1);
}

//Madara function to interface with move_up()
Madara::Knowledge_Record control_functions_move_up (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	move_up();
	return Madara::Knowledge_Record::Integer(1);
}

//Madara function to interface with move_down()
Madara::Knowledge_Record control_functions_move_down (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	move_down();
	return Madara::Knowledge_Record::Integer(1);
}

//Madara function to interface with move_left()
Madara::Knowledge_Record control_functions_move_left (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	move_left();
	return Madara::Knowledge_Record::Integer(1);
}

//Madara function to interface with move_right()
Madara::Knowledge_Record control_functions_move_right (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	move_right();
	return Madara::Knowledge_Record::Integer(1);
}

//Madara function to interface with move_forward()
Madara::Knowledge_Record control_functions_move_forward (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	move_forward();
	return Madara::Knowledge_Record::Integer(1);
}

//Madara function to interface with move_backward()
Madara::Knowledge_Record control_functions_move_backward (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	move_backward();
	return Madara::Knowledge_Record::Integer(1);
}

Madara::Knowledge_Record madara_move_to_gps (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
		
	double lat = variables.get(".movement_command.0").to_double();
	double lon = variables.get(".movement_command.1").to_double();
	
	printf("Moving to %02f, %02f\n", lat, lon);
	
	move_to_location(lat, lon);
		
	return Madara::Knowledge_Record::Integer(1);;
}

Madara::Knowledge_Record process_movement_commands (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	printf("process_movement_commands();\n");
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
	knowledge.define_function ("move_forward",    control_functions_move_forward);
	knowledge.define_function ("move_backward",    control_functions_move_backward);
	knowledge.define_function ("move_right",   control_functions_move_right);
	knowledge.define_function("move_to_gps", madara_move_to_gps);
	knowledge.define_function ("process_movement_commands", process_movement_commands);
}


//Precompile any expressions used by control_functions
void compile_control_function_expressions (Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{	
	expressions[EVALUATE_CONTROL] = knowledge.compile
	(
		"{.movement_command}();"
		// If the swarm is given a command, clear the flag and call said command
		//"swarm.takeoff || drone{.id}.takeoff => (swarm.takeoff = 0; drone.{.id}.takeoff = 0; takeoff(););" //Takeoff
    		//"swarm.land || drone{.id}.land => (swarm.land = 0; drone.{.id}.land = 0; land(););" //Land
    		//"swarm.up || drone{.id}.up => (swarm.up = 0; drone.{.id}.up = 0; move_up(););" //Up
    		//"swarm.down || drone{.id}.down => (swarm.down = 0; drone.{.id}.down = 0; move_down(););" //Down
    		//"swarm.left || drone{.id}.left => (swarm.left = 0; drone.{.id}.left = 0; move_left(););" //Left
    		//"swarm.right || drone{.id}.right => (swarm.right = 0; drone.{.id}.right = 0; move_right(););" //Right
		//"swarm.forward || drone{.id}.forward => (swarm.forward = 0; drone.{.id}.forward = 0; move_forward(););" //Forward
		//"swarm.backward || drone{.id}.backward => (swarm.backward = 0; drone.{.id}.backward = 0; move_backward(););" //Backward
	);
}

void SMASH::Movement::initialize(Madara::Knowledge_Engine::Knowledge_Base& knowledge)
{
	init_control_functions();

	define_control_functions(knowledge);
	compile_control_function_expressions(knowledge);
}

std::string SMASH::Movement::main_logic()
{
	return "process_movement_commands()";
}
