/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * sensors_module.h - Declares the module for sensors.
 *********************************************************************/

#ifndef _SENSORS_MODULE_H
#define _SENSORS_MODULE_H

#include "module.h"

namespace SMASH
{
	namespace Sensors
	{
		/**
		  * Class that implements the IModule interface for sensors.
		  */
		class SensorsModule: public IModule
		{
		public:
			virtual void initialize(Madara::Knowledge_Engine::Knowledge_Base &knowledge);
			virtual void cleanup(Madara::Knowledge_Engine::Knowledge_Base &knowledge);
			virtual std::string get_core_function();
		};
	}
}

#endif
