/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * bridge_module.h - Declares the module for bridge building.
 *********************************************************************/

#ifndef _BRIDGE_MODULE_H
#define _BRIDGE_MODULE_H

#include "module.h"

namespace SMASH
{
	namespace Bridge
	{
		/**
		  * Class that implements the IModule interface for bridge building.
		  */
		class BridgeModule: public IModule
		{
		public:
			virtual void initialize(Madara::Knowledge_Engine::Knowledge_Base &knowledge);
			virtual void cleanup(Madara::Knowledge_Engine::Knowledge_Base &knowledge);
			virtual std::string get_core_function();
		};
	}
}

#endif
