/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * comm_broadcast.h - Implements the communication interface using the
 * default broadcast transport appropriate for communication for drones.
 *********************************************************************/

#include "platforms/comm/comm.h"
#include "platforms/comm/kb_setup.h"

// Defines the IP through which devices on the network will communicate with each other.
static std::string BROADCAST_ADDRESS = "192.168.1.255:15000";

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sets up the knowledge base for this transport.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Engine::Knowledge_Base* comm_setup_knowledge_base(int id, bool enableLog)
{
    // Define the transport.
    Madara::Transport::Settings transportSettings;
    transportSettings.hosts.resize (1);
    transportSettings.hosts[0] = BROADCAST_ADDRESS;
    transportSettings.type = Madara::Transport::BROADCAST;
    transportSettings.queue_length = 1024;                   //Smaller queue len to preserve memory
    transportSettings.delay_launch = true;
   
    // Create the knowledge base.
    Madara::Knowledge_Engine::Knowledge_Base* knowledge = setup_knowledge_base(id, enableLog, transportSettings);

    // Activate the transport.
    knowledge->activate_transport();

    return knowledge;
}
