#ifndef _CUSTOM_TRANSPORT_READ_THREAD_H_
#define _CUSTOM_TRANSPORT_READ_THREAD_H_

/**
 * @file Custom_Transport_Read_Thread.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the Custom_Transport_Read_Thread class, which provides a
 * multicast transport for reading knowledge updates in KaRL
 **/

#include <winsock2.h>
#include <string>

#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/transport/Transport.h"
#include "ace/Barrier.h"

/**
  * @class Custom_Transport_Read_Thread
  * @brief Thread for reading knowledge updates through a Multicast
  *        datagram socket
  **/
class Custom_Transport_Read_Thread
{
public:
  /**
    * Constructor
    * @param    settings   Transport settings
    * @param    id      host:port identifier of this process, to allow for 
    *                   rejection of duplicates
    * @param    context    the knowledge variables to update
    * @param    address    the multicast address we will read from
    **/
  Custom_Transport_Read_Thread (
    const Madara::Transport::Settings & settings,
    const std::string & id,
    Madara::Knowledge_Engine::Thread_Safe_Context & context
    , const char* mc_ipaddr, int mc_port);
      
  /**
  * Destructor
  **/
  ~Custom_Transport_Read_Thread ();
      
  /**
  * Signals the read thread to terminate
  **/
  int enter_barrier (void);
      
  /**
  * Closes the reading socket and clean up the thread
  **/
  int close (void);

  /**
  * Reads messages from a socket
  **/
  int svc (void);
      
  /**
  * Wait for the transport to be ready
  **/
  /*void wait_for_ready (void);*/

  friend unsigned __stdcall threadfunc(void * param);
private:
    /// Transport settings
    const Madara::Transport::Settings & settings_;

    /// host:port identifier of this process
    const std::string                                 id_;

    /// barrier for closing and waiting on the read thread
    ACE_Barrier barrier_;

    /// knowledge context
    Madara::Knowledge_Engine::Thread_Safe_Context & context_;
      
    /// atomic variable that signals termination
    volatile bool     terminated_;
      
    /// Indicates whether the read thread is ready to accept messages
    bool                               is_ready_;

    /// underlying socket for sending
    sockaddr_in                               socketAddress_;

    /// underlying socket for sending
    SOCKET                                    socket_;

    /// buffer for receiving
    Madara::Utility::Scoped_Array <char>      buffer_;
};

#endif // _CUSTOM_TRANSPORT_READ_THREAD_H_
