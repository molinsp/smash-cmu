/********************************************************************* 
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

/**
 * @file    helpers.c
 * @author  Subhav Pradhan (subhav.m.pradhan@vanderbilt.edu)
 * 
 * This file contains implementation of helper functions defined 
 * in helper.h file.
 **/

#include <drk.h>
#include <helpers.h>

void set_ambient_temp (input_param_struct* input_params)
{
  int i, row, col;

  for (i = 0; i < MAX_SAMPLE_SIZE; i++)
  {
    // Call to grab_thermal_copy method.
    grab_thermal_copy();

    // Loop through the above obtained (valid) frame and determine the
    // ambient temperature range.
    for (row = 0; row < 8; row++)
    {
      for (col = 0; col < 8; col++)
      {
        if (THERMAL_DATA[row][col] < AMBIENT_TEMP_MIN || AMBIENT_TEMP_MIN == 0)
          AMBIENT_TEMP_MIN = THERMAL_DATA[row][col];

        if (THERMAL_DATA[row][col] > AMBIENT_TEMP_MAX)
          AMBIENT_TEMP_MAX = THERMAL_DATA[row][col];
      }
    }
  }

  // Apply error ranges to the above calculated ambient temperature range.
  AMBIENT_TEMP_MIN = AMBIENT_TEMP_MIN - input_params->error_limit;
  AMBIENT_TEMP_MAX = AMBIENT_TEMP_MAX + input_params->error_limit;

  // Once the error ranges have been applied make sure the ambient temperature
  // range is not greater than 10.
  if ((AMBIENT_TEMP_MAX - AMBIENT_TEMP_MIN) > 10)
    // If ambient temperature range is > 10 then increase the minimum such that
    // the range is not greater than 10.
     AMBIENT_TEMP_MIN = AMBIENT_TEMP_MIN + (AMBIENT_TEMP_MAX - AMBIENT_TEMP_MIN - 10);

  // Print the final ambient temperature.
  printf("Final ambient min: %6.2f\n", AMBIENT_TEMP_MIN);
  printf("Final ambient max: %6.2f\n", AMBIENT_TEMP_MAX);
}

void set_on_human_detection()
{
  HUMAN_DETECTED_FPTR = spin_drone;
}

void spin_drone()
{
  drk_gyro_goto_relative_heading(179);
}

void display_help()
{
  printf("-s | Human detection strategy. Expected input:[b|s].\n");
  printf("-h | Height (in meters) that a drone needs to acquire before running detection algorithms. Expected input: Double value.\n");
  printf("-e | Error bound to account for sensor noise. Expected input: Integer value.\n");
  printf("-l | Expected lower bound of human temperature. Expected input: Double value.\n");
  printf("-u | Expected upper bound of human temperature. Expected input: Double value.\n");
}

void print_matrix_model()
{
  int row, col;
  for(row = 0; row < 8; row++) 
  {
    for(col = 0; col < 8; col++)
      printf("(%6.2f,%6.2f|%6.2f) ", RANGE_INFO[row][col].min, RANGE_INFO[row][col].max, THERMAL_DATA[row][col]);
      
    printf("\n");
  }
  printf("\n\n");
}

void print_matrix()
{
  int row, col;
  printf("\n");
  for(row = 0; row < 8; row++)
  {
    for(col = 0; col < 8; col++)
      printf("%6.2f\t", THERMAL_DATA[row][col]);

    printf("\n");
  }
  printf("\n\n");
}

int detect_human (input_param_struct* input_params, double temp)
{
  if (temp >= input_params->human_temp_min && temp <= input_params->human_temp_max)
    return 1;

  return 0;
}
