/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#include "movement/movement_module.h"
#include "platform_movement.h"
#include "utilities/CommonMadaraVariables.h"

#define MV_LOCAL_MOVEMENT_COMMAND ".movement_command"

void ensureTakeoff(Madara::Knowledge_Engine::Variables& variables)
{
    //variables.evaluate(MV_IS_LANDED " => takeoff();");
}

void attainAltitude(Madara::Knowledge_Engine::Variables& variables)
{
    //ensureTakeoff(variables);
    //variables.evaluate(variables.expand_statement(MV_IS_AT_ASSIGNED_ALTITUDE " == 0 => (" MV_LOCAL_MOVEMENT_COMMAND ".0 = " MV_ASSIGNED_ALTITUDE("{.id}") "; move_to_altitude();)"));
}

//Madara function to interface with takeoff()
Madara::Knowledge_Record control_functions_takeoff (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	variables.print("In Madara::takeoff()\n", 0);
    
    // Only take off if we are landed, to prevent strange behavior in the drone.
    double isLanded = variables.get(MV_IS_LANDED).to_double();
    if(isLanded == 1)
    {
	    platform_takeoff();
        variables.set(MV_IS_LANDED, 0.0);
    }
    else
    {
        variables.print("Ignoring takeoff command since we are already flying.", 0);
    }

	return Madara::Knowledge_Record::Integer(1);
}

//Madara function to interface with land()
Madara::Knowledge_Record control_functions_land (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	variables.print("In Madara::land()\n", 0);
	platform_land();
    variables.set(MV_IS_LANDED, 1.0);
	return Madara::Knowledge_Record::Integer(1);
}

//Madara function to interface with move_up()
Madara::Knowledge_Record control_functions_move_up (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
    ensureTakeoff(variables);
	platform_move_up();
	return Madara::Knowledge_Record::Integer(1);
}

//Madara function to interface with move_down()
Madara::Knowledge_Record control_functions_move_down (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
    ensureTakeoff(variables);
	platform_move_down();
	return Madara::Knowledge_Record::Integer(1);
}

//Madara function to interface with move_left()
Madara::Knowledge_Record control_functions_move_left (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
    attainAltitude(variables);
	platform_move_left();
	return Madara::Knowledge_Record::Integer(1);
}

//Madara function to interface with move_right()
Madara::Knowledge_Record control_functions_move_right (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
    attainAltitude(variables);
	platform_move_right();
	return Madara::Knowledge_Record::Integer(1);
}

//Madara function to interface with move_forward()
Madara::Knowledge_Record control_functions_move_forward (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
    attainAltitude(variables);
	platform_move_forward();
	return Madara::Knowledge_Record::Integer(1);
}

//Madara function to interface with move_backward()
Madara::Knowledge_Record control_functions_move_backward (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
    attainAltitude(variables);
	platform_move_backward();
	return Madara::Knowledge_Record::Integer(1);
}

Madara::Knowledge_Record madara_move_to_gps (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	double lat = variables.get(MV_MOVEMENT_TARGET_LAT).to_double();
	double lon = variables.get(MV_MOVEMENT_TARGET_LON).to_double();
    double alt = variables.get(variables.expand_statement(MV_ASSIGNED_ALTITUDE("{" MV_MY_ID "}"))).to_double();

    attainAltitude(variables);
	
    // Print an our movement.
    std::stringstream sstream;
    sstream << "Moving to " << lat << ", " << lon << ", " << alt << "\n";
    variables.print(sstream.str(), 0);
	
	platform_move_to_location(lat, lon, alt);

    // Store the current target internally for control.
    variables.set(MV_LAST_TARGET_LAT, lat);
    variables.set(MV_LAST_TARGET_LON, lon);
		
	return Madara::Knowledge_Record::Integer(1);;
}

Madara::Knowledge_Record madara_move_to_altitude (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{		
	double alt = variables.get(MV_LOCAL_MOVEMENT_COMMAND ".0").to_double();

    ensureTakeoff(variables);
	
    std::stringstream sstream;
    sstream << "Moving to altitude" << alt << "\n";
    variables.print(sstream.str(), 0);
	
	platform_move_to_altitude(alt);
		
	return Madara::Knowledge_Record::Integer(1);;
}

Madara::Knowledge_Record process_movement_commands (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
    // Store the current command in another variable for later checks, since the movement command one will be cleared in each loop.
    std::string currentMovementCommand = variables.get(MV_LOCAL_MOVEMENT_COMMAND).to_string();
    if(currentMovementCommand != "0")
    {
        variables.set(MV_LAST_MOVEMENT_REQUESTED, currentMovementCommand);
    }

	std::string expansion = variables.expand_statement("{" MV_LOCAL_MOVEMENT_COMMAND "}();");
	variables.print("Expanded Movement Command: " + expansion + "\n", 0);
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
	knowledge.print("SMASH::Movement::initialize()\n");

	platform_init_control_functions();
	define_control_functions(knowledge);

    // Set initial variables; initially we are not flying.
    knowledge.set(MV_IS_LANDED, 1.0);

	knowledge.print("leaving SMASH::Movement::initialize()\n");
}

void SMASH::Movement::MovementModule::cleanup(Madara::Knowledge_Engine::Knowledge_Base& knowledge)
{
}

std::string SMASH::Movement::MovementModule::get_core_function()
{
	return "process_movement_commands()";
}
