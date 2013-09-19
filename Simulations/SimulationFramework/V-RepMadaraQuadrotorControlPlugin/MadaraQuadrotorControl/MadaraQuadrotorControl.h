/**
 * MadaraQuadrotorControl.h
 * Anton Dukeman
 *
 * MadaraQuadrotorControl class abstracts drone control in vrep
 */

#ifndef _MADARA_QUADROTOR_CONTROL_H_
#define _MADARA_QUADROTOR_CONTROL_H_

#include "madara/knowledge_engine/Knowledge_Base.h"

#include <string>
using std::string;

class MadaraQuadrotorControl
{
private:
  /**
   * Pointer to knowledge_base to work with
   */
  Madara::Knowledge_Engine::Knowledge_Base* m_knowledge;

  /**
   * This drone's id
   */
  unsigned int m_id;

  /**
   * Cleanup this object
   */
  void terminate();

public:
  /**
   * Constructor
   */
  MadaraQuadrotorControl() = delete;
  MadaraQuadrotorControl(const unsigned int& id, const string& ip = "239.255.0.1:4150",
    const string& host = "");

  /**
   * Destructor
   */
  ~MadaraQuadrotorControl();

  /**
   * Go to a location
   */
  void goToLocation(const float& lat, const float& lon, const float& alt);

  /**
   * Land where you are now
   */
  void land();

  /**
   * Takeoff
   */
  void takeoff();
};

#endif // _MADARA_QUADROTOR_CONTROL_H_
