/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#define NOMINMAX

#include "Windows_Multicast_Transport_Read_Thread.h"

#include "madara/utility/Log_Macros.h"
#include "madara/transport/Reduced_Message_Header.h"
#include "madara/knowledge_engine/Bandwidth_Monitor.h"

#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

// Included to get all interfaces through ACE.
#include "ace/SOCK_Dgram_Mcast.h"

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
void joinMulticastGroup(const SOCKET socket,
  const char* multicastIpAddr, u_long interfaceAddr)
{
    // Use setsockopt() to request that the kernel join a multicast group.
    ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(multicastIpAddr);
    mreq.imr_interface.s_addr = interfaceAddr;
    if (setsockopt(socket, IPPROTO_IP, IP_ADD_MEMBERSHIP,
      (char *) &mreq, sizeof(mreq)) < 0) 
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "Windows_Multicast_Transport_Read_Thread::joinMulticastGroup:" \
        " Joining multicast failed for address %s.\n", multicastIpAddr));
    }
    else
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "Windows_Multicast_Transport_Read_Thread::joinMulticastGroup:" \
        " Joining multicast succeeded for address:%s.\n", multicastIpAddr));
    }
}

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
void leaveMulticastGroup(const SOCKET socket,
  const char* multicastIpAddr, u_long interfaceAddr)
{
    // Use setsockopt() to request that the kernel leaves a multicast group.
    ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(multicastIpAddr);
    mreq.imr_interface.s_addr = interfaceAddr;
    if (setsockopt(socket, IPPROTO_IP, IP_DROP_MEMBERSHIP,
      (char *)  &mreq, sizeof(mreq)) < 0) 
    {
		int errorCode =  WSAGetLastError ();
        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
            DLINFO "Windows_Multicast_Transport_Read_Thread::leaveMulticastGroup:" \
            " Error unsubscribing to multicast address %s: error code: %d\n",
            multicastIpAddr, errorCode));
    }
    else
    {
		int errorCode =  WSAGetLastError ();
        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
            DLINFO "Windows_Multicast_Transport_Read_Thread::leaveMulticastGroup:" \
            " Successfully unsubscribed from multicast address %s \n",
            multicastIpAddr));
    }
}

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
void joinMulticastOnAllInterfaces(
  const SOCKET socket, const char* multicastIpAddr)
{
    // Load all available interfaces through an ACE function.
    ACE_INET_Addr *interfaceAddresses = 0;
    size_t interfacesCount;
    ACE::get_ip_interfaces (interfacesCount, interfaceAddresses);

    // Loop through all results and join all interfaces.
    while (interfacesCount > 0)
    {
        --interfacesCount;
        if (interfaceAddresses[interfacesCount].get_type () != AF_INET)
            continue;

        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
            DLINFO "Windows_Multicast_Transport_Read_Thread::joinMulticastOnAllInterfaces:" \
            " Attempting to join multicast interface %s\n",
            interfaceAddresses[interfacesCount].get_host_addr ()));
        joinMulticastGroup(socket, multicastIpAddr,
          inet_addr(interfaceAddresses[interfacesCount].get_host_addr ()));
    }
}

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
void leaveMulticastOnAllInterfaces(
  const SOCKET socket, const char* multicastIpAddr)
{
    // Load all available interfaces through an ACE function.
    ACE_INET_Addr *interfaceAddresses = 0;
    size_t interfacesCount;
    ACE::get_ip_interfaces (interfacesCount, interfaceAddresses);

    // Loop through all results and join all interfaces.
    while (interfacesCount > 0)
    {
        --interfacesCount;
        if (interfaceAddresses[interfacesCount].get_type () != AF_INET)
            continue;

        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
            DLINFO "Windows_Multicast_Transport_Read_Thread::leaveMulticastOnAllInterfaces:" \
            " Attempting to leave multicast on interface %s\n",
            interfaceAddresses[interfacesCount].get_host_addr ()));
        leaveMulticastGroup(socket, multicastIpAddr,
          inet_addr(interfaceAddresses[interfacesCount].get_host_addr ()));
    }
}

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
Windows_Multicast_Transport_Read_Thread::Windows_Multicast_Transport_Read_Thread (
  const Madara::Transport::Settings & settings,
  const std::string & id,
  Madara::Knowledge_Engine::Thread_Safe_Context & context, 
  SOCKET & write_socket,
  const char* mc_ipaddr,
  int mc_port,
  Madara::Knowledge_Engine::Bandwidth_Monitor & send_monitor,
  Madara::Knowledge_Engine::Bandwidth_Monitor & receive_monitor)
  : settings_ (settings), id_ (id), context_ (context),
    barrier_ (2), 
    terminated_ (false), 
    is_ready_ (false),
    write_socket_ (write_socket),
    send_monitor_ (send_monitor),
    receive_monitor_ (receive_monitor)
{
  mc_ipaddr_ = NULL;

  qos_settings_ = dynamic_cast <const Madara::Transport::QoS_Transport_Settings *> (&settings);

  // setup the receive buffer
  if (settings_.queue_length > 0)
  {
    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "Windows_Multicast_Transport_Read_Thread::Windows_Multicast_Transport_Read_Thread:" \
        " Setting buffer with size %d from queue length\n", settings_.queue_length));
    buffer_ = new char [settings_.queue_length];
  }

  MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "Windows_Multicast_Transport_Read_Thread::Windows_Multicast_Transport_Read_Thread:" \
    " read thread started\n"));

  // Prepare socket.
  read_socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(read_socket_ == INVALID_SOCKET)
  {
    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "Windows_Multicast_Transport_Read_Thread::Windows_Multicast_Transport_Read_Thread:" \
      " Error creating socket\n"));
  }

  /* allow multiple sockets to use the same PORT number */
  u_int yes=1;
  if (setsockopt(read_socket_, SOL_SOCKET, SO_REUSEADDR, (char *)  &yes, sizeof(yes)) < 0) 
  {
    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "Windows_Multicast_Transport_Read_Thread::Windows_Multicast_Transport_Read_Thread:" \
      " Reusing ADDR failed\n"));
  }

  // Prepare multicast address.
  //std::string source_iface = "10.64.49.19";
  memset(&socketAddress_, 0, sizeof(socketAddress_));
  socketAddress_.sin_family       = AF_INET;
  socketAddress_.sin_port         = htons(mc_port);
  socketAddress_.sin_addr.s_addr  = htonl(INADDR_ANY);  /*inet_addr(source_iface.c_str());//*/

  // Bind to receive address.
  if (bind(read_socket_, (sockaddr *) &socketAddress_, sizeof(socketAddress_)) < 0) 
  {
    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "Windows_Multicast_Transport_Read_Thread::Windows_Multicast_Transport_Read_Thread:" \
      " Bind failed\n"));
  }

  // check for an on_data_received ruleset
  if (settings_.on_data_received_logic.length () != 0)
  {
    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "Windows_Multicast_Transport_Read_Thread::Windows_Multicast_Transport_Read_Thread:" \
      " setting rules to %s\n", 
      settings_.on_data_received_logic.c_str ()));

    on_data_received_ = context_.compile (settings_.on_data_received_logic);
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "Windows_Multicast_Transport_Read_Thread::Windows_Multicast_Transport_Read_Thread:" \
      " no permanent rules were set\n"));
  }
  
  // Store the multicast address to use for future reference, when leaving group.
  int ipBufferSize = strlen(mc_ipaddr) + 1;
  mc_ipaddr_ = new char[ipBufferSize];
  memset(mc_ipaddr_, 0, ipBufferSize);
  strncpy(mc_ipaddr_, mc_ipaddr, ipBufferSize);

  // Join the multicast address. Join the default interface, and the loopback, explicitly.
  joinMulticastOnAllInterfaces(read_socket_, mc_ipaddr_);
  //joinMulticastGroup(socket_, mc_ipaddr_, htonl(INADDR_ANY));
  //joinMulticastGroup(socket_, mc_ipaddr_, inet_addr("127.0.0.1"));
  
  _beginthreadex(NULL, 0, threadfunc, (void*)this, 0, 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
Windows_Multicast_Transport_Read_Thread::~Windows_Multicast_Transport_Read_Thread ()
{
  if(mc_ipaddr_ != NULL)
  {
    // Leave the group, for both interfaces we selected.
    leaveMulticastOnAllInterfaces(read_socket_, mc_ipaddr_);

    delete mc_ipaddr_;
  }

  closesocket (read_socket_);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
int
Windows_Multicast_Transport_Read_Thread::close (void)
{
  terminated_ = true;

  barrier_.wait ();

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
int
Windows_Multicast_Transport_Read_Thread::svc (void)
{
    return 0;
}

void
Windows_Multicast_Transport_Read_Thread::rebroadcast (
  const char * print_prefix,
  Madara::Transport::Message_Header * header,
  const Madara::Knowledge_Map & records)
{
  int64_t buffer_remaining = (int64_t) settings_.queue_length;
  char * buffer = buffer_.get_ptr ();
  int result = prep_rebroadcast (buffer, buffer_remaining,
                                 *qos_settings_, print_prefix, header, records);

  if (result > 0)
  {
    int bytes_sent = sendto(write_socket_, buffer_.get_ptr (),
      (int)result, 0, (sockaddr *) &socketAddress_, sizeof(socketAddress_));

    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "%s:" \
      " Sent packet of size %d\n",
      print_prefix,
      bytes_sent));
      
    send_monitor_.add ((uint32_t)bytes_sent);

    MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
      DLINFO "%s:" \
      " Send bandwidth = %d B/s\n",
      print_prefix,
      send_monitor_.get_bytes_per_second ()));
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned __stdcall threadfunc(void * param)
{
  Windows_Multicast_Transport_Read_Thread* trt = (Windows_Multicast_Transport_Read_Thread*)param;

  std::ofstream outputFile;
  outputFile.open(std::string("customtransportread" + SSTR(trt->settings_.id) + "log.txt").c_str());
  outputFile << "Starting thread" << std::endl;

  MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "Windows_Multicast_Transport_Read_Thread::svc:" \
    " starting"));

  char * buffer = trt->buffer_.get_ptr ();
  const char * print_prefix = "Windows_Multicast_Transport_Read_Thread::svc";
  int64_t buffer_remaining = trt->settings_.queue_length;


  while (false == trt->terminated_)
  {
    Madara::Knowledge_Map rebroadcast_records;

    if (buffer == 0)
    {
      MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
        DLINFO "%s:" \
        " Unable to allocate buffer of size %d. Exiting thread.\n",
        print_prefix,
        trt->settings_.queue_length));
    
      break;
    }
    
    MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
      DLINFO "%s:" \
      " entering a recv on the socket.\n",
      print_prefix));
    
    outputFile << "Windows_Multicast_Transport_Read_Thread::svc:" " entering message iteration "<<std::endl; outputFile.flush();

    // Wait until timeout or data received.
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(trt->read_socket_, &fds);
    timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    int newDataAvailable = select(trt->read_socket_, &fds, NULL, NULL, &timeout) ;
    if (newDataAvailable == 0)
    { 
        outputFile << "Windows_Multicast_Transport_Read_Thread::svc:"
                    " timeout waiting for messages "<<std::endl; outputFile.flush();

        // Skip the rest of the loop; since there was a timeout, we won't have any real data.
        continue;
    }
    else if(newDataAvailable == SOCKET_ERROR)
    {
        outputFile << "Windows_Multicast_Transport_Read_Thread::svc:"
            " error ocurred waiting for messages "<<std::endl; outputFile.flush();
        
        int error = WSAGetLastError();
        wchar_t *s = NULL;
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
                        NULL, error,
                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                        s, 0, NULL);
        outputFile << "Error: " << error << ", details: " <<s <<std::endl; outputFile.flush();
        LocalFree(s);

        // Skip the rest of the loop; since there was an error, we won't have a real message.
        continue;
    }

    // read the message
    sockaddr_in from_addr;
    int from_len = sizeof(from_addr);
    memset(&from_addr, 0, from_len);
    int bytes_read = 0;
    if(newDataAvailable > 0)
    {        
      outputFile << "Windows_Multicast_Transport_Read_Thread::svc:"
                        " reading new data from socket into buffer of size " << buffer_remaining <<std::endl; outputFile.flush();
      memset(buffer, 0, sizeof(buffer));
      bytes_read = recvfrom(trt->read_socket_, buffer, buffer_remaining,
        0, (sockaddr *) &from_addr, &from_len);

      // Check for errors reading the socket.
      if(bytes_read == SOCKET_ERROR)
      {
        outputFile << "Windows_Multicast_Transport_Read_Thread::svc:"
            " error ocurred reading data "<<std::endl; outputFile.flush();
        
        int error = WSAGetLastError();
        wchar_t *s = NULL;
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
                        NULL, error,
                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                        s, 0, NULL);
        outputFile << "Error: " << error << ", details: " <<s <<std::endl; outputFile.flush();
        LocalFree(s);

        // Skip the rest of the loop; since there was an error, we won't have a real message.
        continue;
      }
    }

    // Get the remote host into a string.
    char remote_host_ip[INET_ADDRSTRLEN+1];
    memset(remote_host_ip, 0, sizeof(remote_host_ip));
    inet_ntop(AF_INET, &(from_addr.sin_addr.s_addr), remote_host_ip, INET_ADDRSTRLEN );
    std::stringstream remote_host;
    remote_host << remote_host_ip << ":" << from_addr.sin_port;
    
    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "%s:" \
      " received a message header of %d bytes from %s\n",
      print_prefix,
      bytes_read,
      remote_host.str ().c_str ()));

    outputFile << print_prefix << " received a message header of " << bytes_read << " bytes from " <<  remote_host.str ().c_str () <<std::endl; outputFile.flush();
    
    if (bytes_read > 0)
    {
      Madara::Transport::Message_Header * header = 0;

      int updatesPerformed = Madara::Transport::process_received_update (
        buffer, bytes_read, trt->id_, trt->context_,
        *(trt->qos_settings_), trt->send_monitor_, trt->receive_monitor_,
        rebroadcast_records,
        trt->on_data_received_, "Windows_Multicast_Transport_Read_Thread::svc",
        remote_host.str ().c_str (), header);

      if(updatesPerformed < 0)
      {
        outputFile << " Warning: Invalid message received, reason code: " << updatesPerformed <<std::endl; outputFile.flush();
      }
      
      if(header)
      {
          if (header->ttl > 0 && rebroadcast_records.size () > 0 &&
              trt->qos_settings_ && trt->qos_settings_->get_participant_ttl () > 0)
          {
            --header->ttl;
            header->ttl = std::min (
              trt->qos_settings_->get_participant_ttl (), header->ttl);

            outputFile << "Performing rebroadcast." <<std::endl; outputFile.flush();
            trt->rebroadcast (print_prefix, header, rebroadcast_records);
          }

          // delete header
          delete header;
      }
    }
  }

  trt->barrier_.wait ();
  return 0;
}