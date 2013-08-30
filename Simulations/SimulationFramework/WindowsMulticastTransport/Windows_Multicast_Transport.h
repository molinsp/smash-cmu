/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#ifndef _WINDOWS_MULTICAST_TRANSPORT_H_
#define _WINDOWS_MULTICAST_TRANSPORT_H_

/**
 * @file Windows_Multicast_Transport.h
 * @author Sebastian Echeverria.
 *
 * This file contains the Windows_Multicast_Transport class, which provides a
 * multicast transport for sending knowledge updates in KaRL using Windows native APIs.
 **/

#include <winsock2.h>
#include <string>

#include "madara/transport/Transport.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"

#include "Windows_Multicast_Transport_Read_Thread.h"

/**
  * @class Windows_Multicast_Transport
  * @brief Multicast-based transport for knowledge in Windows.
  **/
class Windows_Multicast_Transport : public Madara::Transport::Base
{
public:
  /**
    * Constructor
    * @param   id   unique identifer - usually a combination of host:port
    * @param   context  knowledge context
    * @param   config   transport configuration settings
    * @param   launch_transport  whether or not to launch this transport
    **/
  Windows_Multicast_Transport (const std::string & id, 
    Madara::Knowledge_Engine::Thread_Safe_Context & context, 
    Madara::Transport::Settings & config, bool launch_transport);

  /**
    * Destructor
    **/
  ~Windows_Multicast_Transport ();
      
  /**
    * Sends a list of knowledge updates to listeners
    * @param   updates listing of all updates that must be sent
    * @return  result of write operation or -1 if we are shutting down
    **/
  long send_data (const Madara::Knowledge_Records & updates);
	  
  /**
    * Closes the transport
    **/
  void close (void);
      
  /**
    * Accesses reliability setting
    * @return  whether we are using reliable dissemination or not
    **/
  int reliability (void) const;
      
  /**
    * Sets the reliability setting
    * @return  the changed setting
    **/
  int reliability (const int & setting);

  /**
    * Initializes the transport
    * @return  0 if success
    **/
  int setup (void);

private:
      
  /// thread for reading knowledge updates
  Windows_Multicast_Transport_Read_Thread * thread_;
      
  /// indicates whether the transport is correctly configured
  bool                                      valid_setup_;

  /// underlying socket for sending
  sockaddr_in                               socketAddress_;

  /// underlying socket for sending
  SOCKET                                    socket_;
};

#endif // _CUSTOM_TRANSPORT_H_