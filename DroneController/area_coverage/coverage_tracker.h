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

// Used to indicate if tracking variables have to be set or reset (when it is 1).
#define MV_START_COVERAGE_TRACKING ".area_coverage.start_tracking"

// Function to update the current coverage value.
#define MF_UPDATE_COVERAGE_TRACKING "area_coverage_updateCoverageTracking"

namespace SMASH
{
  namespace AreaCoverage
  {
      Madara::Knowledge_Record madaraUpdateCoverageTracking(
        Madara::Knowledge_Engine::Function_Arguments &args, 
        Madara::Knowledge_Engine::Variables &variables);
  }
}

#endif
