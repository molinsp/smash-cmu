/********************************************************************* 
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * area_coverage_module.cpp - Defines the manager for area coverage.
 *********************************************************************/

#include <vector>
#include <map>
#include "area_coverage_module.h"
#include "CommonMadaraVariables.h"

using namespace SMASH::AreaCoverage;

// Macro to convert from int to std::string.
#define INT_TO_STR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Madara Variable Definitions
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions.
#define MF_MAIN_LOGIC				"area_coverage_doAreaCoverage"			// Function that checks if there is area coverage to be done, and does it.
#define MF_POPULATE_LOCAL_VARS		"area_coverage_populateLocalVars"		// Function that populates local variables from global ones sent by the simulator.

// Internal bridging variables.

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private variables.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Define the ids for the expressions that will exist to accomplish different aspects of area coverage.
enum AreaCoverageMadaraExpressionId 
{
    // Expression to call function to update the positions of the drones available for coverage.
	BE_FIND_AVAILABLE_DRONES_POSITIONS,
};

// Map of Madara expressions used in bridge building.
static std::map<AreaCoverageMadaraExpressionId, Madara::Knowledge_Engine::Compiled_Expression> m_expressions;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private function declarations.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void defineFunctions(Madara::Knowledge_Engine::Knowledge_Base &knowledge);
static void compileExpressions(Madara::Knowledge_Engine::Knowledge_Base &knowledge);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Initializer, gets the refence to the knowledge base and compiles expressions.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SMASH::AreaCoverage::initialize(Madara::Knowledge_Engine::Knowledge_Base &knowledge)
{
    // Defines internal and external functions.
    defineFunctions(knowledge);

    // Registers all default expressions, to have them compiled for faster access.
    compileExpressions(knowledge);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gets the main logic to be run. This returns a function call that can be included in another block of logic.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string SMASH::AreaCoverage::get_core_function()
{
    return MF_MAIN_LOGIC "()";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gets a call to prepare simulated data. This returns a function call that can be included in another block of logic.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string SMASH::AreaCoverage::get_sim_setup_function()
{
    return MF_POPULATE_LOCAL_VARS "()";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers functions with Madara.
// ASSUMPTION: Drone IDs are continuous, starting from 0.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void defineFunctions(Madara::Knowledge_Engine::Knowledge_Base &knowledge)
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Compiles all expressions to be used by this class.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void compileExpressions(Madara::Knowledge_Engine::Knowledge_Base &knowledge)
{
    // Expression to update the list of available drone positions, simply calls the predefine function.

}


