/*******************************************************************************
 * DroneRK_Transport.h
 *
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *
 * Custom Madara transport for DroneRK
 ******************************************************************************/

#ifndef _DRONERK_TRANSPORT_H_
#define _DRONERK_TRANSPORT_H_

#include <string>

#include "madara/transport/Transport.h"
#include "DroneRK_Transport_Read_Thread.h"

#include "madara/utility/Scoped_Array.h"
#include "madara/knowledge_engine/Thread_Safe_Context.h"
#include "ace/SOCK_Dgram_Bcast.h"
#include "madara/utility/stdint.h"
#include "madara/expression_tree/Expression_Tree.h"

/**
 * @class DroneRK_Transport
 * @brief Custom madara transport for Drone-RK systems. Sends smaller data over
 *        long range radio and all data over wifi
 **/
class DroneRK_Transport : public Madara::Transport::Base
{
public:
  /**
   * Constructor
   * @param   id   unique identifer - usually a combination of host:port
   * @param   context  knowledge context
   * @param   config   transport configuration settings
   * @param   launch_transport  whether or not to launch this transport
   **/
  DroneRK_Transport(const std::string & id, 
    Madara::Knowledge_Engine::Thread_Safe_Context & context, 
    Madara::Transport::Settings& config, bool launch_transport, int size);

  /**
   * Destructor
   **/
  ~DroneRK_Transport();

  /**
   * Sends a list of knowledge updates to listeners
   * @param   updates listing of all updates that must be sent
   * @return  result of write operation or -1 if we are shutting down
   **/
  long send_data(const Madara::Knowledge_Records & updates);
  
  /**
   * Closes the transport
   **/
  void close(void);
  
  /**
   * Accesses reliability setting
   * @return  whether we are using reliable dissemination or not
   **/
  int reliability(void) const;
  
  /**
   * Sets the reliability setting
   * @return  the changed setting
   **/
  int reliability(const int & setting);

  /**
   * Initializes the transport
   * @return  0 if success
   **/
  int setup(void);

private:
  
  /// host:port identifier of this process
  const std::string                         id_;

  /// thread for reading knowledge updates
  DroneRK_Transport_Read_Thread *           thread_;

  /// indicates whether the transport is correctly configured
  bool                                      valid_setup_;
  
  /// holds all multicast addresses we are sending to
  std::vector <ACE_INET_Addr>               addresses_;

  /// holds splitters for knowledge multiassignment expression for speed
  std::vector <std::string>                 splitters_;

  /// underlying socket for sending
  ACE_SOCK_Dgram_Bcast                      socket_;

  /// data received rules, defined in Transport settings
  Madara::Expression_Tree::Expression_Tree  on_data_received_;

  /// buffer for sending
  Madara::Utility::Scoped_Array <char>      buffer_;

  /// size threshold for long range vs wifi
  const int                                 size_threshold_;
};

#endif // _DRONERK_TRANSPORT_H_
