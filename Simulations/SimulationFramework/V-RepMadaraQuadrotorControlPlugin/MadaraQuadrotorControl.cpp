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
#include <utilities\Position.h>

#ifdef _WIN32
  // Only include the custom transport in Windows, as it is not necessary in Linux.
  #include "Windows_Multicast_Transport.h"
#endif

// Multicast address.
#define DEFAULT_MULTICAST_ADDRESS "239.255.0.1:4150"

// Constructor, sets up a Madara knowledge base and basic values.
MadaraQuadrotorControl::MadaraQuadrotorControl(int droneId)
{
    // Control id is derived from the droneId. But it has to be different to ensure different ids inside Madara.
    int transportId = droneId + 100;

    // Define the transport settings.
    Madara::Transport::Settings settings;
    settings.hosts_.resize(1);
    settings.hosts_[0] = DEFAULT_MULTICAST_ADDRESS;
    settings.id = transportId;

    // Setup the actual transport.
#ifdef __linux
    // In Linux we can use the default Mulitcast transport.
    settings.type = Madara::Transport::MULTICAST;
#elif defined(WIN32)
    // In Windows we need to delay the transport launch to use a custom transport.
    settings.delay_launch = true;
#endif

    // Create the knowledge base.
    m_knowledge = new Madara::Knowledge_Engine::Knowledge_Base("", settings);

    // Setup a log.
    m_knowledge->log_to_file("madaralog.txt", true);
    m_knowledge->evaluate("#log_level(1)");

#ifdef _WIN32
    // In Windows we need a custom transport to avoid crashes due to incompatibilities between Win V-Rep and ACE.
    m_knowledge->attach_transport(new Windows_Multicast_Transport (m_knowledge->get_id (),
        m_knowledge->get_context (), settings, true));
#endif
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

void MadaraQuadrotorControl::updateQuadrotorPosition(const int& id, const double& lat,
  const double& lon, const double& z) // need to update for altitude
{
  // update the location of this drone (this would be done by its sensors).
  // NOTE: we are storing the cell location as a string instead of doubles to ensure we have enough precision, since Madara,
  // as of version 0.9.44, has only 6 digits of precision for doubles (usually 4 decimals for latitudes and longitudes).
  string droneIdString = std::to_string(static_cast<long long>(id));
  m_knowledge->set(MS_SIM_PREFIX MV_DEVICE_LAT(droneIdString), NUM_TO_STR(lat),
    Madara::Knowledge_Engine::Eval_Settings(true));
  m_knowledge->set(MS_SIM_PREFIX MV_DEVICE_LON(droneIdString), NUM_TO_STR(lon),
    Madara::Knowledge_Engine::Eval_Settings(true));
  m_knowledge->set(MS_SIM_PREFIX MV_DEVICE_ALT(droneIdString), NUM_TO_STR(z));

  m_knowledge->print_knowledge(1);
}

// Updates the status of the drones in Madara.
void MadaraQuadrotorControl::updateQuadrotorStatus(const Status& s)
{
    // Need to update for altitude.
    updateQuadrotorPosition(s.m_id, s.m_loc.m_lat, s.m_loc.m_long, s.m_loc.m_alt);
}

// Gets a target position where a drone should move to.
MadaraQuadrotorControl::Command* MadaraQuadrotorControl::getNewCommand(int droneId)
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
        double targetPosLat = m_knowledge->get(MS_SIM_DEVICES_PREFIX + droneIdString +
            MV_MOVEMENT_TARGET_LAT).to_double();
        double targetPosLon = m_knowledge->get(MS_SIM_DEVICES_PREFIX + droneIdString +
            MV_MOVEMENT_TARGET_LON).to_double();
        double targetPosZ = m_knowledge->get(MS_SIM_DEVICES_PREFIX + droneIdString +
            MV_MOVEMENT_TARGET_ALT).to_double();
        Location targetLocation = Location(targetPosLat, targetPosLon , targetPosZ);
        command->m_loc = targetLocation;
    }

    // Set the command as 0 locally, to indicate that we already read it.
    m_knowledge->set(MS_SIM_DEVICES_PREFIX + droneIdString +
        MV_MOVEMENT_REQUESTED, "0",
    Madara::Knowledge_Engine::Eval_Settings(false, true));

    return command;
}
