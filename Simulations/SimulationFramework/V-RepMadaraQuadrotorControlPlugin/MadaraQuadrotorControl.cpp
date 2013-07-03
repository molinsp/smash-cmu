/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "MadaraQuadrotorControl.h"
#include "utilities/CommonMadaraVariables.h"

#include <vector>
using std::vector;
#include <string>
using std::string;

// Multicast address.
#define DEFAULT_MULTICAST_ADDRESS "239.255.0.1:4150"

// Constructor, sets up a Madara knowledge base and basic values.
MadaraQuadrotorControl::MadaraQuadrotorControl()
{
  // Define the transport settings.
  Madara::Transport::Settings settings;
  settings.hosts_.resize(1);
  settings.hosts_[0] = DEFAULT_MULTICAST_ADDRESS;
  settings.type = Madara::Transport::MULTICAST;

  // Create the knowledge base.
  m_knowledge = new Madara::Knowledge_Engine::Knowledge_Base("", settings);
}

// Destructor, simply cleans up.
MadaraQuadrotorControl::~MadaraQuadrotorControl()
{
  terminate();
}

// Cleanup, terminating all threads and open communications.
void MadaraQuadrotorControl::terminate()
{
  if(m_knowledge != NULL)
  {
    m_knowledge->close_transport();
    m_knowledge->clear();
    delete m_knowledge;
    m_knowledge = NULL;
  }
}

void MadaraQuadrotorControl::updateQuadrotorPosition(const int& id, const double& x,
  const double& y, const double& z) // need to update for altitude
{
  // update the location of this drone (this would be done by its sensors).
  string droneIdString = std::to_string(static_cast<long long>(id));
  m_knowledge->set(MS_SIM_PREFIX MV_DEVICE_LAT(droneIdString), x,
    Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);
  m_knowledge->set(MS_SIM_PREFIX MV_DEVICE_LON(droneIdString), y,
    Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);
  m_knowledge->set(MS_SIM_PREFIX MV_DEVICE_ALT(droneIdString), z,
    Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);
}

// Updates the status of the drones in Madara.
void MadaraQuadrotorControl::updateQuadrotorStatus(const Status& s)
{
    // Need to update for altitude.
    updateQuadrotorPosition(s.m_id, s.m_loc.m_lat, s.m_loc.m_long, s.m_loc.m_alt);
}

// Gets a target position where a drone should move to.
MadaraQuadrotorControl::Command* MadaraQuadrotorControl::getNewCommand(
  int droneId)
{
    // Check for command.
    string droneIdString = std::to_string(static_cast<long long>(droneId));
    string commandStr =
    m_knowledge->get(MS_SIM_DEVICES_PREFIX + droneIdString +
        MV_MOVEMENT_REQUESTED).to_string();
    if(commandStr == "0")
        return NULL;

    // create the movement command to return
    Command* command = new Command();
    command->m_command = commandStr;

    // Depending on the command, we may need to get more parameters.
    if(commandStr == MO_MOVE_TO_GPS_CMD)
    {
        // Move to certain location command; get the target location.
        double targetPosX = m_knowledge->get(MS_SIM_DEVICES_PREFIX + droneIdString +
            MV_MOVEMENT_TARGET_LAT).to_double();
        double targetPosY = m_knowledge->get(MS_SIM_DEVICES_PREFIX + droneIdString +
            MV_MOVEMENT_TARGET_LON).to_double();
        double targetPosZ = m_knowledge->get(MS_SIM_DEVICES_PREFIX + droneIdString +
            MV_MOVEMENT_TARGET_ALT).to_double();
        Location targetLocation = Location(targetPosX, targetPosY, targetPosZ);
        command->m_loc = targetLocation;
    }

    // Set the command as 0 locally, to indicate that we already read it.
    m_knowledge->set(MS_SIM_DEVICES_PREFIX + droneIdString +
        MV_MOVEMENT_REQUESTED, "0",
    Madara::Knowledge_Engine::TREAT_AS_LOCAL_EVAL_SETTINGS);

    return command;
}
