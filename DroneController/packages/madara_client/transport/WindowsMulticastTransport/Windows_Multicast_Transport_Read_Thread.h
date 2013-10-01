/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#ifndef _WINDOWS_MULTICAST_TRANSPORT_READ_THREAD_H_
#define _WINDOWS_MULTICAST_TRANSPORT_READ_THREAD_H_

/**
 * @file Windows_Multicast_Transport_Read_Thread.h
 * @author Sebastian Echeverria
 * @author James Edmondson
 *
 * This file contains the Windows_Multicast_Transport_Read_Thread class, which provides a
 * multicast transport for reading knowledge updates in KaRL
 **/

#include <winsock2.h>
#include <string>

#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "madara/transport/Message_Header.h"
#include "madara/transport/Transport.h"
#include "ace/Barrier.h"

/**
  * @class Windows_Multicast_Transport_Read_Thread
  * @brief Thread for reading knowledge updates through a Multicast
  *        datagram socket
  **/
class Windows_Multicast_Transport_Read_Thread
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
  Windows_Multicast_Transport_Read_Thread (
    const Madara::Transport::Settings & settings,
    const std::string & id,
    Madara::Knowledge_Engine::Thread_Safe_Context & context, 
    SOCKET & write_socket,
    const char* mc_ipaddr,
    int mc_port,
    Madara::Transport::Bandwidth_Monitor & send_monitor,
    Madara::Transport::Bandwidth_Monitor & receive_monitor,
    Madara::Transport::Packet_Scheduler & packet_scheduler);
      
  /**
  * Destructor
  **/
  ~Windows_Multicast_Transport_Read_Thread ();
      
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
    * Sends a rebroadcast packet.
    * @param  print_prefix     prefix to include before every log message,
    *                          e.g., "My_Transport::svc"
    * @param   header   header for the rebroadcasted packet
    * @param   records  records to rebroadcast (already filtered for
    *                   rebroadcast)
    **/
  void rebroadcast (
    const char * print_prefix,
    Madara::Transport::Message_Header * header,
    const Madara::Knowledge_Map & records);

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
    volatile bool							  terminated_;
      
    /// Indicates whether the read thread is ready to accept messages
    bool									  is_ready_;

	/// The multicast IP address to be used to listen for messages.
	char*								  mc_ipaddr_;

    /// underlying socket for sending
    sockaddr_in                               socketAddress_;
    
    /// underlying socket for sending
    SOCKET                &                   write_socket_;

    /// underlying socket for sending
    SOCKET                                    read_socket_;

    /// buffer for receiving
    Madara::Utility::Scoped_Array <char>      buffer_;

    /// pointer to qos_settings (if applicable)
    const Madara::Transport::QoS_Transport_Settings * 
      qos_settings_;
      
    /// monitor for sending bandwidth usage
    Madara::Transport::Bandwidth_Monitor   &   send_monitor_;
      
    /// monitor for receiving bandwidth usage
    Madara::Transport::Bandwidth_Monitor   &   receive_monitor_;

    /// scheduler for mimicking real-world packet dropping
    Madara::Transport::Packet_Scheduler &      packet_scheduler_;

    /// data received rules, defined in Transport settings
    Madara::Knowledge_Engine::Compiled_Expression  on_data_received_;

};

#endif // _CUSTOM_TRANSPORT_READ_THREAD_H_
