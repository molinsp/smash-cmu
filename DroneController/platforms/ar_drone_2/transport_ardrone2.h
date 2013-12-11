/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * transport_ardrone2.h - Defines variables for the VREP transport.
 *********************************************************************/

#ifndef _TRANSPORT_ARDRONE2_H
#define _TRANSPORT_ARDRONE2_H

#include <string.h>
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/transport/broadcast/Broadcast_Transport.h"

// Defines the communication parameters for Broadcast.
static std::string BROADCAST_ADDRESS = "192.168.1.255:15000";
static int BROADCAST_QUEUE_LENGTH = 1024;

///////////////////////////////////////////////////////////////////////////////
// Returns a broadcast transport configured for the drones.
///////////////////////////////////////////////////////////////////////////////
Madara::Transport::Broadcast_Transport* get_ardrone2_broadcast_transport(int id, 
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
  transportSettings.hosts[0] = BROADCAST_ADDRESS;
  transportSettings.queue_length = BROADCAST_QUEUE_LENGTH;

  // Create the actual transport.
  Madara::Transport::Broadcast_Transport* transport = 
    new Madara::Transport::Broadcast_Transport(hostId,
    kb->get_context(), transportSettings, true);

  return transport;
}

#endif
