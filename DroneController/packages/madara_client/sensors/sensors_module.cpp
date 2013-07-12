/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
 
#include "sensors/sensors_module.h"
#include "platform_sensors.h"
#include <iomanip>		// std::setprecision

#define TASK_COUNT		1
#define EVALUATE_SENSORS	0

static Madara::Knowledge_Engine::Compiled_Expression expressions2 [TASK_COUNT];


Madara::Knowledge_Record read_thermal_sensor (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	printf("read_thermal();\n");
	double buffer[8][8];
	read_thermal(buffer);
	int x, y;
	for (x = 0; x < 8; x++)
	{
		for (y = 0; y < 8; y++)
		{
			std::stringstream strBuffer;
			strBuffer << ".sensors.thermal." << x << "." << y;
			variables.set(strBuffer.str(), Madara::Knowledge_Record(buffer[x][y]));
		}
	}
	return Madara::Knowledge_Record::Integer(1);
}

Madara::Knowledge_Record read_gps_sensor (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	printf("in Madara::read_gps\n");
	struct madara_gps gps;
	read_gps(&gps);
	std::stringstream buffer;
	buffer << std::setprecision(10) << gps.latitude << "," << gps.longitude;
	variables.set(".location", buffer.str());
	variables.set(".location.gps.locks", Madara::Knowledge_Record::Integer(gps.num_sats));
	
	return Madara::Knowledge_Record::Integer(1);
}

Madara::Knowledge_Record read_sensors (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	printf("read_sensors();\n");
	return variables.evaluate(expressions2[EVALUATE_SENSORS], Madara::Knowledge_Engine::Eval_Settings(false, true));
}


//Define the functions provided by the sensor_functions module
void define_sensor_functions (Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{
	knowledge.define_function ("read_thermal", read_thermal_sensor);
	knowledge.define_function ("read_gps", read_gps_sensor);
	knowledge.define_function ("read_sensors", read_sensors);
}


//Precompile any expressions used by sensor_functions
void compile_sensor_function_expressions (Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{	
	expressions2[EVALUATE_SENSORS] = knowledge.compile
	(
		//"read_thermal();"
		"read_gps();"
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

