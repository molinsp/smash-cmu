/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#include "Windows_Multicast_Transport.h"

#include "madara/utility/Log_Macros.h"
#include "madara/utility/Utility.h"
#include "madara/transport/Reduced_Message_Header.h"

#include <iostream>
#include <vector>

Windows_Multicast_Transport::Windows_Multicast_Transport (const std::string & id,
        Madara::Knowledge_Engine::Thread_Safe_Context & context, 
        Madara::Transport::Settings & config, bool launch_transport)
: Base (config, context),
  id_ (id), thread_ (0), valid_setup_ (false)//,
  //socket_ (ACE_sap_any_cast (ACE_INET_Addr &), PF_INET, 0, 1)
{
  if (launch_transport)
    setup ();
}

Windows_Multicast_Transport::~Windows_Multicast_Transport ()
{
  close ();
}

void
Windows_Multicast_Transport::close (void)
{
  this->invalidate_transport ();

  if (thread_)
  {
    thread_->close ();
    delete thread_;
    thread_ = 0;
  }
  
  if(socket_)
      closesocket(socket_);

  WSACleanup(); //Clean up Winsock

  this->shutting_down_ = false;
}

int
Windows_Multicast_Transport::reliability (void) const
{
  return Madara::Transport::BEST_EFFORT;
}

int
Windows_Multicast_Transport::reliability (const int &)
{
  return Madara::Transport::BEST_EFFORT;
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

int
Windows_Multicast_Transport::setup (void)
{
    WSADATA wsadata;
    int error = WSAStartup(0x0202, &wsadata);
    if(error)
    {
        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO "Windows_Multicast_Transport::setup:" \
          " Error starting Windows Sockets\n"));
    }

  // setup the send buffer
  if (settings_.queue_length > 0)
    buffer_ = new char [settings_.queue_length];

  // resize addresses to be the size of the list of hosts
  if (settings_.hosts_.size () > 0)
  {
    std::vector<std::string> parts = split(settings_.hosts_[0], ':');

    int mc_port = atoi(parts[1].c_str());
    const char * mc_ipaddr = parts[0].c_str();

    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "Windows_Multicast_Transport::setup:" \
        " settings address[0] to %s:%d\n", 
        mc_ipaddr, mc_port));

    // Prepare socket.
    socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(socket_ == INVALID_SOCKET)
    {
        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
            DLINFO "Windows_Multicast_Transport::setup:" \
            " Error creating socket to %s:%d\n",  
            mc_ipaddr, mc_port));
    }

    // Enable broadcast options.
    //int optionValue = 1;
    //if ((setsockopt(socket_, SOL_SOCKET, SO_BROADCAST, 
    //                (char*) &optionValue, sizeof(optionValue))) < 0) 
    //{
    //    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
    //        DLINFO "Windows_Multicast_Transport::setup:" \
    //        " Error creating socket to %s:%d\n",  
    //        mc_ipaddr, mc_port));
    //}

    // Set the TTL (just in case).
    unsigned char mc_ttl = 1;     /* time to live (hop count) */
    if ((setsockopt(socket_, IPPROTO_IP, IP_MULTICAST_TTL, 
                    (char*) &mc_ttl, sizeof(mc_ttl))) < 0) 
    {
        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
            DLINFO "Windows_Multicast_Transport::setup:" \
            " Error creating socket to %s:%d\n",  
            mc_ipaddr, mc_port));
    }

    // Set the interface we will use for multicast.
    //std::string source_iface = "10.64.49.19";
    //ip_mreq multi;
    //multi.imr_multiaddr.s_addr = inet_addr(mc_ipaddr);
    //multi.imr_interface.s_addr = htonl(INADDR_ANY); /*inet_addr(source_iface.c_str()); */

    //if (setsockopt(socket_, IPPROTO_IP, IP_MULTICAST_IF,
    //	            (char *)&multi,
    //	            sizeof(multi)) <0)
    //{
    //    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
    //        DLINFO "Windows_Multicast_Transport::setup:" \
    //        " Error setting multicast iface\n"  
    //        ));
    //}

    // Prepare address for multicast comm.
    memset(&socketAddress_, 0, sizeof(socketAddress_));
    socketAddress_.sin_family       = AF_INET;
    socketAddress_.sin_port         = htons(mc_port);
    socketAddress_.sin_addr.s_addr  = inet_addr(mc_ipaddr);

    // start thread with the addresses (only looks at the first one for now)
    thread_ = new Windows_Multicast_Transport_Read_Thread (
                    settings_, id_, context_, mc_ipaddr, mc_port);
  }
  return this->validate_transport ();
}

