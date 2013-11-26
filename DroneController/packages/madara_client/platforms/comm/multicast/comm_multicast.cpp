/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * comm_multicast.h - Implements the communication interface using the
 * default multicast transport.
 *********************************************************************/

#include "platforms/comm/comm.h"
#include "platforms/comm/kb_setup.h"

// Defines the IP through which devices on the network will communicate with each other.
static std::string MULTICAST_ADDRESS = "239.255.0.1:4150";

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sets up the knowledge base for this transport.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Engine::Knowledge_Base* comm_setup_knowledge_base(int id, bool enableLog)
{
    // Define the transport.
    Madara::Transport::Settings transportSettings;
    transportSettings.hosts.resize (1);
    transportSettings.hosts[0] = MULTICAST_ADDRESS;
    transportSettings.queue_length = 512000;
    transportSettings.delay_launch = true;
    transportSettings.type = Madara::Transport::MULTICAST;
   
    // Create the knowledge base.
    Madara::Knowledge_Engine::Knowledge_Base* knowledge = setup_knowledge_base(id, enableLog, transportSettings);

    // Activate the transport.
    knowledge->activate_transport();

    return knowledge;
}
