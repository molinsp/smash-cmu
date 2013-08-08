/*******************************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 ******************************************************************************/

/*******************************************************************************
 * SlidingWindowStrategy.h - 
 ******************************************************************************/

#ifndef _SLIDING_WINDOW_STRATEGY_H
#define _SLIDING_WINDOW_STRATEGY_H

#include "HumanDetection.h"

namespace SMASH 
{ 
  namespace HumanDetection
  {
    // Metadata to store temperature range of individual buffer pixel. This is
    // used to construct models used by SlidingWindowStrategy.
    struct range_str
    {
      double min;
      double max;
    };      
    
    /**
     * @brief Represents sliding window strategy for human detection.
     **/
    class SlidingWindowStrategy: public SMASH::HumanDetection::HumanDetection
    {
      public:
        /**
         * Detect human.
         *
         * @param result_map        Integer array that will be filled by this method
         *                          with "1" in locations where humans were detected 
         *                          and "0" in location where there were no humans.   
         * @param on_human_detected Callback function which will be invoked once human
         *                          is detected.
         *
         * @return                  Number of pixels with human temperature. 
         **/
        virtual int detect_human (int[8][8], void (*on_human_detected)()); 
    };  
  }
}

#endif // _SLIDING_WINDOW_STRATEGY_H_
