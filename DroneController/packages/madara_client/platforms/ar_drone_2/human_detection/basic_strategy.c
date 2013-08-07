/********************************************************************* 
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

/**
 * @file    basic_strategy.c
 * @author  Subhav Pradhan (subhav.m.pradhan@vanderbilt.edu)
 * 
 * This file contains implementation of the basic strategy to detect human.
 **/

#include <stdio.h>
#include <helpers.h>

int basic_strategy (input_param_struct* input_params, int result_map[][8])
{
  // Declare local variables.
  int row, col, rt_val;
  int human_count = 0;
  int debug_verbose = 0;

  // Check environment variable for verbose debugging and set flag appropriately.
  if (getenv("DEBUG_VERBOSE") != 0)
    if (strcmp (getenv("DEBUG_VERBOSE"), "1") == 0)
      debug_verbose = 1;

  // Call to grab_thermal_copy method. This copy is used to detect anomaly.
  grab_thermal_copy();

  // Check environment variable for verbose debugging before printing.
  if (debug_verbose == 1)
  {
    printf("Found new valid frame: %i\n", FRAME_NUMBER);
    print_matrix();
  }

  // Detect anomaly.
  for (row = 0; row < 8; row++)
  {
    for (col = 0; col < 8; col++)
    {
      if (THERMAL_DATA[row][col] < AMBIENT_TEMP_MIN || THERMAL_DATA[row][col] > AMBIENT_TEMP_MAX)
      {
        // Check environment variable for verbose debugging before printing.
        if (debug_verbose == 1)
          printf("For frame: %d, anomaly detected @ (%i, %i) Range: (%6.2f,%6.2f), Frame value:%6.2f \n", FRAME_NUMBER,
                                                                                                          row,
                                                                                                          col,
                                                                                                          AMBIENT_TEMP_MIN,
                                                                                                          AMBIENT_TEMP_MAX,
                                                                                                          THERMAL_DATA[row][col]);

        // Call method to detect human. The method returns an int (0 or 1).
        rt_val = detect_human (input_params, THERMAL_DATA[row][col]);

        // If the above method returns 1, that means human has been 
        // detected.
        if (rt_val == 1)
        {
          result_map[row][col] = 1;
          human_count++;
          printf("Human Count: %i\n", human_count);
          printf("Human detected ");
          printf("in frame: %d @ (%i, %i) Range: (%6.2f,%6.2f), Frame value:%6.2f \n", FRAME_NUMBER,
                                                                                       row,
                                                                                       col,
                                                                                       AMBIENT_TEMP_MIN,
                                                                                       AMBIENT_TEMP_MAX,
                                                                                       THERMAL_DATA[row][col]);
        }
        else
        {
          result_map[row][col] = 0;
          if (debug_verbose == 1)
            printf("Non-Human object detected with temperature:%6.2f \n", THERMAL_DATA[row][col]);
        }
      }
      else
        result_map[row][col] = 0;
    }
  }
  return human_count;
}
