/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

#ifndef _MADARA_QUADROTOR_CONTROL_H_
#define _MADARA_QUADROTOR_CONTROL_H_

#include "madara/knowledge_engine/Knowledge_Base.h"
#include "Location.h"

#include <vector>
#include <string>
#include <map>

using std::string;

namespace SMASHSim
{
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
        * Clears the current command locally from Madara.
        */
        void clearCommand(std::string droneIdString);

	    /**
	     * Stores the amount of drones that are using this controller, for correct deinitialization.
	     */
	    int numDrones;

        // Stores references to the thermal variables for quick access.
        std::map<std::string, Madara::Knowledge_Engine::Variable_Reference> variables;

    public:
        /**
        * stores a command to the quadrotor
        */
        struct Command
        {
            std::string m_command; // actual command {move, takeoff, land}
            Location m_loc;         // location where command takes place
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
        * Update position in the knowledge base.
        */
        void updateQuadrotorPosition(const int& id, const Location& location);

        /**
        * get new command from knowledge base
        */
        Command* getNewCommand(int droneId);

        /**
        * Set a new thermal scan to the knowledge base.
        */
        void setNewThermalScan(int droneId, std::string thermalBuffer, int thermalHeight, int thermalWidth);
    };

}

#endif // _MADARA_QUADROTOR_CONTROL_H_
