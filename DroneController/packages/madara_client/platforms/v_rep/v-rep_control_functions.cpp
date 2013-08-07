/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#pragma once

#ifdef V_REP


#include "platforms/platform.h"
#include "movement/platform_movement.h"
#include "sensors/platform_sensors.h"

#include "utilities/CommonMadaraVariables.h"
#include "madara/knowledge_engine/Knowledge_Base.h"
#include <string>

#include <cmath>

#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

#define DEG_TO_RAD(x) x * M_PI / 180

// NOTE: We are using a hack here, assuming that an external Main module will set this KB to the common KB used by the system.
Madara::Knowledge_Engine::Knowledge_Base* m_sim_knowledge;

// Define the ids for the internal expressions.
enum VRepMadaraExpressionId 
{
    // Expression to send a movement command.
	VE_SEND_MOVE_TO_GPS_COMMAND,

    // Expression to send a move to altitude command.
	VE_SEND_MOVE_TO_ALT_COMMAND,
};

// Map of Madara expressions.
static std::map<VRepMadaraExpressionId, Madara::Knowledge_Engine::Compiled_Expression> m_expressions;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Internal functions.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void compileExpressions(Madara::Knowledge_Engine::Knowledge_Base* knowledge);

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// General Functions.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool init_platform()
{
    //// By default we identify ourselves by the hostname set in our OS.
    //std::string g_host ("");

    //// By default, we use the multicast port 239.255.0.1.:4150
    //const std::string DEFAULT_MULTICAST_ADDRESS ("239.255.0.1:4150");

    //// Used for updating various transport settings
    //Madara::Transport::Settings g_settings;

    //// Define the transport.
    //g_settings.hosts_.resize (1);
    //g_settings.hosts_[0] = DEFAULT_MULTICAST_ADDRESS;
    //g_settings.type = Madara::Transport::MULTICAST;
    //g_settings.id = 1000;
    //
    //// Create the knowledge base.
    //m_knowledge = new Madara::Knowledge_Engine::Knowledge_Base(g_host, g_settings);

    compileExpressions(m_sim_knowledge);

	// Indicate that we have not sent or received replied to commands yet. The first id sent will be 1.
    // NOTE: this is currently not being used for anything other than debugging. It could be used to fix a bug where
    // commands some times do not get sent for some reason to the Madara base in VRep, by checking if no "acks" have
    // beeen recieved from VRep.
    m_sim_knowledge->set(MS_SIM_DEVICES_PREFIX "{.id}" MS_SIM_CMD_SENT_ID, (Madara::Knowledge_Record::Integer) 0,
                Madara::Knowledge_Engine::Eval_Settings(true, true));
    m_sim_knowledge->set(MS_SIM_DEVICES_PREFIX "{.id}" MS_SIM_CMD_RCVD_ID, (Madara::Knowledge_Record::Integer) 0,
                Madara::Knowledge_Engine::Eval_Settings(true, true));

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Compiles all expressions to be used by this class.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void compileExpressions(Madara::Knowledge_Engine::Knowledge_Base* knowledge)
{
    m_expressions[VE_SEND_MOVE_TO_GPS_COMMAND] = knowledge->compile(
        // Send move to latitude and longitude command to VRep.
        "("
			// Send the actual command and its parameters.
            MS_SIM_DEVICES_PREFIX "{.id}" MV_MOVEMENT_REQUESTED "=" MV_MOVEMENT_REQUESTED ";"
            MS_SIM_DEVICES_PREFIX "{.id}" MV_MOVEMENT_TARGET_LAT "=" MV_MOVEMENT_TARGET_LAT ";"
			MS_SIM_DEVICES_PREFIX "{.id}" MV_MOVEMENT_TARGET_LON "=" MV_MOVEMENT_TARGET_LON ";"

			// Send the command id after increasing it. We first increase it so the first id sent is 1.
			"++" MS_SIM_DEVICES_PREFIX "{.id}" MS_SIM_CMD_SENT_ID";"
        ")"
    );

    m_expressions[VE_SEND_MOVE_TO_ALT_COMMAND] = knowledge->compile(
        // Send move to altitude command to VRep.
        "("
			// Send the actual command and its parameters.
            MS_SIM_DEVICES_PREFIX "{.id}" MV_MOVEMENT_REQUESTED "=" MV_MOVEMENT_REQUESTED ";"
            MS_SIM_DEVICES_PREFIX "{.id}" MV_MOVEMENT_TARGET_ALT "=" MV_MOVEMENT_TARGET_ALT ";"

			// Send the command id after increasing it. We first increase it so the first id sent is 1.
			"++" MS_SIM_DEVICES_PREFIX "{.id}" MS_SIM_CMD_SENT_ID";"
        ")"
    );

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Movement Functions.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool init_control_functions()
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void takeoff()
{
	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void land()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move_up()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move_down()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move_left()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move_right()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move_forward()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move_backward()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move_to_location(double lat, double lon, double alt)
{
    // We will assume that lat and lon have already been loaded in the local Madara variables.
    m_sim_knowledge->evaluate(m_expressions[VE_SEND_MOVE_TO_GPS_COMMAND]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move_to_altitude(double alt)
{
    // We will assume that alt has already been loaded in the local Madara variables.
    m_sim_knowledge->evaluate(m_expressions[VE_SEND_MOVE_TO_ALT_COMMAND]);
}

void stop_movement()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sensor Functions.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool init_sensor_functions()
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void read_thermal(double buffer[8][8])
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gets the GPS coordinates from the simulator and gives them back to the 
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void read_gps(struct madara_gps * ret)
{
    // Get the latitude and longitude that the simulator set for this drone, in variables with the sim prefix.
    double latitude = m_sim_knowledge->get(m_sim_knowledge->expand_statement(MS_SIM_PREFIX MV_DEVICE_LAT("{"MV_MY_ID"}"))).to_double();
    double longitude = m_sim_knowledge->get(m_sim_knowledge->expand_statement(MS_SIM_PREFIX MV_DEVICE_LON("{"MV_MY_ID"}"))).to_double();
	double altitude = m_sim_knowledge->get(m_sim_knowledge->expand_statement(MS_SIM_PREFIX MV_DEVICE_ALT("{"MV_MY_ID"}"))).to_double();

    //std::cout << "Lat " << latitude << ", Long " << longitude << " from: " << std::string(MS_SIM_PREFIX MV_DEVICE_LAT("{"MV_MY_ID"}")) << std::endl;
    //m_sim_knowledge->print_knowledge();

    // Set the values in the return structure.
	ret->latitude = latitude;
	ret->longitude = longitude;
	ret->altitude = altitude;
	ret->num_sats = 10;             // Just because it should be really exact with the simulator.
}

double read_ultrasound()
{
    return m_sim_knowledge->get(MV_DEVICE_ALT("{.id}")).to_double();
}

bool cleanup_platform()
{
    return true;
}

/* Calculate the distance between two coordinate pairs */
double gps_coordinates_distance (double lat1, double long1, double lat2, double long2)
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

double get_distance_to_gps(double lat, double lon)
{
    double curLat = m_sim_knowledge->get(MV_DEVICE_LAT("{.id}")).to_double();
    double curLong = m_sim_knowledge->get(MV_DEVICE_LON("{.id}")).to_double();

    return gps_coordinates_distance(curLat, curLong, lat, lon);
}

#endif
