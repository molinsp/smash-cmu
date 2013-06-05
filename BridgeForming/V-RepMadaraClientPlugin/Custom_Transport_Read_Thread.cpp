/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#define NOMINMAX

#include "Custom_Transport_Read_Thread.h"

#include "madara/utility/Log_Macros.h"
#include "madara/transport/Message_Header.h"

#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <algorithm>

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

Custom_Transport_Read_Thread::Custom_Transport_Read_Thread (
  const Madara::Transport::Settings & settings, const std::string & id,
  Madara::Knowledge_Engine::Thread_Safe_Context & context, const char* mc_ipaddr, int mc_port)
  : settings_ (settings), id_ (id), context_ (context),
    barrier_ (2), 
    terminated_ (false), 
    //mutex_ (), is_not_ready_ (mutex_), 
    is_ready_ (false)
{
  _beginthreadex(NULL, 0, threadfunc, (void*)this, 0, 0);

  MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
    DLINFO "Custom_Transport_Read_Thread::Custom_Transport_Read_Thread:" \
    " read thread started\n"));

    // Prepare socket.
    socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(socket_ == INVALID_SOCKET)
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "Custom_Transport_Read_Thread::Custom_Transport_Read_Thread:" \
        " Error creating socket\n"));
    }

    /* allow multiple sockets to use the same PORT number */
    u_int yes=1;
    if (setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, (char *)  &yes, sizeof(yes)) < 0) 
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "Custom_Transport_Read_Thread::Custom_Transport_Read_Thread:" \
        " Reusing ADDR failed\n"));
   }

    // Prepare multicast address.
    //std::string source_iface = "10.64.49.19";
    memset(&socketAddress_, 0, sizeof(socketAddress_));
    socketAddress_.sin_family       = AF_INET;
    //socketAddress_.sin_port         = htons(mc_port*10 + settings_.id);
    socketAddress_.sin_port         = htons(mc_port);
    socketAddress_.sin_addr.s_addr  = htonl(INADDR_ANY);  /*inet_addr(source_iface.c_str());//*/
    
    // Bind to receive address.
    if (bind(socket_, (sockaddr *) &socketAddress_, sizeof(socketAddress_)) < 0) 
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "Custom_Transport_Read_Thread::Custom_Transport_Read_Thread:" \
        " Bind failed\n"));
    }
     
    // Use setsockopt() to request that the kernel join a multicast group.
    ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(mc_ipaddr);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(socket_, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)  &mreq, sizeof(mreq)) < 0) 
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "Custom_Transport_Read_Thread::Custom_Transport_Read_Thread:" \
        " Joining multicast failed\n"));
    }
    else
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "Custom_Transport_Read_Thread::Custom_Transport_Read_Thread:" \
        " Joining multicast succeeded.\n"));
    }
}

Custom_Transport_Read_Thread::~Custom_Transport_Read_Thread ()
{
    // Use setsockopt() to request that the kernel leaves a multicast group.
    const char * host = inet_ntoa(socketAddress_.sin_addr);
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(host);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(socket_, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)  &mreq, sizeof(mreq)) < 0) 
    {
    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "Custom_Transport_Read_Thread::close:" \
      " Error unsubscribing to multicast address\n"));
    }

    closesocket (socket_);
}

int
Custom_Transport_Read_Thread::close (void)
{
  terminated_ = true;

  barrier_.wait ();

  return 0;
}

int
Custom_Transport_Read_Thread::svc (void)
{
    return 0;
}

