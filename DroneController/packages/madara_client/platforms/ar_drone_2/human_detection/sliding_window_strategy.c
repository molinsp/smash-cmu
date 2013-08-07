/********************************************************************* 
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

/**
 * @file    sliding_window_strategy.c
 * @author  Subhav Pradhan (subhav.m.pradhan@vanderbilt.edu)
 * 
 * This file contains implementation of the sliding window strategy to 
 * detect human.
 **/

#include <stdio.h>
#include <helpers.h>

int sliding_window_strategy (input_param_struct* input_params, int result_map[][8])
{
  // Declare local variables.
  int row, col, i, rt_val;
  int human_count = 0;
  int debug_verbose = 0;

  // Check environment variable for verbose debugging and set flag appropriately.
  if (getenv("DEBUG_VERBOSE") != 0)
    if (strcmp (getenv("DEBUG_VERBOSE"), "1") == 0)
      debug_verbose = 1;
	
  // Initialize content of 2D model array to 0.
  memset(&RANGE_INFO, 0, sizeof(RANGE_INFO));

  /* Construct the model using above initialized array.*/
  for(i = 0; i < MAX_BUFFER_SIZE; i++)
  {
    grab_thermal_copy();

    // Check environment variable for verbose debugging before printing.
    if(debug_verbose == 1)
    {      
      printf("Found new valid frame: %i\n", FRAME_NUMBER);
      print_matrix();
    }
      
    // Construct the model using above obtained frame.
	  for(row = 0; row < 8; row++) 
    {
		  for(col = 0; col < 8; col++)
      {
        if(RANGE_INFO[row][col].min == 0 || THERMAL_DATA[row][col] < RANGE_INFO[row][col].min)
          RANGE_INFO[row][col].min = THERMAL_DATA[row][col];

        if(RANGE_INFO[row][col].max < THERMAL_DATA[row][col])
          RANGE_INFO[row][col].max = THERMAL_DATA[row][col];
      }
	  }
  
    // Call to grab_thermal_copy method. This copy is used
    // to detect anomaly.
	  grab_thermal_copy();
            
    // Check environment variable for verbose debugging before printing.
    if(debug_verbose == 1)
    {
      printf("Using frame: %i for detection\n", FRAME_NUMBER);
      print_matrix_model();
    }

    // Detect anomaly.
    for(row = 0; row < 8; row++)
    {
      for(col = 0; col < 8; col++)
      {
        if(THERMAL_DATA[row][col] < RANGE_INFO[row][col].min - input_params->error_limit || THERMAL_DATA[row][col] > RANGE_INFO[row][col].max + input_params->error_limit)
        {
          // Check environment variable for verbose debugging before printing.
          if(debug_verbose == 1)
            printf("For frame: %d, anomaly detected @ (%i, %i) Model range: (%6.2f,%6.2f), Frame value:%6.2f \n", FRAME_NUMBER, 
                                                                                                                  row, 
                                                                                                                  col, 
                                                                                                                  RANGE_INFO[row][col].min, 
                                                                                                                  RANGE_INFO[row][col].max, 
                                                                                                                  THERMAL_DATA[row][col]);

          // Call method to detect human. The method returns an int (0 or 1).
          rt_val = detect_human(input_params, THERMAL_DATA[row][col]);
              
          // If the above method returns 1, that means human has been 
          // detected.
          if (rt_val == 1)
          {
            result_map[row][col] = 1;
            human_count++;
            printf("Human Count: %i\n", human_count);
            printf("Human detected ");
            printf("in frame: %d @ (%i, %i) Model range: (%6.2f, %6.2f), Frame value:%6.2f\n", FRAME_NUMBER,                                                                                         
                                                                                               row,
                                                                                               col,
                                                                                               RANGE_INFO[row][col].min,
                                                                                               RANGE_INFO[row][col].max,
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
  }
  return human_count;
}
