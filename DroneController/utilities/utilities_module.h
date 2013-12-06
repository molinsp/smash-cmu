/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * utilities_module.h - Declares the module for utilities.
 *********************************************************************/

#ifndef _UTILITIES_MODULE_H
#define _UTILITIES_MODULE_H

#include "module.h"

namespace SMASH
{
	namespace Utilities
	{
		/**
		  * Class that implements the IModule interface for utilities.
		  */
		class UtilitiesModule: public IModule
		{
		public:
            // Interface functions.
			virtual void initialize(Madara::Knowledge_Engine::Knowledge_Base &knowledge);
			virtual void cleanup(Madara::Knowledge_Engine::Knowledge_Base &knowledge);
			virtual std::string get_core_function();
		};
	}
}

#endif
