/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "MadaraController.h"

#include "Custom_Transport.h"

#include <vector>

#define DEFAULT_MULTICAST_ADDRESS "239.255.0.1:4150"

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Madara Variable Definitions
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Controller variables.
#define MV_MY_ID					".id"								    /* The id of this drone. */
#define MV_CONTROLLER_POSX(i)   	"controller" + SSTR(i) + ".pos.x"       /* The x position of controller with ID i, in meters. */
#define MV_CONTROLLER_POSY(i)   	"controller" + SSTR(i) + ".pos.y"       /* The x position of controller with ID i, in meters. */

// General drone variables.
#define MV_TOTAL_DRONES				"controller.max_drones"					/* The total amount of drones in the system. */
#define MV_COMM_RANGE				"controller.high_bw_comm_range"			/* The range of the high-banwidth radio, in meters. */

// Individual drone variables.
#define MV_DRONE_POSX(i)			"drone" + SSTR(i) + ".pos.x"			/* The x position of a drone with ID i, in meters. */
#define MV_DRONE_POSY(i)			"drone" + SSTR(i) + ".pos.y"			/* The y position of a drone with ID i, in meters. */
#define MV_MOBILE(i)				"drone" + SSTR(i) + ".mobile"			/* True of drone with ID i is flying and available for bridging. */

// Bridge request variables,
#define MV_USER_BRIDGE_REQUEST_ID	"user_bridge_request.request_id"		/* Identifies a unique bridge request, along with its bridge. */
#define MV_USER_BRIDGE_REQUEST_ON	"user_bridge_request.enabled"			/* Being true triggers the bridge behavior. */
#define MV_BRIDGE_SOURCE_ID			"user_bridge_request.source_id"			/* Contains the ID of the drone acting as source. */
#define MV_BRIDGE_SINK_ID			"user_bridge_request.sink_id"			/* Containts the ID of the controller acting as sink.*/

