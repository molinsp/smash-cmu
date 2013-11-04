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
: Base (id, config, context),
  thread_ (0), valid_setup_ (false),  open_ (false)//,
  //socket_ (ACE_sap_any_cast (ACE_INET_Addr &), PF_INET, 0, 1)
{
  if (launch_transport)
    setup ();
}

Windows_Multicast_Transport::~Windows_Multicast_Transport ()
{
  MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
       DLINFO "Windows_Multicast_Transport::~Windows_Multicast_Transport:" \
       " Destroying transport\n"));
  if(this->open_ )
    close ();
}

void
Windows_Multicast_Transport::close (void)
{
  MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
       DLINFO "Windows_Multicast_Transport::close:" \
       " Closing transport\n"));
  this->invalidate_transport ();

  if (thread_)
  {
    thread_->close ();
    delete thread_;
    thread_ = 0;
  }
  
  if(socket_)
      closesocket(socket_);

  MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
       DLINFO "Windows_Multicast_Transport::close:" \
       " Calling WSACleanup\n"));

  WSACleanup(); //Clean up Winsock

  this->shutting_down_ = false;
  this->open_ = false;
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
          " Error starting Windows Sockets: %d\n", error));
    }
    else
    {
        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO "Windows_Multicast_Transport::setup:" \
          " Successfully initialized WSA\n"));
    }

  // call base setup method to initialize certain common variables
  Base::setup ();

  // resize addresses to be the size of the list of hosts
  if (settings_.hosts.size () > 0)
  {
    std::vector<std::string> parts = split(settings_.hosts[0], ':');

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

    // Prepare address for multicast comm.
    memset(&socketAddress_, 0, sizeof(socketAddress_));
    socketAddress_.sin_family       = AF_INET;
    socketAddress_.sin_port         = htons(mc_port);
    socketAddress_.sin_addr.s_addr  = inet_addr(mc_ipaddr);

    // start thread with the addresses (only looks at the first one for now)
    thread_ = new Windows_Multicast_Transport_Read_Thread (
                    settings_, id_, context_, socket_,
                    mc_ipaddr, mc_port, this->send_monitor_,
                    this->receive_monitor_,
                    this->packet_scheduler_);
  }

  // Indicate that the transport has been opened.
  open_ = true;

  return this->validate_transport ();
}

long
Windows_Multicast_Transport::send_data (
  const Madara::Knowledge_Records & updates)
{
  long result =
    prep_send (updates, "Windows_Multicast_Transport::send_data:");
  
  if (settings_.hosts.size () > 0 && result > 0)
  {
    //int bytes_sent = socket_.send(
    //  buffer, size, addresses_[0]);

    int bytes_sent = sendto(socket_, buffer_.get_ptr (),
      (int)result, 0, (sockaddr *) &socketAddress_, sizeof(socketAddress_));

    MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
      DLINFO "Windows_Multicast_Transport::send_data:" \
      " Sent packet of size %d to %s:%d\n",
      bytes_sent, inet_ntoa(socketAddress_.sin_addr), ntohs(socketAddress_.sin_port)));

    result = (long) bytes_sent;
  }

  return result;
}
  