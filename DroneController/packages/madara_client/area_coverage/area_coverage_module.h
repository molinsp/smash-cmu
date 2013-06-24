/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * area_coverage_module.h - Declares an module to handle the Madara logic
 *   of covering a certain area.
 *********************************************************************/

#ifndef _AREA_COVERAGE_MODULE_H
#define _AREA_COVERAGE_MODULE_H

#include "madara/knowledge_engine/Knowledge_Base.h"
#include <string>

namespace SMASH
{
    namespace AreaCoverage
    {
        /**
         * Sets the knowledge base and initalizes expressions and functions. Must be called once for the module.
         * @param knowledge	Knowledge base object, used to define functions and compile expressions.
         **/
	    void initialize(Madara::Knowledge_Engine::Knowledge_Base &knowledge);

	    /**
         * Method used to get a string with the main call to the area coverage funtionality.
         * @return  A string that can be evaluted to call the logic to check and do area coverage.
         **/
	    std::string get_core_function();

	    /**
         * Method used to return a Madara function call used to setup any simulated data required to connect with simulator.
         * @return  A string that can be evaluted to call the logic to prepare data from the simulator.
         **/
        std::string get_sim_setup_function();

	    /**
         * Method used to setup a search test environment in Madara.
         **/
        void setupSearchTest(Madara::Knowledge_Engine::Knowledge_Base &knowledge);
    }
}

#endif