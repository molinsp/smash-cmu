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
         * Constructor. 
         **/
        SlidingWindowStrategy();

        /**
         * Destructor.
         **/
        ~SlidingWindowStrategy();

        /**
         * Detect human.
         *
         * @param thermal_buffer    Thermal buffer which will be analyzed for human
         *                          presence.
         * @param curr_height       Current height/altitude of the drone.
         * @param result_map        Integer array that will be filled by this method
         *                          with "1" in locations where humans were detected 
         *                          and "0" in location where there were no humans.
         * @param on_human_detected Callback function which will be invoked once human
         *                          is detected.
         *
         * @return                  Number of pixels with human temperature. 
         **/
        int detect_human (double thermal_buffer[8][8],
                          double curr_height,
                          int result_map[8][8], 
                          void (*on_human_detected)());

      private:
        int frames_sampled;

        range_str range_info[8][8]; 
    };  
  }
}

#endif // _SLIDING_WINDOW_STRATEGY_H_
