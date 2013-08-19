/*******************************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 ******************************************************************************/

/*******************************************************************************
 * BasicStrategy.cpp - 
 ******************************************************************************/

#include "BasicStrategy.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

using namespace SMASH::HumanDetection;

BasicStrategy::BasicStrategy(double min, double max): ambient_min (min),
                                                      ambient_max (max)
{
}

BasicStrategy::~BasicStrategy()
{}

int BasicStrategy::detect_human (double thermal_buffer[8][8],
                                 double curr_height,
                                 int result_map[8][8], 
                                 void (*on_human_detected)())
{
  printf("BasicStrategy::detect_human\n");  

  // Declare local variables.
  int human_count = 0, debug_verbose = 0;

  // Check environment variable for verbose debugging and set flag appropriately.
  if (getenv("DEBUG_VERBOSE") != 0)
    if (strcmp (getenv("DEBUG_VERBOSE"), "1") == 0)
      debug_verbose = 1;

  // Detect anomaly.
  for (int row = 0; row < 8; row++)
  {
    for (int col = 0; col < 8; col++)
    {
      if (thermal_buffer[row][col] < ambient_min || thermal_buffer[row][col] > ambient_max)
      {
        // Check environment variable for verbose debugging before printing.
        if (debug_verbose == 1)
          printf("Anomaly detected @ (%i, %i) Range: (%6.2f,%6.2f), Frame value:%6.2f \n", row,
                                                                                           col,
                                                                                           ambient_min,
                                                                                           ambient_max,
                                                                                           thermal_buffer[row][col]);


        // Check if temperature falls in expected human temperature range.
        if (HumanDetection::check_if_human (thermal_buffer[row][col], curr_height))
        {
          // Human temperature detected so mark that location as "1" in result_map.
          result_map[row][col] = 1;

          human_count++;
          printf("Human Count: %i\n", human_count);
          printf("Human detected ");
          printf("@ (%i, %i) Range: (%6.2f,%6.2f), Frame value:%6.2f \n", row,
                                                                          col,
                                                                          ambient_min,
                                                                          ambient_max,
                                                                          thermal_buffer[row][col]);

          // Invoke callback function for post human detection action.
          on_human_detected();
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
  return human_count;
}
