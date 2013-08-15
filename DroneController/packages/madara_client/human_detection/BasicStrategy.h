/*******************************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 ******************************************************************************/

/*******************************************************************************
 * BasicStrategy.h - 
 ******************************************************************************/

#ifndef _BASIC_STRATEGY_H
#define _BASIC_STRATEGY_H

#include "HumanDetection.h"

namespace SMASH 
{ 
  namespace HumanDetection
  {
    /**
     * @brief Represent basic strategy for human detection.
     **/
    class BasicStrategy: public SMASH::HumanDetection::HumanDetection
    {
      public:
        /**
         * Constructor.
         *
         * @param min Minimum ambient temperature.
         * @param max Maximum ambient temperature.
         **/
        BasicStrategy (double min, double max);

        /**
         * Destructor.
         **/
        ~BasicStrategy ();        

        /**
         * Detect human.
         *
         * @param result_map        Integer array that will be filled by this method
         *                          with "1" in locations where humans were detected 
         *                          and "0" in location where there were no humans.   
         * @param curr_height       Current height/altitude of the drone.
         * @param on_human_detected Callback function which will be invoked once human
         *                          is detected.
         *
         * @return                  Number of pixels with human temperature. 
         **/
        int detect_human (int result_map[8][8], double curr_height, void (*on_human_detected)()); 

      private:
        // Minimum ambient temperature.
        double ambient_min;

        // Maximum ambient temperature.
        double ambient_max;
    };  
  }
}

#endif // _BASIC_STRATEGY_H_
