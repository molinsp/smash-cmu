/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

#include "MadaraQuadrotorControl.h"
#include "platforms/v_rep/sim_kb_setup.h"
#include "platforms/v_rep/platform_vrep_madara_variables.h"
#include "utilities/Position.h"
#include "utilities/string_utils.h"

using std::vector;
using std::string;
using namespace SMASHSim;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor, sets up a Madara knowledge base and basic values.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
MadaraQuadrotorControl::MadaraQuadrotorControl(int droneId)
{
    // Initializes this to one, as for now we have only one drone using this controller.
    numDrones = 1;

    // Control id is derived from the droneId. But it has to be different to it to ensure different ids inside this domain.
    int transportId = droneId + 100;

    // Get a proper simulation knowledge base.
    m_knowledge = sim_setup_knowledge_base(transportId, true);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Initializes Madara variables for communicating with the drone platform.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MadaraQuadrotorControl::initInternalData(int droneId)
{
    // Initialize the internal command variable so that we start with no commands.
    string droneIdString = NUM_TO_STR(droneId);
    clearCommand(droneIdString);

    // Indicate that we have not received or replied to commands yet.
    m_knowledge->set(MS_SIM_DEVICES_PREFIX + droneIdString + MS_SIM_CMD_SENT_ID, (Madara::Knowledge_Record::Integer) 0,
                Madara::Knowledge_Engine::Eval_Settings(true, true));
    m_knowledge->set(MS_SIM_DEVICES_PREFIX + droneIdString + MS_SIM_CMD_RCVD_ID, (Madara::Knowledge_Record::Integer) 0,
                Madara::Knowledge_Engine::Eval_Settings(true, true));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Destructor, simply cleans up.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
MadaraQuadrotorControl::~MadaraQuadrotorControl()
{    
    sim_cleanup_knowledge_base(m_knowledge);
    m_knowledge = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Cleanup, terminating all threads and open communications.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool MadaraQuadrotorControl::terminate()
{
    if(m_knowledge != NULL)
    {
        bool otherDronesStillUsingPlugin = numDrones > 0;
        if(otherDronesStillUsingPlugin)
        {
            // If there are other simulated drones still using the plugin, we won't terminate and cleanup yet.
            MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
              DLINFO "MadaraQuadrotorControl::terminate:" \
              "Not terminating knowledge base, controller still in use by other %d drones.\n", numDrones));
        }
        else
        {
            sim_cleanup_knowledge_base(m_knowledge);
            m_knowledge = NULL;
            return true;
        }
    }

    // We didn't really do any cleanup if we got here.
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MadaraQuadrotorControl::updateQuadrotorPosition(const int& id, const Location& location)
{
    // update the location of this drone (this would be done by its sensors).
    string droneIdString = NUM_TO_STR(id);
    if(m_knowledge != NULL)
    {
        m_knowledge->set(MS_SIM_DEVICES_PREFIX + droneIdString + MV_LATITUDE, (location.latAndLong.latitude),
            Madara::Knowledge_Engine::Eval_Settings(true));
        m_knowledge->set(MS_SIM_DEVICES_PREFIX + droneIdString + MV_LONGITUDE, (location.latAndLong.longitude),
            Madara::Knowledge_Engine::Eval_Settings(true));
        m_knowledge->set(MS_SIM_DEVICES_PREFIX + droneIdString + MV_ALTITUDE, (location.altitude),
            Madara::Knowledge_Engine::Eval_Settings(true));

        m_knowledge->send_modifieds();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gets a target position where a drone should move to.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
MadaraQuadrotorControl::Command* MadaraQuadrotorControl::getNewCommand(int droneId)
{
    string droneIdString = NUM_TO_STR(droneId);

    // Check if commands have started being received for the requested drone.
    int recievedCommandId = (int) m_knowledge->get(MS_SIM_DEVICES_PREFIX + droneIdString + MS_SIM_CMD_SENT_ID).to_integer();
    if(recievedCommandId == 0)
        return NULL;
    
    // Get the actual command, if there is a new one.
    string commandStr = m_knowledge->get(MS_SIM_DEVICES_PREFIX + droneIdString + MV_MOVEMENT_REQUESTED).to_string();
    if(commandStr == "0")
        return NULL;

    // Create the movement command to return.
    Command* command = new Command();
    command->m_command = commandStr;

    // Depending on the command, we may need to get more parameters.
    if(commandStr == MO_MOVE_TO_GPS_CMD)
    {
        // Move to certain location command; get the target location.
        double targetPosLat = m_knowledge->get(MS_SIM_DEVICES_PREFIX + droneIdString + MV_MOVEMENT_CMD_ARG("0")).to_double();
        double targetPosLon = m_knowledge->get(MS_SIM_DEVICES_PREFIX + droneIdString + MV_MOVEMENT_CMD_ARG("1")).to_double();

        // We don't care about alt here.
        Location targetLocation = Location(targetPosLat, targetPosLon, 0);
        command->m_loc = targetLocation;
    }
    else if(commandStr == MO_MOVE_TO_ALTITUDE_CMD)
    {
        double targetAltitude = m_knowledge->get(MS_SIM_DEVICES_PREFIX + droneIdString + MV_MOVEMENT_CMD_ARG("0")).to_double();

        // We don't care about lat and long here.
        Location targetLocation = Location(0, 0, targetAltitude);
        command->m_loc = targetLocation;
    }

    // Set the command as 0 locally, to indicate that we already read it.
    clearCommand(droneIdString);

    // Indicate that this is the last command we have received.
    int lastSentCmdId = (int) m_knowledge->get(MS_SIM_DEVICES_PREFIX + droneIdString + MS_SIM_CMD_SENT_ID).to_integer();
    m_knowledge->set(MS_SIM_DEVICES_PREFIX + droneIdString + MS_SIM_CMD_RCVD_ID, (Madara::Knowledge_Record::Integer) lastSentCmdId,
        Madara::Knowledge_Engine::Eval_Settings(false, false));

    return command;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Set the command as 0 locally, to indicate that we already read it.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MadaraQuadrotorControl::clearCommand(std::string droneIdString)
{
    m_knowledge->set(MS_SIM_DEVICES_PREFIX + droneIdString + MV_MOVEMENT_REQUESTED, "0",
        Madara::Knowledge_Engine::Eval_Settings(true, true));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MadaraQuadrotorControl::setNewThermalScan(int droneId, std::string thermalBuffer, int thermalRows, int thermalColumns)
{
    if(m_knowledge != NULL)
    {
        std::stringstream thermalBufferName;
        thermalBufferName << MS_SIM_DEVICES_PREFIX << droneId << MV_SIM_THERMAL_BUFFER;
        m_knowledge->set(thermalBufferName.str(), thermalBuffer);
    }
}
