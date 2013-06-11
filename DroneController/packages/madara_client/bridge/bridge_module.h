/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * MadaraBridgeManager.h - Declares an module to handle the Madara logic
 *   of building a bridge and finding the positions in it for drones.
 *
 * ASSUMPTIONS:
 * - Device IDs start from 0 and go up from there.
 * - Bridge IDs start from 0 and go up from there.
 *********************************************************************/

#ifndef _MADARA_BRIDGE_MANAGER_H
#define _MADARA_BRIDGE_MANAGER_H

#include "madara/knowledge_engine/Knowledge_Base.h"
#include <map>
#include <string>
#include "BridgeAlgorithm.h"

namespace SMASH
{
    namespace Bridge
    {
        /**
         * Sets the knowledge base and initalizes expressions and functions. Must be called once for the module.
         * @param knowledge	Knowledge base object, used to define functions and compile expressions.
         **/
	    void initialize(Madara::Knowledge_Engine::Knowledge_Base &knowledge);

	    /**
         * Method used to get a string with the main call to the bridge funtionality.
         * @return  A string that can be evaluted to call the logic to check and do bridge building.
         **/
	    std::string get_core_function();

	    /**
         * Method used to return a Madara function call used to setup any simulated data required to connect with simulator.
         * @return  A string that can be evaluted to call the logic to prepare data from the simulator.
         **/
        std::string get_sim_setup_function();

	    /**
         * Method used to set up a simple test scenario.
         **/
	    void setupBridgeTest(Madara::Knowledge_Engine::Knowledge_Base &knowledge);
    }
}

#endif