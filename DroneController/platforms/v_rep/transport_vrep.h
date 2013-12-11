/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * transport_vrep.h - Defines variables for the VREP transport.
 *********************************************************************/

#ifndef _TRANSPORT_VREP_H
#define _TRANSPORT_VREP_H

#include <string.h>
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/transport/multicast/Multicast_Transport.h"

// Defines the communication parameters for Multicast.
static std::string MULTICAST_ADDRESS = "239.255.0.1:4150";
static int MULTICAST_QUEUE_LENGTH = 512000;

///////////////////////////////////////////////////////////////////////////////
// Returns a multicast transport configured for VREP.
///////////////////////////////////////////////////////////////////////////////
Madara::Transport::Multicast_Transport* get_vrep_multicast_transport(int id, 
  Madara::Knowledge_Engine::Knowledge_Base* kb)
{
  // Generate a unique host id.
  std::string hostId = kb->setup_unique_hostport();

  // Define basic transport settings.
  Madara::Transport::Settings transportSettings;
  transportSettings.hosts.resize(1);
  transportSettings.delay_launch = true;
  transportSettings.id = id;

  // Set particular transport settings.
  transportSettings.hosts[0] = MULTICAST_ADDRESS;
  transportSettings.queue_length = MULTICAST_QUEUE_LENGTH;

  // Create the actual transport.
  Madara::Transport::Multicast_Transport* transport = 
    new Madara::Transport::Multicast_Transport(hostId,
    kb->get_context(), transportSettings, true);

  return transport;
}

#endif
