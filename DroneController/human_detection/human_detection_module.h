/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * human_detection_module.h - Declares the module for human detection.
 *********************************************************************/

#ifndef _HUMAN_DETECTION_MODULE_H
#define _HUMAN_DETECTION_MODULE_H

#include "module.h"

namespace SMASH
{
	namespace HumanDetection
	{
		/**
		  * Class that implements the IModule interface for human detection.
		  */
		class HumanDetectionModule: public IModule
		{
		public:
			virtual void initialize(Madara::Knowledge_Engine::Knowledge_Base &knowledge);
			virtual void cleanup(Madara::Knowledge_Engine::Knowledge_Base &knowledge);
			virtual std::string get_core_function();
		};
	}
}

#endif //_HUMAN_DETECTION_MODULE_H
