/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * coverage_tracker.h - Declares functions for tracking coverage.
 *********************************************************************/

#ifndef _COVERAGE_TRACKER_H
#define _COVERAGE_TRACKER_H

#include "madara/knowledge_engine/Knowledge_Base.h"

namespace SMASH
{
  namespace AreaCoverage
  {
      Madara::Knowledge_Record madaraSetupCoverageTracking(
        Madara::Knowledge_Engine::Variables &variables);

      Madara::Knowledge_Record madaraUpdateTracking(
        Madara::Knowledge_Engine::Function_Arguments &args, 
        Madara::Knowledge_Engine::Variables &variables);
      
      Madara::Knowledge_Record madaraUpdateCoveragePercentage(
        Madara::Knowledge_Engine::Variables &variables);
  }
}

#endif
