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
#include "sensors/platform_sensors.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

using namespace SMASH::HumanDetection;

int BasicStrategy::detect_human (int result_map[8][8], void (*on_human_detected)())
{
  // Declare local variables.
  int ambient_temp_min, ambient_temp_max, human_count, debug_verbose;
  double buffer[8][8];

  // Check environment variable for verbose debugging and set flag appropriately.
  if (getenv("DEBUG_VERBOSE") != 0)
    if (strcmp (getenv("DEBUG_VERBOSE"), "1") == 0)
      debug_verbose = 1;

  // Get thermal data.
  read_thermal (buffer);

  // Calculate ambient temperature range. 
  calculate_ambient_temp (ambient_temp_min, ambient_temp_max);

  // Detect anomaly.
  for (int row = 0; row < 8; row++)
  {
    for (int col = 0; col < 8; col++)
    {
      if (buffer[row][col] < ambient_temp_min || buffer[row][col] > ambient_temp_max)
      {
        // Check environment variable for verbose debugging before printing.
        if (debug_verbose == 1)
          printf("Anomaly detected @ (%i, %i) Range: (%6.2f,%6.2f), Frame value:%6.2f \n", row,
                                                                                           col,
                                                                                           ambient_temp_min,
                                                                                           ambient_temp_max,
                                                                                           buffer[row][col]);


        // Check if temperature falls in expected human temperature range.
        if (HumanDetection::check_if_human (buffer[row][col]))
        {
          // Human temperature detected so mark that location as "1" in result_map.
          result_map[row][col] = 1;

          human_count++;
          printf("Human Count: %i\n", human_count);
          printf("Human detected ");
          printf("@ (%i, %i) Range: (%6.2f,%6.2f), Frame value:%6.2f \n", row,
                                                                          col,
                                                                          ambient_temp_min,
                                                                          ambient_temp_max,
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

void BasicStrategy::calculate_ambient_temp (int& min, int& max)
{
  double buffer[8][8];
  for (int i = 0; i < MAX_SAMPLE_SIZE; i++)
  {
    // Call to read thermal buffer.
    read_thermal (buffer);

    // Use the above obtained (valid) buffer and determine the ambient temperature range.
    for (int row = 0; row < 8; row++)
    {
      for (int col = 0; col < 8; col++)
      {
        if (buffer[row][col] < min || min == 0)
          min = buffer[row][col];

        if (buffer[row][col] > max)
          max = buffer[row][col];
      }
    }
  }

  // Apply error ranges to the above calculated ambient temperature range.
  min = min - ERROR_LIMIT;
  max = max + ERROR_LIMIT;

  // Once the error ranges have been applied make sure the ambient temperature
  // range is not greater than 10.
  if ((max - min) > 10)
    // If ambient temperature range is > 10 then increase the minimum such that
    // the range is not greater than 10.
     min = min + (max - min - 10);

  // Print the final ambient temperature.
  printf("Final ambient min: %6.2f\n", min);
  printf("Final ambient max: %6.2f\n", max);
  
}
