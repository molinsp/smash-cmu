/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#include "platforms/platform.h"
#include "movement/platform_movement.h"
#include "sensors/platform_sensors.h"

#include "madara/knowledge_engine/Knowledge_Base.h"

#include "v-rep_madara_variables.h"
#include "v-rep_main_madara_transport_settings.h"
#include "utilities/Position.h"
#include "utilities/string_utils.h"

#include <string>
#include <cmath>
#include <map>

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Macros, constants and enums.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Define the ids for the internal expressions.
enum VRepMadaraExpressionId 
{
    // Updated the command id.
    VE_UPDATE_COMMAND_ID, 
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Variables.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Map of Madara expressions used when simulating the hardware.
static std::map<VRepMadaraExpressionId, Madara::Knowledge_Engine::Compiled_Expression> m_expressions;

// The knowledge base used to simulate the hardware.
static Madara::Knowledge_Engine::Knowledge_Base*m_sim_knowledge;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Internal functions.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void compileExpressions(Madara::Knowledge_Engine::Knowledge_Base* knowledge);
static void setupInternalHardwareKnowledgeBase(int id);

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Overrides: init_platform().
////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool platform_init()
{
    // The actual initialization is postponed until the setup_knowledge_base function is called,
    // since only then the id is received.
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Overrides: setup_knowledge_base().
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Engine::Knowledge_Base* platform_setup_knowledge_base(int id)
{
    // Setup the internal, totally separate knowledge base to be used as a proxy for the hardware.
    setupInternalHardwareKnowledgeBase(id);

    // Define the transport.
    Madara::Transport::Settings g_settings;
    g_settings.hosts_.resize (1);
    g_settings.hosts_[0] = MAIN_MULTICAST_ADDRESS;
    g_settings.type = Madara::Transport::MULTICAST;
    g_settings.queue_length = SIMULATION_TRANSPORT_QUEUE_LENGTH;

    // Set the transport id as the given id.
    g_settings.id = id;
    
    // Setup a log for Madara.
    //Madara::Knowledge_Engine::Knowledge_Base::log_level(10);
    //Madara::Knowledge_Engine::Knowledge_Base::log_to_file(std::string("dronemadaralog" + NUM_TO_STR(id) + ".txt").c_str(), false);
    
    // Create the knowledge base.
    std::string g_host ("");
    Madara::Knowledge_Engine::Knowledge_Base* knowledge = new Madara::Knowledge_Engine::Knowledge_Base(g_host, g_settings);
    Madara::Knowledge_Record::set_precision(10);

    return knowledge;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Overrides: cleanup_platform().
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool platform_cleanup()
{
    m_sim_knowledge->print_knowledge();

	// Cleanup the internal Madara platform.
	m_sim_knowledge->close_transport();
    m_sim_knowledge->clear();
    delete m_sim_knowledge;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sets up a knowledge base that will be used as a proxy to send comands to simulated hardware, and to receive
// sensed data from simulated sensors.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void setupInternalHardwareKnowledgeBase(int id)
{
    // Used for updating various transport settings
    Madara::Transport::Settings transportSettings;

    // Define the transport.
    transportSettings.hosts_.resize (1);
    transportSettings.hosts_[0] = SIMULATED_HW_MULTICAST_ADDRESS;
    transportSettings.type = Madara::Transport::MULTICAST;
    transportSettings.domains = VREP_DOMAIN;

    // Sets the id. NOTE: we are assuming that g_id will be setup externally by the code.
    transportSettings.id = id;

    // Setup a log for Madara.
    //Madara::Knowledge_Engine::Knowledge_Base::log_level(10);
    //Madara::Knowledge_Engine::Knowledge_Base::log_to_file(std::string("simhwmadaralog" + NUM_TO_STR(id) + ".txt").c_str(), false);
    
    // Create the knowledge base.
    std::string host = "";
    m_sim_knowledge = new Madara::Knowledge_Engine::Knowledge_Base(host, transportSettings);

    // Define Madara functions.
    compileExpressions(m_sim_knowledge);

    // Set the ID inside Madara.
    m_sim_knowledge->set (".id", (Madara::Knowledge_Record::Integer) transportSettings.id);

	// Indicate that we have not sent or received replied to commands yet. The first id sent will be 1.
    // NOTE: this is currently not being used for anything other than debugging. It could be used to fix a bug where
    // commands some times do not get sent for some reason to the Madara base in VRep, by checking if no "acks" have
    // beeen recieved from VRep.
    m_sim_knowledge->set(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MS_SIM_CMD_SENT_ID), 
                (Madara::Knowledge_Record::Integer) 0,
                Madara::Knowledge_Engine::Eval_Settings(true, true));
    m_sim_knowledge->set(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MS_SIM_CMD_RCVD_ID), 
                (Madara::Knowledge_Record::Integer) 0,
                Madara::Knowledge_Engine::Eval_Settings(true, true));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers functions with Madara.
// ASSUMPTION: Drone IDs are continuous, starting from 0.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void compileExpressions(Madara::Knowledge_Engine::Knowledge_Base* knowledge)
{
    m_expressions[VE_UPDATE_COMMAND_ID] = knowledge->compile(
        "("
			// Send the command id after increasing it. We first increase it so the first id sent is 1.
			"++" MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MS_SIM_CMD_SENT_ID";"
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
    // Send the command.
    m_sim_knowledge->set(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MV_MOVEMENT_REQUESTED), MO_TAKEOFF_CMD);

    // Update the command id.
    m_sim_knowledge->evaluate(m_expressions[VE_UPDATE_COMMAND_ID]);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void land()
{
    // Send the command.
    m_sim_knowledge->set(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MV_MOVEMENT_REQUESTED), MO_LAND_CMD);

    // Update the command id.
    m_sim_knowledge->evaluate(m_expressions[VE_UPDATE_COMMAND_ID]);
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
    // Set the arguments for this command. Note that we are intentionally ignoring altitude, as the 
    // simulation is doing that as of now.
    m_sim_knowledge->set(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MV_MOVEMENT_CMD_ARG("0")), lat);
    m_sim_knowledge->set(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MV_MOVEMENT_CMD_ARG("1")), lon);

    // Send the command.
    m_sim_knowledge->set(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MV_MOVEMENT_REQUESTED), MO_MOVE_TO_GPS_CMD);

