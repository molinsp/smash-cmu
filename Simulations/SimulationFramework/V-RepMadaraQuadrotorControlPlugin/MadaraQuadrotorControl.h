/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#ifndef _MADARA_QUADROTOR_CONTROL_H_
#define _MADARA_QUADROTOR_CONTROL_H_

#include "madara/knowledge_engine/Knowledge_Base.h"
#include <vector>
#include <string>
using std::string;

#include "Location.h"

/**
 * Publishes quadrotor information and executes commands
 */
class MadaraQuadrotorControl
{
private:
  /**
   * Madara knowlege base
   */
  Madara::Knowledge_Engine::Knowledge_Base* m_knowledge;
  
  /**
   * update position in the knowledge base
   */
  void updateQuadrotorPosition(const int& id, const double& x, const double& y,
    const double& z);

public:
  /**
   * stores id and current location on quadrotor
   */
  struct Status
  {
      int m_id; // drone's id
      Location m_loc; // drone's current location
  };
  
  /**
   * stores a command to the quadrotor
   */
  struct Command
  {
    ::string m_command; // actual command {move, takeoff, land}
    Location m_loc; // location where command takes place
  };

  /**
   * constructor
   */
  MadaraQuadrotorControl();

  /**
   * destructor
   */
  ~MadaraQuadrotorControl();

  /**
   * clean up the object
   */
  void terminate();

  /**
   * update status in the knowledge base
   */
  void updateQuadrotorStatus(const Status& status);

  /**
   * get new command from knowledge base
   */
  Command* getNewCommand(int droneId);
};

#endif // _MADARA_QUADROTOR_CONTROL_H_
