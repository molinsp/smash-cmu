/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * module.h - Declares the interface that every Madara module should implement.
 *********************************************************************/

#ifndef _MODULE_H
#define _MODULE_H

#include "madara/knowledge_engine/Knowledge_Base.h"
#include <string>

namespace SMASH
{
	class IModule
	{
	public:
		/**
		 * Sets the knowledge base and initalizes expressions and functions. Must be called once for the module.
		 * @param knowledge	Knowledge base object, used to define functions and compile expressions.
		 **/
		virtual void initialize(Madara::Knowledge_Engine::Knowledge_Base &knowledge) = 0;

		/**
		 * Cleans up the module.
		 * @param knowledge	Knowledge base object used in the module.
		 **/
		virtual void cleanup(Madara::Knowledge_Engine::Knowledge_Base &knowledge) = 0;

	   /**
		 * Method used to get a string with the main call to the funtionality. This can be used in Madara logic
		 * when he functionality of this module needs to be triggered.
		 * @return  A string containing a Madara function call to trigger the functionality of this module.
		 **/
		virtual std::string get_core_function() = 0;
	};
}

#endif
