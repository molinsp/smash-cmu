/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * MadaraBridgeManager.h - Declares an object to handle the Madara logic
 *   of building a bridge and finding the positions in it for drones.
 *
 * ASSUMPTIONS:
 * - The current logic assumes that when a drone doing area coverage
 *   finds a person, it stops there.
 * - The logic also assumes that when the user asks the network to form
 *   a bridge, the user will indicate the ID of the stopped drone that 
 *   is to be used as a source. The controller should also provide its
 *   ID, or let the user select one (see Main Input section below).
 *********************************************************************/

#ifndef _MADARA_BRIDGE_MANAGER_H
#define _MADARA_BRIDGE_MANAGER_H

#include "madara/knowledge_engine/Knowledge_Base.h"
#include <map>
#include <string>

/**
 * @brief Define the ids for the expressions that will exist to accomplish different aspects of bridge building.
 **/
enum BridgeMadaraExpressionId 
{
    // Expression to call function to update the positions of the drones available for a bridge.
	BE_FIND_AVAILABLE_DRONES_POSITIONS
};

/**
 * @brief Manages the Madara expressions required for bridge building.
 * It is a singleton so it can be called by Madara through an external function
 * and still maintain its internal state.
 **/
class MadaraBridgeManager
{
public:
	// Get an instance of the singleton.
	static MadaraBridgeManager& MadaraBridgeManager::getInstance();

	/**
	 * Sets the knowledge base and initalizes expressions and functions. Must be called once for the singleton.
	 * @param knowledge	Knowledge base object, used to define functions and compile expressions.
	 **/
	void initialize(Madara::Knowledge_Engine::Knowledge_Base &knowledge);

	/**
     * Method used to get a string with the main call to the bridge funtionality.
     * @return  A string that can be evaluted to call the logic to check and do bridge building.
     **/
	std::string getMainLogicMadaraCall();

	/**
     * Method that invocates the functionality of finding our position in a bridge, which will be called from Madara when required.
     * Will be called from an external Madara function.
     * @return  Always returns true (1).
     **/
	Madara::Knowledge_Record findPositionInBridge (Madara::Knowledge_Engine::Function_Arguments &args,
             Madara::Knowledge_Engine::Variables &variables);

	/**
     * Method used to set up a simple test scenario.
     **/
	void setupBridgeTest();
private:
	// The knowledge engine used for the evaluations.
	Madara::Knowledge_Engine::Knowledge_Base m_knowledge;

	// Map of Madara expressions used in bridge building.
	std::map<BridgeMadaraExpressionId, Madara::Knowledge_Engine::Compiled_Expression> m_expressions;

    // Private methods used to setup internal functions and compiled expressions.
	void compileExpressions();
	void defineFunctions();

	// Private methods to allow singleton to exist.
	MadaraBridgeManager() {};
	MadaraBridgeManager(MadaraBridgeManager const&);
	void operator=(MadaraBridgeManager const&);
};

#endif