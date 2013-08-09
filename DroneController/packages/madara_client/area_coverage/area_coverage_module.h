/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * area_coverage_module.h - Declares the module for area coverage.
 *********************************************************************/

#ifndef _AREA_COVERAGE_MODULE_H
#define _AREA_COVERAGE_MODULE_H

#include "module.h"

namespace SMASH
{
	namespace AreaCoverage
	{
		/**
		  * Class that implements the IModule interface for area coverage.
		  */
		class AreaCoverageModule: public IModule
		{
		public:
			virtual void initialize(Madara::Knowledge_Engine::Knowledge_Base &knowledge);
			virtual void cleanup(Madara::Knowledge_Engine::Knowledge_Base &knowledge);
			virtual std::string get_core_function();
		};
	}
}

#endif
