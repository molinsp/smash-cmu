/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
 
#include "sensors/sensors_module.h"
#include "platform_sensors.h"
#include <iomanip>		// std::setprecision
#include <math.h>
#include "utilities/CommonMadaraVariables.h"
#include "utilities/Position.h"

#include <math.h>

#define TASK_COUNT		    1
#define EVALUATE_SENSORS	0

#define HEIGHT_ACCURACY     "0.5"     // How many meters to use to determine that a certain height has been reached.
#define LAND_ACCURACY       "0.1"     // How many meters to use to determine that a drone has landed.
#define ULTRASOUND_LIMIT    5.5       // Where to stop using ultrasound readings.

// Internal Madara variables.
#define MV_LOCAL_LOCATION            ".location"            // The location (lat,lon) in a local variable.
#define MV_LOCAL_ALTITUDE            ".location.altitude"   // The altitude, in a local variable.
#define MV_GPS_LOCKS                 ".location.gps.locks"  // The amount of GPS locks when the location was taken.

// For Windows compatibility.
#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

// Conversion from degrees to radians.
#define DEG_TO_RAD(x) (x)*M_PI/180.0

static Madara::Knowledge_Engine::Compiled_Expression expressions2 [TASK_COUNT];

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record read_thermal_sensor (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	printf("Inside read_thermal();\n");
	double buffer[8][8];
	read_thermal(buffer);
	int x, y;
	for (x = 0; x < 8; x++)
	{
		for (y = 0; y < 8; y++)
		{
            std::string rowString = NUM_TO_STR(x);
            std::string colString = NUM_TO_STR(y);
			variables.set(MV_THERMAL_BUFFER(rowString,colString), buffer[x][y]);
		}
	}
	return Madara::Knowledge_Record::Integer(1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record read_gps_sensor (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
    // Get GPS from platform.
	struct madara_gps gps;
	read_gps(&gps);

    // Store the latitude and longitude in our local location variable.
	std::stringstream buffer;
	buffer << std::setprecision(10) << gps.latitude << "," << gps.longitude;
	variables.set(MV_LOCAL_LOCATION, buffer.str());

    // Store the altitude obtained by the GPS, but only if we currently were, and the GPS is reporting, an altitude 
    // higher than the ultrasound limit. Otherwise, we will ignore this altitude in favor of the ultrasound one.
    double estAlt = variables.get(MV_LOCAL_ALTITUDE).to_double();
    if(gps.altitude > ULTRASOUND_LIMIT || estAlt > ULTRASOUND_LIMIT)
    {
	    variables.set(MV_LOCAL_ALTITUDE, gps.altitude);
        variables.set(variables.expand_statement(MV_DEVICE_ALT("{.id}")), gps.altitude);
    }

    // Set the amount of GPS locks available for these values.
	variables.set(MV_GPS_LOCKS, Madara::Knowledge_Record::Integer(gps.num_sats));
	
	return Madara::Knowledge_Record::Integer(1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record read_ultrasound (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
    // Get the current ultrasound reading from the platform.
    double ultraAlt = read_ultrasound();

    // Use ultrasound to override any other altitude if we were currently, and the ultrasound reported, a height below a certain limit.
    double estAlt = variables.get(MV_LOCAL_ALTITUDE).to_double();
    if(ultraAlt < ULTRASOUND_LIMIT || estAlt < ULTRASOUND_LIMIT)
    {
        variables.set(variables.expand_statement(MV_DEVICE_ALT("{.id}")), ultraAlt);
        variables.set(MV_LOCAL_ALTITUDE, ultraAlt);
    }

    return Madara::Knowledge_Record::Integer(1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record read_sensors (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	return variables.evaluate(expressions2[EVALUATE_SENSORS], Madara::Knowledge_Engine::Eval_Settings(false, true));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Calculate the distance between two coordinate pairs.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
static double gps_coordinates_distance (double lat1, double long1, double lat2, double long2)
{
    const double EARTH_RADIUS = 6371000;

    // Get the difference between our two points then convert the difference into radians
    double lat_diff = DEG_TO_RAD(lat2 - lat1);
    double long_diff = DEG_TO_RAD(long2 - long1);

    lat1 =  DEG_TO_RAD(lat1);
    lat2 =  DEG_TO_RAD(lat2);

    double a =  pow(sin(lat_diff/2),2)+
                cos(lat1) * cos(lat2) *
                pow ( sin(long_diff/2), 2 );

    double c = 2 * atan2( sqrt(a), sqrt( 1 - a));
    return EARTH_RADIUS * c;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
* Checks last target set to move to has been reached.
* Note: should all this be in the sensors module? Or somewhere else?
* @return  Returns true (1) if we are, or false (0) if not.
**/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record gpsTargetReached (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
    // If the current command is not move to GPS, then the parameters can be different, and it doesn't make sense to check this.
    std::string lastCommand = variables.get(MV_LAST_MOVEMENT_REQUESTED).to_string();
    if(lastCommand != MO_MOVE_TO_GPS_CMD)
    {
        //printf("Current command no longer move to GPS, checking for target reached does not make sense.\n");
        return Madara::Knowledge_Record::Integer(0);
    }

    // Get all current values.
    double currLat = variables.get(variables.expand_statement(MV_DEVICE_LAT("{" MV_MY_ID "}"))).to_double();
	  double currLon = variables.get(variables.expand_statement(MV_DEVICE_LON("{" MV_MY_ID "}"))).to_double();
	  double targetLat = variables.get(MV_LAST_TARGET_LAT).to_double();
	  double targetLon = variables.get(MV_LAST_TARGET_LON).to_double();

    printf("Lat:      %.10f Long:   %.10f\n", currLat, currLon);
    printf("T_Lat:    %.10f T_Long: %.10f\n", targetLat, targetLon);

    double dist = gps_coordinates_distance(currLat, currLon, targetLat, targetLon);
    printf("Distance: %.2f\n", dist);

    // The accuracy of whether a location has been reached depends on the platform. Get the accuracy for the particular platform we are using.
    // For now we assume the accuracy we want is the same as the GPS accuracy, though we may want to add some more slack.
    double reachedAccuracyMeters = get_gps_accuracy();
    if(dist < reachedAccuracyMeters)
    {
        printf("HAS reached target\n");
        return Madara::Knowledge_Record::Integer(1);
    }
    else
    {
        printf("HAS NOT reached target yet\n");
        return Madara::Knowledge_Record::Integer(0);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define the functions provided by the sensor_functions module.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void define_sensor_functions (Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{
	  knowledge.define_function ("read_thermal", read_thermal_sensor);
	  knowledge.define_function ("read_gps", read_gps_sensor);
    knowledge.define_function ("read_ultrasound", read_ultrasound);
	  knowledge.define_function ("read_sensors", read_sensors);
    knowledge.define_function ("sensors_gpsTargetReached", gpsTargetReached);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Precompile any expressions used by sensor_functions.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void compile_sensor_function_expressions (Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{	
	expressions2[EVALUATE_SENSORS] = knowledge.compile
	(
		"read_thermal();"

        // Note: since we are reading the ultrasound after the GPS, the height given by the ultrasound may overwrite the GPS 
        // depending on our current height and the max height defined to be the ultrasound limit.
		"read_gps();"
        "read_ultrasound();"

        // Check if we have reached our assigned altitude.
        // NOTE: Should this be here or somewhere else?
        "("
            MV_IS_AT_ASSIGNED_ALTITUDE " = 0;"
            "("
                "(" MV_ASSIGNED_ALTITUDE("{" MV_MY_ID "}") " != 0) &&"
                "(" MV_DEVICE_ALT("{" MV_MY_ID "}") " - " MV_ASSIGNED_ALTITUDE("{" MV_MY_ID "}") " < " HEIGHT_ACCURACY " ) && "
                "(" MV_ASSIGNED_ALTITUDE("{" MV_MY_ID "}") " - " MV_DEVICE_ALT("{" MV_MY_ID "}") " < " HEIGHT_ACCURACY ")"
            ") => "
                "(" MV_IS_AT_ASSIGNED_ALTITUDE " = 1);"
        ");"

        // Check if we have landed.
        // NOTE: Should this be here or somewhere else?
        "("
            MV_IS_LANDED " = 1;"
            MV_DEVICE_ALT("{" MV_MY_ID "}") " > " LAND_ACCURACY " => " MV_IS_LANDED " = 0;"
        ");"

        // Check if we have reached our GPS target, if any.
        // NOTE: Should this be here or somewhere else?
        "("
            MV_REACHED_GPS_TARGET " = 0;"
           "sensors_gpsTargetReached()" " => " MV_REACHED_GPS_TARGET " = 1;"
        ");"
	);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SMASH::Sensors::SensorsModule::initialize(Madara::Knowledge_Engine::Knowledge_Base& knowledge)
{
    printf("SMASH::Sensors::initialize...\n");
	init_sensor_functions();

	define_sensor_functions(knowledge);
	compile_sensor_function_expressions(knowledge);
    printf("leaving SMASH::Sensors::initialize...\n");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SMASH::Sensors::SensorsModule::cleanup(Madara::Knowledge_Engine::Knowledge_Base& knowledge)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string SMASH::Sensors::SensorsModule::get_core_function()
{
	return "read_sensors()";
}
