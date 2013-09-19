/**
 * MadaraQuadrotorControl.cpp
 * Anton Dukeman
 *
 * Definitions for MadaraQuadrotorControl class
 */

#include "MadaraQuadrotorControl.h"

#include "madara/knowledge_engine/Knowledge_Base.h"
#include "utilities/CommonMadaraVariables.h"

#include <string>
using std::string;

/**
 * Pointer to knowledge_base to work with
 */
Madara::Knowledge_Engine::Knowledge_Base* m_knowledge;

/**
 * This drone's id
 */
unsigned int m_id;

// Constructor
MadaraQuadrotorControl::MadaraQuadrotorControl(const unsigned int& id,
  const string& ip, const string& host) : m_id(id)
{
  // Define the transport settings.
  Madara::Transport::Settings settings;
  settings.hosts_.resize(1);
  settings.hosts_[0] = ip;
  settings.type = Madara::Transport::MULTICAST;

  // Create the knowledge base.
  m_knowledge = new Madara::Knowledge_Engine::Knowledge_Base(host, settings);

  m_knowledge->set(".id", Madara::Knowledge_Record::Integer(id));
}

// Destructor
MadaraQuadrotorControl::~MadaraQuadrotorControl() { terminate(); }

// Cleanup, terminating all threads and open communications.
void MadaraQuadrotorControl::terminate()
{
  if(m_knowledge != NULL)
  {
    m_knowledge->close_transport();
    m_knowledge->clear();
    delete m_knowledge;
    m_knowledge = NULL;
  }
}

// Go to a location
void MadaraQuadrotorControl::goToLocation(const float& lat, const float& lon, const float& alt)
{
  m_knowledge->set(MV_MOVEMENT_REQUESTED, MO_MOVE_TO_GPS_CMD);
  m_knowledge->set(MV_MOVEMENT_TARGET_LAT, lat);
  m_knowledge->set(MV_MOVEMENT_TARGET_LON, lon);
  m_knowledge->set(MV_MOVEMENT_TARGET_ALT, alt);

  string eval = string(MS_SIM_DEVICES_PREFIX) + "{.id}" + string(MV_MOVEMENT_REQUESTED) + "=" + string(MV_MOVEMENT_REQUESTED) + ";";
  eval += string(MS_SIM_DEVICES_PREFIX) + "{.id}" + string(MV_MOVEMENT_TARGET_LAT) + "=" + string(MV_MOVEMENT_TARGET_LAT) + ";";
  eval += string(MS_SIM_DEVICES_PREFIX) + "{.id}" + string(MV_MOVEMENT_TARGET_LON) + "=" + string(MV_MOVEMENT_TARGET_LON) + ";";
  eval += string(MS_SIM_DEVICES_PREFIX) + "{.id}" + string(MV_MOVEMENT_TARGET_ALT) + "=" + string(MV_MOVEMENT_TARGET_ALT) + ";";

  m_knowledge->evaluate(eval);
}

// Land where you are now
void MadaraQuadrotorControl::land() {}

// Takeoff
void MadaraQuadrotorControl::takeoff() {}
