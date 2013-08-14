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
#include "sensors/platform_sensors.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

using namespace SMASH::HumanDetection;

int SlidingWindowStrategy::detect_human(int result_map[8][8], void (*on_human_detected)())
{
  printf("SlidingWindowStrategy::detect_human");

  // Declare local variables.
  int human_count, debug_verbose;

  double buffer[8][8];

  range_str range_info[8][8];

  // Check environment variable for verbose debugging and set flag appropriately.
  if (getenv("DEBUG_VERBOSE") != 0)
    if (strcmp (getenv("DEBUG_VERBOSE"), "1") == 0)
      debug_verbose = 1;
	
  // Initialize content of 2D model array to 0.
  memset(&range_info, 0, sizeof(range_info));

  /* Construct the model using above initialized array.*/
  for (int i = 0; i < MAX_SAMPLE_SIZE; i++)
  {
    // Read thermal buffer.
    read_thermal (buffer);
      
    // Construct the model using above obtained frame.
	  for (int row = 0; row < 8; row++) 
    {
		  for (int col = 0; col < 8; col++)
      {
        if (range_info[row][col].min == 0 || buffer[row][col] < range_info[row][col].min)
          range_info[row][col].min = buffer[row][col];

        if (range_info[row][col].max < buffer[row][col])
          range_info[row][col].max = buffer[row][col];
      }
	  }
  }
  
  // Read thermal buffer for detectionl.
	read_thermal (buffer);

  // Detect anomaly.
  for (int row = 0; row < 8; row++)
  {
    for (int col = 0; col < 8; col++)
    {
      if (buffer[row][col] < range_info[row][col].min - ERROR_LIMIT || buffer[row][col] > range_info[row][col].max + ERROR_LIMIT)
      {
        // Check environment variable for verbose debugging before printing.
        if(debug_verbose == 1)
          printf("Anomaly detected @ (%i, %i) Model range: (%6.2f,%6.2f), Frame value:%6.2f \n", row, 
                                                                                                   col, 
                                                                                                   range_info[row][col].min, 
                                                                                                   range_info[row][col].max, 
                                                                                                   buffer[row][col]);

          
        // Check if temperature falls in expected human range.    
        if (check_if_human (buffer[row][col]))
        {
          // Human temperature detected so mark that location as "1" in result_map.
          result_map[row][col] = 1;

          human_count++;
          printf("Human Count: %i\n", human_count);
          printf("Human detected ");
          printf("@ (%i, %i) Model range: (%6.2f, %6.2f), Frame value:%6.2f\n", row,
                                                                                col,
                                                                                range_info[row][col].min,
                                                                                range_info[row][col].max,
                                                                                buffer[row][col]);
          
          // Invoke callback function for post human detection action.
          on_human_detected();
        }
        else
        {
          // Human temperature not detected so mark that location as "0" in result_map.
          result_map[row][col] = 0;

          if (debug_verbose == 1)
            printf("Non-Human object detected with temperature:%6.2f \n", buffer[row][col]);
        }
      }
      else
        // Not an anomaly so mark that location as "0" as well in result_map.
        result_map[row][col] = 0;
    }
  }
  return human_count;
}