    // Update the command id.
    m_sim_knowledge->evaluate(m_expressions[VE_UPDATE_COMMAND_ID]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move_to_altitude(double alt)
{
    // Set the arguments for this command. Note that we are intentionally ignoring altitude, as the 
    // simulation is doing that as of now.
    m_sim_knowledge->set(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MV_MOVEMENT_CMD_ARG("0")), alt);

    // Send the command.
    m_sim_knowledge->set(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MV_MOVEMENT_REQUESTED), MO_MOVE_TO_ALTITUDE_CMD);

    // Update the command id.
    m_sim_knowledge->evaluate(m_expressions[VE_UPDATE_COMMAND_ID]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
// Loads information from the Madara thermal variables into the buffer given as a parameter.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void read_thermal(double buffer[8][8])
{
    int numRows = 8;
    int numColumns = 8;

    // Initialize the buffer with zeros.
    memset(buffer, 0, numRows*numColumns);

    // Get the thermal string from the knowledge base.
    std::stringstream thermalBufferName;
    thermalBufferName << MS_SIM_DEVICES_PREFIX << "{" MV_MY_ID "}" << MV_THERMAL_BUFFER;
    std::string thermalValues = m_sim_knowledge->get(m_sim_knowledge->expand_statement(thermalBufferName.str())).to_string();

	// Parse the thermal values.
	std::vector<std::string> thermalValueList = stringSplit(thermalValues, ',');
		
    // Get the thermals from the string, if any.
    if(thermalValueList.size() >= (unsigned) numRows*numColumns)
    {
        for(int row=0; row<numRows; row++)
        {
            // Loop over this row.
            for(int col=0; col<numColumns; col++)
            {
                // Get the current value from the parsed list.
                std::string currValue = std::string(thermalValueList[row*numColumns + col]);

                // Store the current value as a double in the buffer.
                buffer[row][col] = atof(currValue.c_str());
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gets the GPS coordinates from the simulator and gives them back to the 
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void read_gps(struct madara_gps * ret)
{
    // Get the latitude and longitude that the simulator set for this drone, in variables with the sim prefix.
    double latitude = m_sim_knowledge->get(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MV_LATITUDE)).to_double();
    double longitude = m_sim_knowledge->get(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MV_LONGITUDE)).to_double();
	double altitude = m_sim_knowledge->get(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MV_ALTITUDE)).to_double();

    //std::cout << "Lat " << latitude << ", Long " << longitude << " from: " << std::string(MS_SIM_PREFIX MV_DEVICE_LAT("{"MV_MY_ID"}")) << std::endl;
    //m_sim_knowledge->print_knowledge();

    // Set the values in the return structure.
	ret->latitude = latitude;
	ret->longitude = longitude;
	ret->altitude = altitude;
	ret->num_sats = 10;             // Just because it should be really exact with the simulator.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gets the altitude from the ultrasound sensor.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
double read_ultrasound()
{
    double currHeight = m_sim_knowledge->get(m_sim_knowledge->expand_statement(MS_SIM_DEVICES_PREFIX "{" MV_MY_ID "}" MV_ALTITUDE)).to_double();
    return currHeight;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gets the accuracy of the GPS for this platform, in meters.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
double get_gps_accuracy()
{
    return 0.3;
}
