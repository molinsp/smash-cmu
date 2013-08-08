/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
 

#include "movement/movement_module.h"
#include "platform_movement.h"
#include "utilities/CommonMadaraVariables.h"

void ensureTakeoff(Madara::Knowledge_Engine::Variables& variables)
{
    variables.evaluate(MV_IS_LANDED " == 0 => takeoff();");
}

void attainAltitude(Madara::Knowledge_Engine::Variables& variables)
{
    ensureTakeoff(variables);
    variables.evaluate(MV_IS_AT_ALTITUDE " == 0 => (.movement_command.0 = " MV_ASSIGNED_ALTITUDE("{.id}") "; move_to_altitude();)");
}

//Madara function to interface with takeoff()
Madara::Knowledge_Record control_functions_takeoff (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	printf("In Madara::takeoff()\n");
	takeoff();
    variables.set(MV_IS_LANDED, 0.0);
	return Madara::Knowledge_Record::Integer(1);
}

//Madara function to interface with land()
Madara::Knowledge_Record control_functions_land (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	printf("In Madara::land()\n");
	land();
    variables.set(MV_IS_LANDED, 1.0);
	return Madara::Knowledge_Record::Integer(1);
}

//Madara function to interface with move_up()
Madara::Knowledge_Record control_functions_move_up (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
    ensureTakeoff(variables);
	move_up();
	return Madara::Knowledge_Record::Integer(1);
}

//Madara function to interface with move_down()
Madara::Knowledge_Record control_functions_move_down (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
    ensureTakeoff(variables);
	move_down();
	return Madara::Knowledge_Record::Integer(1);
}

//Madara function to interface with move_left()
Madara::Knowledge_Record control_functions_move_left (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
    attainAltitude(variables);
	move_left();
	return Madara::Knowledge_Record::Integer(1);
}

//Madara function to interface with move_right()
Madara::Knowledge_Record control_functions_move_right (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
    attainAltitude(variables);
	move_right();
	return Madara::Knowledge_Record::Integer(1);
}

//Madara function to interface with move_forward()
Madara::Knowledge_Record control_functions_move_forward (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
    attainAltitude(variables);
	move_forward();
	return Madara::Knowledge_Record::Integer(1);
}

//Madara function to interface with move_backward()
Madara::Knowledge_Record control_functions_move_backward (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
    attainAltitude(variables);
	move_backward();
	return Madara::Knowledge_Record::Integer(1);
}

Madara::Knowledge_Record madara_move_to_gps (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	double lat = variables.get(".movement_command.0").to_double();
	double lon = variables.get(".movement_command.1").to_double();
    double alt = variables.get(MV_ASSIGNED_ALTITUDE("{.id}")).to_double();

    attainAltitude(variables);
	
	printf("Moving to %08f, %08f, %02f\n", lat, lon, alt);
	
	move_to_location(lat, lon, alt);
		
	return Madara::Knowledge_Record::Integer(1);;
}

Madara::Knowledge_Record madara_move_to_altitude (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{		
	double alt = variables.get(".movement_command.0").to_double();

    ensureTakeoff(variables);
	
	printf("Moving to altitude %02f\n", alt);
	
	move_to_altitude(alt);
		
	return Madara::Knowledge_Record::Integer(1);;
}

Madara::Knowledge_Record process_movement_commands (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	std::string expansion = variables.expand_statement("{.movement_command}();");
	printf("Expanded Movement Command: %s\n", expansion.c_str());
	return variables.evaluate(expansion, Madara::Knowledge_Engine::Eval_Settings(false, true));
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
	knowledge.define_function ("move_to_gps", madara_move_to_gps);
	knowledge.define_function ("move_to_altitude", madara_move_to_altitude);
	knowledge.define_function ("process_movement_commands", process_movement_commands);
}


void SMASH::Movement::MovementModule::initialize(Madara::Knowledge_Engine::Knowledge_Base& knowledge)
{
	printf("SMASH::Movement::initialize()\n");
	init_control_functions();
	define_control_functions(knowledge);
	printf("leaving SMASH::Movement::initialize()\n");
}

void SMASH::Movement::MovementModule::cleanup(Madara::Knowledge_Engine::Knowledge_Base& knowledge)
{
}

std::string SMASH::Movement::MovementModule::get_core_function()
{
	return "process_movement_commands()";
}
