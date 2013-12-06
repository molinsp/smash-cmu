/*******************************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 ******************************************************************************/

/*******************************************************************************
 * SlidingWindowStrategy.cpp - 
 ******************************************************************************/

#include "SlidingWindowStrategy.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

using namespace SMASH::HumanDetection;

SlidingWindowStrategy::SlidingWindowStrategy():frames_sampled(0)
{
  memset(&range_info, 0, sizeof(range_info));
}

SlidingWindowStrategy::~SlidingWindowStrategy()
{}

int SlidingWindowStrategy::detect_human(double thermal_buffer[8][8],
                                        double curr_height,
                                        int result_map[8][8], 
                                        void (*on_human_detected)())
{
  printf("SlidingWindowStrategy::detect_human");

  // Declare local variables.
  int human_count = 0, debug_verbose = 0;

  // Check environment variable for verbose debugging and set flag appropriately.
  if (getenv("DEBUG_VERBOSE") != 0)
    if (strcmp (getenv("DEBUG_VERBOSE"), "1") == 0)
      debug_verbose = 1;

  
  if (frames_sampled < MAX_SAMPLE_SIZE)
  {    
    for (int row = 0; row < 8; ++row) 
    {
      for (int col = 0; col < 8; ++col)
      {
        if (range_info[row][col].min == 0 || thermal_buffer[row][col] < range_info[row][col].min)
          range_info[row][col].min = thermal_buffer[row][col];

        if (range_info[row][col].max < thermal_buffer[row][col])
          range_info[row][col].max = thermal_buffer[row][col];
      }
    }
    frames_sampled++;
  }
  else
  {
    frames_sampled = 0;    

    // Detect anomaly.
    for (int row = 0; row < 8; ++row)
    {
      for (int col = 0; col < 8; ++col)
      {
        if (thermal_buffer[row][col] < range_info[row][col].min - ERROR_LIMIT || thermal_buffer[row][col] > range_info[row][col].max + ERROR_LIMIT)
        {
          // Check environment variable for verbose debugging before printing.
          if(debug_verbose == 1)
            printf("Anomaly detected @ (%i, %i) Model range: (%6.2f,%6.2f), Frame value:%6.2f \n", row, 
                                                                                                   col, 
                                                                                                   range_info[row][col].min, 
                                                                                                   range_info[row][col].max, 
                                                                                                   thermal_buffer[row][col]);

          
          // Check if temperature falls in expected human range.    
          if (HumanDetection::check_if_human (thermal_buffer[row][col], curr_height))
          {
            // Human temperature detected so mark that location as "1" in result_map.
            result_map[row][col] = 1;

            human_count++;
          
            printf("Human Count: %i\n", human_count);
            printf("At height %6.2f ", curr_height);
            printf("Human Detected@ (%i, %i) Model range: (%6.2f, %6.2f), Frame value:%6.2f\n", row,
                                                                                                col,
                                                                                                range_info[row][col].min,
                                                                                                range_info[row][col].max,
                                                                                                thermal_buffer[row][col]);
          }
          else
          {
            // Human temperature not detected so mark that location as "0" in result_map.
            result_map[row][col] = 0;

            if (debug_verbose == 1)
              printf("Non-Human object detected with temperature:%6.2f \n", thermal_buffer[row][col]);
          }
        }
        else
          // Not an anomaly so mark that location as "0" as well in result_map.
          result_map[row][col] = 0;
      }
    }
  }

  // If human count is greater than 0 then we know that human has been detected.
  // So invoke the callback function.
  if (human_count > 0)
    on_human_detected();

  return human_count;
}
