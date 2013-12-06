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
#include <vector>

// Defines the communication parameters for Broadcast.
static std::string BROADCAST_ADDRESS = "192.168.1.255:15000";
static int BROADCST_QUEUE_LENGTH = 1024;

///////////////////////////////////////////////////////////////////////////////
// Sets up a knowledge base.
///////////////////////////////////////////////////////////////////////////////
void setup_knowledge_base(Madara::Knowledge_Engine::Knowledge_Base* knowledge, 
  std::vector<Madara::Transport::Base*> transports, int id, bool enableLog)
{
    // Setup a log for Madara.
    if(enableLog)
    {
        std::stringstream stream;
        stream << id;
        Madara::Knowledge_Engine::Knowledge_Base::log_to_file(
          std::string("madara_id_" + stream.str() + "_log.txt").c_str(), false);
    }
    knowledge->print ("Knowledge log set up.\n");

    // Sets a double precision of 10.
    Madara::Knowledge_Record::set_precision(10);

    // Attach and activate the transports.
    for(unsigned int i=0; i<transports.size(); i++)
    {
      knowledge->attach_transport(transports[i]);
    }
    knowledge->activate_transport();
}
