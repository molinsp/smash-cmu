/********************************************************************* 
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

/**
 * @file    human_detection.c
 * @author  Subhav Pradhan (subhav.m.pradhan@vanderbilt.edu)
 * 
 * Desc: This file contains the main (driver) code that reads
 *       user input and calls appropriate strategy in order to 
 *       detect humans. 
 **/

#include <drk.h>
#include <string.h>
#include <helpers.h>

/* Main function.*/
int main (int argc, char* argv[])
{
  // Check if valid number of arguments.
  if (argc != 11)
  {
    // Check if user requested for help.
    if (argc == 2 && (strcmp (argv[1], "-h") == 0 || strcmp (argv[1], "--help") == 0))
    {  
      display_help();
      return 0;
    }
    // If user entered invalid number of arguments and didn't ask for help, display
    // error message and exit.
    else
    {
      printf("ERROR: Incorrect number of arguments \n");
      return -1;
    }
  }
  
  // Declare and initialize struct to store input params.
  input_param_struct input_params;
  memset (&input_params, 0 , sizeof(input_params));

  int i;  

  // Loop through arguments and store them in the struct created above.
  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-s") == 0)
      input_params.strategy = argv[i+1];

    if (strcmp(argv[i], "-h") == 0)
      input_params.height = atof(argv[i+1]);

    if (strcmp(argv[i], "-e") == 0)
      input_params.error_limit = atoi(argv[i+1]);

    if (strcmp(argv[i], "-l") == 0)
      input_params.human_temp_min = atof(argv[i+1]);
    
    if (strcmp(argv[i], "-u") == 0)
      input_params.human_temp_max = atof(argv[i+1]);
  }

  printf("Strategy:%s, Height:%6.2f, Error:%i, Human min:%6.2f, Human max:%6.2f\n", input_params.strategy,
                                                                                    input_params.height,
                                                                                    input_params.error_limit,
                                                                                    input_params.human_temp_min,
                                                                                    input_params.human_temp_max);

  // Initialize drk.
	drk_init();
  
  // Call the flight control method to make the drone hover
  // at a specific height.
  flight_control (&input_params);
	
  // Restore and set exit status.
	drk_restore_termios();
	drk_exit(EXIT_SUCCESS);
	return EXIT_SUCCESS;
}

/** 
 * Method to make the drone hover at a user given height.
 **/
void flight_control(input_param_struct* input_params)
{
  drk_ar_flat_trim();    
  
  drk_takeoff();
  drk_hover(2000);
  
  // While loop to make sure the drone attains user entered height.
  while (drk_ultrasound_altitude() <= input_params->height)
    drk_move_up (0.4, 500, DRK_HOVER);

  sleep (2);
  
  // Create thread that runs the method responsible for detecting which strategy
  // to use for human detection.
  pthread_t tid;
  int rt_val = pthread_create(&tid, NULL, &select_strategy,(void *) input_params);

  // Check thread creation.
  if(rt_val != 0)
    printf("pthread_create failed with return code: %i\n", rt_val);
  else
    printf("Thread created succesfully \n");

  // Hover for a minute and then land.
  drk_hover(60000);
  drk_land();

  /*
  // Following block of code is for indoor testing purpose.
   
  pthread_t tid;
  int rt_val = pthread_create (&tid, NULL, &select_strategy,(void *) input_params);

  if (rt_val != 0)
    printf("pthread_create failed with return code: %i\n", rt_val);
  else
    printf("Thread created succesfully \n");

  sleep (60);
  */
}

void* select_strategy (void* input_params)
{
  // Pointer type conversion. Convert void* to input_param_struct*.
  input_param_struct *input_param_str = input_params;
  
  human_detection_strategy detection_strategy_fptr;
  
  // Check user input stored in a struct and set strategy accordingly.
  if (strcmp(input_param_str->strategy, "b") == 0)
  {
    // Set ambient temperature.
    set_ambient_temp (input_param_str);
    
    // Set strategy to basic_strategy.
    detection_strategy_fptr = basic_strategy;
  }
  
  if (strcmp(input_param_str->strategy, "s") == 0)
    // Set strategy to sliding_window_strategy.
    detection_strategy_fptr = sliding_window_strategy;    

  // Call method that sets the action we want to take when humans are detected.
  set_on_human_detection ();

  int human_count;
  int result_map[8][8];

  while (1)
  {
    // This call invokes the actual detection algorithm.
    human_count = detection_strategy_fptr (input_param_str, result_map);

    // Check if humans detected.
    if (human_count >= 1)
    {
      // Humans detected so, call the post detection method using the
      // appropriate function pointer.
      HUMAN_DETECTED_FPTR;
      
      // Print result_map filled by detection algorithm.
      int row, col;
      for (row = 0; row < 8; row++)
      {
        for (col=0; col < 8; col++)
          printf("%i\t", result_map[row][col]);

        printf("\n");
      }
      printf("\n\n");    
    }
  } 
}