// Bridge input from drones.
#define MV_BRIDGING(i)				"drone" + SSTR(i) + ".bridging"		    /* True if drone with ID i is bridging. */
#define MV_DRONE_TARGET_POSX(i)		"drone" + SSTR(i) + ".target_pos.x"	    /* The x target position of a drone with ID i, where it should head to, in meters. */
#define MV_DRONE_TARGET_POSY(i)		"drone" + SSTR(i) + ".target_pos.y"	    /* The y target position of a drone with ID i, where it should head to, in meters. */

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor, sets up a Madara knowledge base and basic values.
////////////////////////////////////////////////////////////////////////////////////////////////////////
MadaraController::MadaraController(Madara::Knowledge_Engine::Knowledge_Base* knowledge, int id, double commRange)
{
    // Set the knowledge base.
    m_knowledge = knowledge;
    
    // Set our id and comm range.
    m_id = id;
    m_knowledge->set (MV_MY_ID, (Madara::Knowledge_Record::Integer) m_id);
    m_commRange = commRange;
    m_knowledge->set (MV_COMM_RANGE, m_commRange);

    //m_knowledge->print_knowledge();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Destructor, simply cleans up.
////////////////////////////////////////////////////////////////////////////////////////////////////////
MadaraController::~MadaraController()
{
    terminate();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor, sets up a Madara knowledge base and basic values.
////////////////////////////////////////////////////////////////////////////////////////////////////////
MadaraController::MadaraController(int id, double commRange)
{
    // Define the transport.
    m_host = "";
    m_transportSettings.hosts_.resize (1);
    m_transportSettings.hosts_[0] = DEFAULT_MULTICAST_ADDRESS;
    //m_transportSettings.type = Madara::Transport::MULTICAST;
    m_transportSettings.delay_launch = true;
    m_transportSettings.id = id;

    // Create the knowledge base.
    m_knowledge = new Madara::Knowledge_Engine::Knowledge_Base(m_host, m_transportSettings);

    //m_knowledge->log_to_file("madaralog.txt", true);
    //m_knowledge->evaluate("#log_level(10)");

    // Add the actual transport.
    m_knowledge->attach_transport(new Custom_Transport (m_knowledge->get_id (),
        m_knowledge->get_context (), m_transportSettings, true));
   
    // Set our id and comm range.
    m_id = id;
    m_knowledge->set (MV_MY_ID, (Madara::Knowledge_Record::Integer) m_id);
    m_commRange = commRange;
    m_knowledge->set (MV_COMM_RANGE, m_commRange);

    //m_knowledge->print_knowledge();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sets up all the variables required for a bridge request.
////////////////////////////////////////////////////////////////////////////////////////////////////////
void MadaraController::setupBridgeRequest(int requestId, int sourceId)
{
    // Simulate the sink actually sending the command to bridge.
    m_knowledge->set(MV_USER_BRIDGE_REQUEST_ID, (Madara::Knowledge_Record::Integer) requestId, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);
    m_knowledge->set(MV_BRIDGE_SOURCE_ID, (Madara::Knowledge_Record::Integer) sourceId, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);
    m_knowledge->set(MV_BRIDGE_SINK_ID, (Madara::Knowledge_Record::Integer) m_id, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);

    // This call has no delay to flush all past changes.
    m_knowledge->set(MV_USER_BRIDGE_REQUEST_ON, 1.0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sets a drone as stopped.
////////////////////////////////////////////////////////////////////////////////////////////////////////
void MadaraController::stopDrone(int droneId)
{
    // Simulate the sink actually sending the command to bridge.
    m_knowledge->set(MV_MOBILE(droneId), (Madara::Knowledge_Record::Integer) 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Convenience method that updates both the status of this controller, as well as the status of the
// drone with the information form the simulator.
////////////////////////////////////////////////////////////////////////////////////////////////////////
void MadaraController::updateNetworkStatus(double controllerPosx, double controllerPosy, std::vector<DroneStatus> droneStatusList)
{
    // Updates status about myself and the drones with info form the simulation.
    updateMyStatus(controllerPosx, controllerPosy);
    updateDroneStatus(droneStatusList);

    // This is done just to ensure this is propagated, since we are just setting this value to the same value it already has.
    m_knowledge->set (MV_COMM_RANGE, m_commRange, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);

    // This call has no delay to flush all past changes, and updates the total of drones in the system.
    m_knowledge->set(MV_TOTAL_DRONES, (Madara::Knowledge_Record::Integer) droneStatusList.size());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Updates the status of the controller in Madara.
////////////////////////////////////////////////////////////////////////////////////////////////////////
void MadaraController::updateMyStatus(double posx, double posy)
{
    m_knowledge->set(MV_CONTROLLER_POSX(m_id), posx, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);
    m_knowledge->set(MV_CONTROLLER_POSY(m_id), posy, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Updates the status of the drones in Madara. This should be done in each drone, but since V-Rep is
// simulating them, it has to come from the controller, which is at the simulator.
////////////////////////////////////////////////////////////////////////////////////////////////////////
void MadaraController::updateDroneStatus(std::vector<DroneStatus> droneStatusList)
{
    // Update the status of all drones.
    for (std::vector<DroneStatus>::iterator it = droneStatusList.begin() ; it != droneStatusList.end(); ++it)
    {
        // Update the overall status of this drone.
        m_knowledge->set(MV_DRONE_POSX(it->id), it->posx, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);
        m_knowledge->set(MV_DRONE_POSY(it->id), it->posy, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);
        //if(it->flying)
        //    m_knowledge->set(MV_MOBILE(it->id), 1.0, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);
        //else
        //    m_knowledge->set(MV_MOBILE(it->id), 0.0, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gets the position in the bridge where the drone with id droneId should go to, if any, or NULL if the
// drone is not part of the bridge.
////////////////////////////////////////////////////////////////////////////////////////////////////////
Position* MadaraController::getBridgePosition(int droneId)
{
    int isDroneInBridge = (int) m_knowledge->get(MV_BRIDGING(droneId)).to_integer();
    if(isDroneInBridge == 0)
    {
        // No position to return since drone has not joined the bridge process.
        return NULL;
    }

    // Get the targeted positions this drone want to go to in the bridge.
    double targetPosX = m_knowledge->get(MV_DRONE_TARGET_POSX(droneId)).to_double();
    double targetPosY = m_knowledge->get(MV_DRONE_TARGET_POSY(droneId)).to_double();

    // Return it as a position object.
    Position* targetPosition = new Position(targetPosX, targetPosY);
    return targetPosition;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Cleanup, terminating all threads and open communications.
////////////////////////////////////////////////////////////////////////////////////////////////////////
void  MadaraController::terminate()
{
    if(m_knowledge != NULL)
    {
        m_knowledge->close_transport();
        m_knowledge->clear();
        delete m_knowledge;
        m_knowledge = NULL;
    }
}