long
Windows_Multicast_Transport::send_data (
  const Madara::Knowledge_Records & updates)
{
  
  // check to see if we are shutting down
  long ret = this->check_transport ();
  if (-1 == ret)
  {
    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "Windows_Multicast_Transport::send_data: transport has been told to shutdown")); 
    return ret;
  }
  else if (-2 == ret)
  {
    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "Windows_Multicast_Transport::send_data: transport is not valid")); 
    return ret;
  }
 
  // get the maximum quality from the updates
  uint32_t quality = Madara::max_quality (updates);
  bool reduced = false;

  // allocate a buffer to send
  char * buffer = buffer_.get_ptr ();
  int64_t buffer_remaining = settings_.queue_length;
  
  if (buffer == 0)
  {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      DLINFO "Windows_Multicast_Transport::send_data:" \
      " Unable to allocate buffer of size %d. Exiting thread.\n",
      settings_.queue_length));
    
    return -3;
  }

  // set the header to the beginning of the buffer
  Madara::Transport::Message_Header * header = 0;

  if (settings_.send_reduced_message_header)
  {
    MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
      DLINFO "Multicast_Transport::send_data:" \
      " Preparing message with reduced message header.\n"));
    header = new Madara::Transport::Reduced_Message_Header ();
    reduced = true;
  }
  else
  {
    MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
      DLINFO "Multicast_Transport::send_data:" \
      " Preparing message with normal message header.\n"));
    header = new Madara::Transport::Message_Header ();
  }
  
  // get the clock
  header->clock = Madara::Utility::endian_swap (context_.get_clock ());

  if (!reduced)
  {
    // copy the domain from settings
    strncpy (header->domain, this->settings_.domains.c_str (),
      sizeof (header->domain) - 1);

    // get the quality of the key
    header->quality = Madara::Utility::endian_swap (quality);

    // copy the message originator (our id)
    strncpy (header->originator, id_.c_str (), sizeof (header->originator) - 1);

    // send data is generally an assign type. However, Message_Header is
    // flexible enough to support both, and this will simply our read thread
    // handling
    header->type = Madara::Transport::MULTIASSIGN;
  }

  header->updates = uint32_t (updates.size ());

  // compute size of this header
  header->size = header->encoded_size ();

  // set the update to the end of the header
  char * update = header->write (buffer, buffer_remaining);
  uint64_t * message_size = (uint64_t *)buffer;
  
  // Message header format
  // [size|id|domain|originator|type|updates|quality|clock|list of updates]
  
  /**
   * size = buffer[0] (unsigned 64 bit)
   * transport id = buffer[8] (8 byte)
   * domain = buffer[16] (32 byte domain name)
   * originator = buffer[48] (64 byte originator host:port)
   * type = buffer[112] (unsigned 32 bit type of message--usually MULTIASSIGN)
   * updates = buffer[116] (unsigned 32 bit number of updates)
   * quality = buffer[120] (unsigned 32 bit quality of message)
   * clock = buffer[124] (unsigned 64 bit clock for this message)
   * knowledge = buffer[132] (the new knowledge starts here)
  **/

  // Message update format
  // [key|value]
  
  int j = 0;
  for (Madara::Knowledge_Records::const_iterator i = updates.begin ();
    i != updates.end (); ++i, ++j)
  {
    update = i->second->write (update, i->first, buffer_remaining);

    if (buffer_remaining > 0)
    {
      MADARA_DEBUG (MADARA_LOG_MINOR_EVENT, (LM_DEBUG, 
        DLINFO "Windows_Multicast_Transport::send_data:" \
        " update[%d] => encoding %s of type %d and size %d\n",
        j, i->first.c_str (), i->second->type (), i->second->size ()));
    }
    else
    {
    MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
      DLINFO "Windows_Multicast_Transport::send_data:" \
      " unable to encode update[%d] => %s of type %d and size %d\n",
      j, i->first.c_str (), i->second->type (), i->second->size ()));
    }
  }
  
  if (buffer_remaining > 0)
  {
    int size = (int)(settings_.queue_length - buffer_remaining);
    *message_size = Madara::Utility::endian_swap ((uint64_t)size);

    // send the buffer contents to the multicast address
  
    if (settings_.hosts_.size () > 0)
    {
      //int bytes_sent = socket_.send(
      //  buffer, size, addresses_[0]);

      int bytes_sent = sendto(socket_, buffer, size, 0, (sockaddr *) &socketAddress_, sizeof(socketAddress_));

      MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
        DLINFO "Windows_Multicast_Transport::send_data:" \
        " Sent packet of size %d to %s:%d\n",
        bytes_sent, inet_ntoa(socketAddress_.sin_addr), ntohs(socketAddress_.sin_port)));
    }
  }

  return 0;
}