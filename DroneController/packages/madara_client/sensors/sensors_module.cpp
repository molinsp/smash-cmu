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
#include <math.h>

#define TASK_COUNT		    1
#define EVALUATE_SENSORS	0

#define HEIGHT_ACCURACY     0.5     // How many meters to use to determine that a certain height has been reached.

static Madara::Knowledge_Engine::Compiled_Expression expressions2 [TASK_COUNT];

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record read_thermal_sensor (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
    variables.print("Inside read_thermal();\n", 2);
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
    // NOTE: we ignore the altitude for now, as this can be obtained from a separate function, and the GPS one may not be the optimal.
    std::stringstream buffer;
    buffer << std::setprecision(10) << gps.latitude << "," << gps.longitude;
    variables.set(MV_LOCAL_LOCATION, buffer.str());

    // Set the amount of GPS locks available for these values.
    variables.set(MV_GPS_LOCKS, Madara::Knowledge_Record::Integer(gps.num_sats));
    
    return Madara::Knowledge_Record::Integer(1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record read_altitude (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
    // Get the altitude and store it in the knowledge base.
    double altitude = platform_get_altitude();
    variables.set(variables.expand_statement(MV_DEVICE_ALT("{.id}")), altitude);
    variables.set(MV_LOCAL_ALTITUDE, altitude);

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Calculates whether we are close enough to our current target to define we have reached it.
// * Note: This should be moved into a platform function.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool hasTargetBeenReached(Madara::Knowledge_Engine::Variables & variables)
{
    // Get all current values.
    double currLat = variables.get(variables.expand_statement(MV_DEVICE_LAT("{" MV_MY_ID "}"))).to_double();
    double currLon = variables.get(variables.expand_statement(MV_DEVICE_LON("{" MV_MY_ID "}"))).to_double();
    double targetLat = variables.get(MV_LAST_TARGET_LAT).to_double();
    double targetLon = variables.get(MV_LAST_TARGET_LON).to_double();

    // Calculate the distance we are from the target.
    double dist = SMASH::Utilities::gps_coordinates_distance(currLat, currLon, targetLat, targetLon);

    // The accuracy of whether a location has been reached depends on the platform. Get the accuracy for the particular platform we are using.
    // For now we assume the accuracy we want is the same as the GPS accuracy, though we may want to add some more slack.
    double reachedAccuracyMeters = platform_get_gps_accuracy();

    // Print the current state.
    std::stringstream sstream;
    sstream << "Lat:      " << currLat   << ", Long:   " << currLon << "\n";
    sstream << "T_Lat:    " << targetLat << ", T_Long: " << targetLon << "\n";
    sstream << "Distance: " << dist      << ", Tolerance: " << reachedAccuracyMeters << "\n";
    variables.print(sstream.str(), 1);

    // Check if we have reached the target, depending if we are close enough to it.
    bool targetHasBeenReached = dist < reachedAccuracyMeters;
    return targetHasBeenReached;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
* Checks if we have reached the last target we were moving towards, and updates a corresponding knowledge base variable.
* Note: should all this be in the sensors module? Or somewhere else?
* @return  Returns true (1) if we updated the value, false (0) if it didn't make sense to calculate this.
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

    // Calculate if we have reached the target, and return appropriately.
    bool targetHasBeenReached = hasTargetBeenReached(variables);
    if(targetHasBeenReached)
    {
        variables.print("HAS reached target\n", 1);
        variables.set(MV_REACHED_GPS_TARGET, Madara::Knowledge_Record::Integer(1));
    }
    else
    {
        variables.print("HAS NOT reached target yet\n", 1);
        variables.set(MV_REACHED_GPS_TARGET, Madara::Knowledge_Record::Integer(0));
    }

    return Madara::Knowledge_Record::Integer(1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Calculates whether we are close enough to our current target to define we have reached it.
// * Note: This should be moved into a platform function.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
static bool hasAltitudeBeenReached(Madara::Knowledge_Engine::Variables & variables)
{
    // Get all current values.
    double currAlt = variables.get(variables.expand_statement(MV_DEVICE_ALT("{" MV_MY_ID "}"))).to_double();
    double targetAlt = variables.get(MV_LAST_TARGET_ALT).to_double();

    // Calculate the difference between the altitudes.
    double altDiff = fabs(targetAlt - currAlt);

    // Check if we have reached the target, depending if we are close enough to it.
    bool altitudeHasBeenReached = altDiff < HEIGHT_ACCURACY;
    return altitudeHasBeenReached;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
* Checks to see if we have reached the last altitude we were moving towards, and updates a corresponding knowledge base variable.
* @return  Returns true (1) if we updated the value, false (0) if it didn't make sense to calculate this.
**/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record altitudeReached (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
    // If the current command is not move to altitude, then the parameters can be different, and it doesn't make sense to check this.
    std::string lastCommand = variables.get(MV_LAST_MOVEMENT_REQUESTED).to_string();
    if(lastCommand != MO_MOVE_TO_ALTITUDE_CMD)
    {
        //variables.print("Current command no longer move to altitude, checking for altitude reached does not make sense.\n", 0);
        return Madara::Knowledge_Record::Integer(0);
    }

    // Calculate if we have reached the altitude, and return appropriately.
    bool altitudeHasBeenReached = hasAltitudeBeenReached(variables);
    if(altitudeHasBeenReached)
    {
        variables.print("HAS reached altitude\n", 1);
        variables.set(MV_REACHED_ALTITUDE, Madara::Knowledge_Record::Integer(1));
    }
    else
    {
        variables.print("HAS NOT reached altitude yet\n", 1);
        variables.set(MV_REACHED_ALTITUDE, Madara::Knowledge_Record::Integer(1));
    }

    return Madara::Knowledge_Record::Integer(1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define the functions provided by the sensor_functions module.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void define_sensor_functions (Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{
      knowledge.define_function ("read_thermal", read_thermal_sensor);
      knowledge.define_function ("read_gps", read_gps_sensor);
      knowledge.define_function ("read_altitude", read_altitude);
      knowledge.define_function ("read_sensors", read_sensors);
      knowledge.define_function ("read_battery", read_battery);
      knowledge.define_function ("sensors_gpsTargetReached", gpsTargetReached);
      knowledge.define_function ("sensors_altitudeTargetReached", altitudeReached);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Precompile any expressions used by sensor_functions.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void compile_sensor_function_expressions (Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{	
    expressions2[EVALUATE_SENSORS] = knowledge.compile
    (
        "read_battery();"
        "read_gps();"
        "read_altitude();"
        "read_thermal();"

        // Check if we have reached our target altitude.
        "sensors_altitudeTargetReached();"

        // Check if we have reached our GPS target, if any.
        "sensors_gpsTargetReached();"
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
