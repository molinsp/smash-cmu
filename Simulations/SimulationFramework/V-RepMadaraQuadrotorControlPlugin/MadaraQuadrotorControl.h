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
    * Madara knowlege base.
    */
    Madara::Knowledge_Engine::Knowledge_Base* m_knowledge;

    /**
    * Update position in the knowledge base.
    */
    void updateQuadrotorPosition(const int& id, const double& x, const double& y,
        const double& z);

    /**
    * Clears the current command locally from Madara.
    */
    void clearCommand(std::string droneIdString);

	/**
	 * Stores the amount of drones that are using this controller, for correct deinitialization.
	 */
	int numDrones;

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
    * Constructor.
    */
    MadaraQuadrotorControl(int droneId);

    /**
    * Destructor.
    */
    ~MadaraQuadrotorControl();

    /**
    * Increments the number of drones using this controller, for destruction purposes.
    */
	void incrementNumDrones() { numDrones++; };

    /**
    * Decrements the number of drones using this controller, for destruction purposes.
    */
	void decrementNumDrones() { numDrones--; };

    /**
    * Initialize the drone variables.
    */
    void initInternalData(int droneId);

    /**
    * Clean up the object. True if it cleaned it up, false if it was not possible due to other
	* drones still referencing it.
    */
    bool terminate();

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
