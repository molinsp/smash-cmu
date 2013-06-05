/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * MadaraBridgeManager.h - Declares
 *********************************************************************/

#ifndef _MADARA_BRIDGE_MANAGER_H
#define _MADARA_BRIDGE_MANAGER_H

#include "madara/knowledge_engine/Knowledge_Base.h"
#include <map>
#include <string>

// Define the ids for the expressions that will exist to accomplish different aspects of bridge building.
enum BridgeMadaraExpressionId 
{
	BE_FIND_AVAILABLE_DRONES_POSITIONS
};

// Manages the Madara expressions required for bridge building.
// It is a singleton so it can be called by Madara through an external function
// and still maintain its internal state.
class MadaraBridgeManager
{
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
public:
	// Get an instance of the singleton.
	static MadaraBridgeManager& MadaraBridgeManager::getInstance();

	/**
	 * Sets the knowledge base and initalizes expressions and functions.
	 * @param knowledge	Knowledge base object, used to define functions and compile expressions.
	 **/
	void initialize(Madara::Knowledge_Engine::Knowledge_Base &knowledge);

    // Method used to get a string with the main call to the bridge funtionality.
	std::string getMainLogicMadaraCall();

    // Method that invocates the bridge position finding functionality, which will be called from Madara when required.
	Madara::Knowledge_Record findPositionInBridge (Madara::Knowledge_Engine::Function_Arguments &args,
             Madara::Knowledge_Engine::Variables &variables);

    // Method used to set up a simple test scenario.
	void setupBridgeTest();
};

#endif