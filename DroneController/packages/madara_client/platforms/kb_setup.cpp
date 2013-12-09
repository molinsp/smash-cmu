/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * kb_setup.cpp - Implements setting up a knowledge base.
 *********************************************************************/

#include "kb_setup.h"
#include <string>
#include <sstream>

// Defines the communication parameters for Multicast.
static std::string MULTICAST_ADDRESS = "239.255.0.1:4150";
static int MULTICAST_QUEUE_LENGTH = 512000;

// Defines the communication parameters for Broadcast.
static std::string BROADCAST_ADDRESS = "192.168.1.255:15000";
static int BROADCST_QUEUE_LENGTH = 1024;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sets up a knowledge base.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Engine::Knowledge_Base* setup_knowledge_base(int id, 
  bool enableLog, Madara::Transport::Types transportType)
{
    // Define the transport.
    Madara::Transport::Settings transportSettings;
    transportSettings.hosts.resize (1);
    transportSettings.delay_launch = true;

    // Setup transport depending on transport type.
    if(transportType == Madara::Transport::MULTICAST)
    {
      transportSettings.type = Madara::Transport::MULTICAST;
      transportSettings.hosts[0] = MULTICAST_ADDRESS;
      transportSettings.queue_length = MULTICAST_QUEUE_LENGTH;
    }
    else if(transportType == Madara::Transport::BROADCAST)
    {
      transportSettings.type = Madara::Transport::BROADCAST;
      transportSettings.hosts[0] = BROADCAST_ADDRESS;
      transportSettings.queue_length = BROADCST_QUEUE_LENGTH;
    }

    // Set the transport id as the given id.
    transportSettings.id = id;
    
    // Setup a log for Madara.
    if(enableLog)
    {
        std::stringstream stream;
        stream << id;
        Madara::Knowledge_Engine::Knowledge_Base::log_to_file(std::string("madara_id_" + stream.str() + "_log.txt").c_str(), false);
    }

    // Name the host based on the id.
    char host[30];
    sprintf(host, "device%d", id);
    
    // Create the knowledge base.
    Madara::Knowledge_Engine::Knowledge_Base* knowledge = new Madara::Knowledge_Engine::Knowledge_Base(host, transportSettings);
    Madara::Knowledge_Record::set_precision(10);
    knowledge->print ("Knowledge base created.\n");

    // Activate the transport.
    knowledge->activate_transport();

    return knowledge;
}
