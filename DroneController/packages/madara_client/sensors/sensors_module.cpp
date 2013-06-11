/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
 
#include "sensors/sensors_module.h"
#include "platform_sensors.h"

#define TASK_COUNT		1
#define EVALUATE_SENSORS	0

static Madara::Knowledge_Engine::Compiled_Expression expressions2 [TASK_COUNT];

Madara::Knowledge_Record read_highest_thermal (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	printf("read_highest_thermal();\n");
	double val = human_detected();
	printf("Read Thermal Value: %f\n", val);
	return val;
}

Madara::Knowledge_Record read_sensors (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	printf("read_sensors();\n");
	return variables.evaluate(expressions2[EVALUATE_SENSORS], Madara::Knowledge_Engine::TREAT_AS_LOCAL_UPDATE_SETTINGS);
}


//Define the functions provided by the sensor_functions module
void define_sensor_functions (Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{
	knowledge.define_function ("read_highest_thermal", read_highest_thermal);
	knowledge.define_function ("read_sensors", read_sensors);
}


//Precompile any expressions used by sensor_functions
void compile_sensor_function_expressions (Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{	
	expressions2[EVALUATE_SENSORS] = knowledge.compile
	(
		"drone.{.id}.thermal=read_highest_thermal();"
	);
}

void SMASH::Sensors::initialize(Madara::Knowledge_Engine::Knowledge_Base& knowledge)
{
	init_sensor_functions();

	define_sensor_functions(knowledge);
	compile_sensor_function_expressions(knowledge);
}
std::string SMASH::Sensors::main_logic()
{
	return "read_sensors()";
}

