/*******************************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
******************************************************************************/

/*******************************************************************************
* HumanDetection.h - Base class that contains common interface for different
*                    human detection strategies
******************************************************************************/

#ifndef _HUMAN_DETECTION_H_
#define _HUMAN_DETECTION_H_

#define MAX_SAMPLE_SIZE 5
#define ERROR_LIMIT 2

namespace SMASH 
{ 
  namespace HumanDetection 
  {
    /**
    * @brief Represents a base class for human detection algorithms.
    **/
    class HumanDetection
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
        virtual int detect_human (int[8][8], void (*on_human_detected)()) = 0;

        /**
         * Check if a given temperature falls within expected human temperature range.
         *
         * @param temp  Temperature that needs to be checked.
         *
         * @return      True if given temperature falls within expected human temperature
         *              False otherwise.
         **/
        bool check_if_human (double);
    };
  }
}

#endif // _HUMAN_DETECTION_H_