unsigned __stdcall threadfunc(void * param)
{
    Custom_Transport_Read_Thread* trt = (Custom_Transport_Read_Thread*)param;

    std::ofstream outputFile;
    outputFile.open(std::string("test" + SSTR(trt->settings_.id) + ".txt").c_str());
    outputFile << "Starting thread" << std::endl;
    
    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "Custom_Transport_Read_Thread::Multicast_Transport_Read_Thread:" \
      " starting"));

  char buffer[2048];

  while (false == trt->terminated_)
  {
    outputFile << "Custom_Transport_Read_Thread::svc:" \
      " entering message iteration "<<std::endl; outputFile.flush();
    
    // Wait until timeout or data received.
    //fd_set fds;
    //FD_ZERO(&fds);
    //FD_SET(trt->socket_, &fds);
    //timeval timeout;
    //timeout.tv_sec = 5;
    //timeout.tv_usec = 0;
    //int success = select(trt->socket_, &fds, NULL, NULL, &timeout) ;
    //if (success == 0)
    //{ 
    //    outputFile << "Custom_Transport_Read_Thread::svc:" \
    //      " timeout waiting for messages "<<std::endl; outputFile.flush();
    //}
    //else if(success == SOCKET_ERROR)
    //{
    //    outputFile << "Custom_Transport_Read_Thread::svc:" \
    //      " error ocurred waiting for messages "<<std::endl; outputFile.flush();
    //}

    // read the message
    sockaddr_in from_addr;
    int bytes_read = 0;
    //if(success > 0)
    {
        int from_len = sizeof(from_addr);
        memset(&from_addr, 0, from_len);
        memset(buffer, 0, sizeof(buffer));
        bytes_read = recvfrom(trt->socket_, buffer, sizeof(buffer), 0, (sockaddr *) &from_addr, &from_len);
    }

  //ACE_Time_Value wait_time (1);
  //ACE_INET_Addr  remote;
  //ssize_t bytes_read = trt->socket_.recv ((void *)buffer, trt->settings_.queue_length, remote, 0, &wait_time);

    int from_port = ntohs(from_addr.sin_port);
     
    if (bytes_read <= 0)
    {
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "Custom_Transport_Read_Thread::svc:" \
        " received %d bytes from %s:%d. Proceeding to next wait\n", bytes_read, inet_ntoa(from_addr.sin_addr),from_port));

        outputFile << "Custom_Transport_Read_Thread::svc:" \
              " received " << bytes_read << " bytes. Proceeding to next wait. ("<< inet_ntoa(from_addr.sin_addr) <<":" <<from_port << ")" <<std::endl;outputFile.flush();

    }
    else
    {
        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO "Custom_Transport_Read_Thread::svc:" \
          " received a message header of %d bytes from %s:%d\n",
          bytes_read,
          inet_ntoa(from_addr.sin_addr), from_port));

        outputFile << "Custom_Transport_Read_Thread::svc:" \
            " received a message header of "<< bytes_read << " bytes from "<< inet_ntoa(from_addr.sin_addr) << ":" << from_port<<std::endl; outputFile.flush();

      int64_t buffer_remaining = (int64_t)bytes_read;
      Madara::Transport::Message_Header header;
      char * update = header.read (buffer, buffer_remaining);

      // reject the message if it is not KaRL
      if (strncmp (header.madara_id, "KaRL", 4) != 0)
      {
        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO "Custom_Transport_Read_Thread::svc:" \
          " dropping non-KaRL message from %s:%d\n",
          inet_ntoa(from_addr.sin_addr), from_port));

        outputFile << "Custom_Transport_Read_Thread::svc:" \
              "dropping non-KaRL message from  "<< inet_ntoa(from_addr.sin_addr) << ":" << from_port<<std::endl;outputFile.flush();
        continue;
      }
      else
      {
        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO "Custom_Transport_Read_Thread::svc:" \
          " processing KaRL message from %s:%d\n",
          inet_ntoa(from_addr.sin_addr), from_port));
        outputFile << "Custom_Transport_Read_Thread::svc:" \
              " processing KaRL message from  "<< inet_ntoa(from_addr.sin_addr) << ":" << from_port<<std::endl;outputFile.flush();
      }
    
      // reject the message if it is us as the originator (no update necessary)
      //if (strncmp (header.originator, id_.c_str (),
      //     std::min (sizeof (header.originator), id_.size ())) == 0)
      //{
      //  MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      //    DLINFO "Custom_Transport_Read_Thread::svc:" \
      //    " dropping message from ourself (id %s)\n",
      //    id_.c_str ()));
      //  continue;
      //}
      //else
      //{
      //  MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
      //    DLINFO "Custom_Transport_Read_Thread::svc:" \
      //    " remote id (%s:%d) is not our own\n",
      //    remote.get_host_addr (), remote.get_port_number ()));
      //}
      
      // reject the message if it is from a different domain
      if (strncmp (header.domain, 
                   trt->settings_.domains.c_str(),
                   std::min(sizeof (header.domain), trt->settings_.domains.size())
                  ) != 0)
      {
        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO "Custom_Transport_Read_Thread::svc:" \
          " remote id (%s:%d) in a different domain (%s). Dropping message.\n",
          inet_ntoa(from_addr.sin_addr), from_port,
          header.domain));

        outputFile << "Custom_Transport_Read_Thread::svc:" \
              " remote in a different domain (" << header.domain <<"). Dropping message. "<< inet_ntoa(from_addr.sin_addr) << ":" << from_port<<std::endl;outputFile.flush();
        continue;
      }
      else
      {
        MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
          DLINFO "Custom_Transport_Read_Thread::svc:" \
          " remote id (%s:%d) message is in our domain\n",
          inet_ntoa(from_addr.sin_addr), from_port));

        outputFile << "Custom_Transport_Read_Thread::svc:" \
              " remote id message is in our domain  "<< inet_ntoa(from_addr.sin_addr) << ":" << from_port<<std::endl;outputFile.flush();
      }

      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "Custom_Transport_Read_Thread::svc:" \
        " iterating over the %d updates\n",
        header.updates));
      
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "Custom_Transport_Read_Thread::svc:" \
        " locking context\n"));
      
      // temporary record for reading from the updates buffer
      Madara::Knowledge_Record record;
      record.quality = header.quality;
      record.clock = header.clock;
      std::string key;

      // lock the context
      trt->context_.lock ();
            
      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "Custom_Transport_Read_Thread::svc:" \
        " past the lock\n"));

      // iterate over the updates
      for (uint32_t i = 0; i < header.updates; ++i)
      {
        // read converts everything into host format from the update stream
        update = record.read (update, key, buffer_remaining);
        
        if (buffer_remaining < 0)
        {
          MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
            DLINFO "Custom_Transport_Read_Thread::svc:" \
            " unable to process message. Buffer remaining is negative." \
            " Server is likely being targeted by custom KaRL tools.\n"));

        outputFile << "Custom_Transport_Read_Thread::svc:" \
              " unable to process message. Buffer remaining is negative."<<std::endl;outputFile.flush();

          break;
        }

        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO "Custom_Transport_Read_Thread::svc:" \
          " attempting to apply %s=%s\n",
          key.c_str (), record.to_string ().c_str ()));

        int result = record.apply (trt->context_, key, header.quality,
          header.clock, false);

        if (result != 1)
        {
          MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
            DLINFO "Custom_Transport_Read_Thread::svc:" \
            " update %s=%s was rejected\n",
            key.c_str (), record.to_string ().c_str ()));
        }
        else
        {
          MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
            DLINFO "Custom_Transport_Read_Thread::svc:" \
            " update %s=%s was accepted\n",
            key.c_str (), record.to_string ().c_str ()));
        }
      }
      
      // unlock the context
      trt->context_.unlock ();
      trt->context_.set_changed ();
    }
  }
  
    outputFile << "Closing listener thread"<<std::endl;outputFile.flush();
    outputFile.close();

  trt->barrier_.wait ();
  return 0;
}