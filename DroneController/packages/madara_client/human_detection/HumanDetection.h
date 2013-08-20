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

#define MAX_SAMPLE_SIZE 5   // Max number of samples used to determine ambient 
                            // temperature for BasicStrategy as well as build 
                            // model for SlidingWindowStrategy.

#define AMBIENT_RANGE 5     // Used to calculate ambient temperature range when
                            // user gives an extected environment temperature.

#define ERROR_LIMIT 2       // Used to account for error when calculating ambient
                            // temperature for BasicStrategy as well as building
                            // model for SlidingWindowStrategy.   

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
         * Constructor.
         **/
        HumanDetection();

        /**
         * Destructor.
         **/
        virtual ~HumanDetection();

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
        virtual int detect_human (double thermal_buffer[8][8], 
                                  double curr_height, 
                                  int result_map[8][8], 
                                  void (*on_human_detected)()) = 0;
  
        /**
         * Check if a given temperature falls within expected human temperature range.
         *
         * @param temp        Temperature that needs to be checked.
         * @param curr_height Current height/altitude of the drone.
         *
         * @return            True if given temperature falls within expected human 
         *                    temperature. False otherwise.
         **/
        bool check_if_human (double temp, double curr_height);
    };
  }
}

#endif // _HUMAN_DETECTION_H_
