/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
 
#include "sensors/sensors_module.h"
#include "platform_sensors.h"
#include "utilities/CommonMadaraVariables.h"
#include "utilities/Position.h"
#include "utilities/gps_utils.h"
#include "utilities/string_utils.h"

#include <iomanip>		// std::setprecision

#define TASK_COUNT		    1
#define EVALUATE_SENSORS	0

#define HEIGHT_ACCURACY     "0.5"     // How many meters to use to determine that a certain height has been reached.
#define MV_LAND_ACCURACY       ".sensors.land_accuracy"     // How many meters to use to determine that a drone has landed.
#define ULTRASOUND_LIMIT    5.5       // Where to stop using ultrasound readings.

static Madara::Knowledge_Engine::Compiled_Expression expressions2 [TASK_COUNT];

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record read_thermal_sensor (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
    variables.print("Inside read_thermal();\n", 0);
    double buffer[8][8];
    platform_read_thermal(buffer);
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
    platform_read_gps(&gps);

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
    double ultraAlt = platform_read_ultrasound();

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
Madara::Knowledge_Record read_battery (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
    // Get the battery remaining from the platform and move it into a Madara variable.
    double batteryRemaining = platform_get_battery_remaining();
    variables.set(MV_BATTERY, batteryRemaining);

    return Madara::Knowledge_Record::Integer(1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record read_sensors (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
    return variables.evaluate(expressions2[EVALUATE_SENSORS], Madara::Knowledge_Engine::Eval_Settings(false, true));
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
        //variables.print("Current command no longer move to GPS, checking for target reached does not make sense.\n", 0);
        return Madara::Knowledge_Record::Integer(0);
    }

    // Get all current values.
    double currLat = variables.get(variables.expand_statement(MV_DEVICE_LAT("{" MV_MY_ID "}"))).to_double();
    double currLon = variables.get(variables.expand_statement(MV_DEVICE_LON("{" MV_MY_ID "}"))).to_double();
    double targetLat = variables.get(MV_LAST_TARGET_LAT).to_double();
    double targetLon = variables.get(MV_LAST_TARGET_LON).to_double();
    double dist = SMASH::Utilities::gps_coordinates_distance(currLat, currLon, targetLat, targetLon);

    // Print the current state.
    std::stringstream sstream;
    sstream << "Lat:      " << currLat   << ", Long:   " << currLon << "\n";
    sstream << "T_Lat:    " << targetLat << ", T_Long: " << targetLon << "\n";
    sstream << "Distance: " << dist << "\n";
    variables.print(sstream.str(), 0);

    // The accuracy of whether a location has been reached depends on the platform. Get the accuracy for the particular platform we are using.
    // For now we assume the accuracy we want is the same as the GPS accuracy, though we may want to add some more slack.
    double reachedAccuracyMeters = platform_get_gps_accuracy();
    if(dist < reachedAccuracyMeters)
    {
        variables.print("HAS reached target\n", 0);
        return Madara::Knowledge_Record::Integer(1);
    }
    else
    {
        variables.print("HAS NOT reached target yet\n", 0);
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
      knowledge.define_function ("read_battery", read_battery);
      knowledge.define_function ("sensors_gpsTargetReached", gpsTargetReached);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Precompile any expressions used by sensor_functions.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void compile_sensor_function_expressions (Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{	
    expressions2[EVALUATE_SENSORS] = knowledge.compile
    (
        "read_battery();"
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
            MV_DEVICE_ALT("{" MV_MY_ID "}") " > " MV_LAND_ACCURACY " => " MV_IS_LANDED " = 0;"
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
    knowledge.print("SMASH::Sensors::initialize...\n");
    platform_init_sensor_functions();

    define_sensor_functions(knowledge);
    compile_sensor_function_expressions(knowledge);

    // Initialize internal variables.
    double landAccuracy = platform_get_landed_height_accuracy();
    knowledge.set(MV_LAND_ACCURACY, landAccuracy);

    knowledge.print("leaving SMASH::Sensors::initialize...\n");
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
