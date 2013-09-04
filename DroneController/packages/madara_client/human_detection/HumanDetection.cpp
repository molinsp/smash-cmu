/*******************************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
******************************************************************************/

/*******************************************************************************
* HumanDetection.cpp - Implementation of base class functions
******************************************************************************/

#include "HumanDetection.h"
#include "utilities/CommonMadaraVariables.h"
#include "sensors/platform_sensors.h"
#include <stdio.h>

using namespace SMASH::HumanDetection;

HumanDetection::HumanDetection()
{}

HumanDetection::~HumanDetection()
{}

bool HumanDetection::check_if_human (double temp, double curr_height)
{
  double exp_temp_min = 0;
  double exp_temp_max = 0;

  // Set expected human temperature range based on current height.
  if (curr_height <= 0.5)
  {
    exp_temp_min = 89;
    exp_temp_max = 92;
  }
  else if (curr_height > 0.5 && curr_height <= 1)
  {
    exp_temp_min = 84;
    exp_temp_max = 90;
  }
  else if (curr_height > 1 && curr_height <= 2)
  {
    exp_temp_min = 78;
    exp_temp_max = 84;
  }  
  else
  {
    exp_temp_min = 68;
    exp_temp_max = 78;
  }  

  // Check if temp falls within the expected human temperature range.
  if (temp >= exp_temp_min && temp <= exp_temp_max)
  {
    printf("HUMAN DETECTED! Height:%6.2f | Temp:%6.2f | Exp Range: [%6.2f, %6.2f]\n", curr_height,
                                                                                      temp,
                                                                                      exp_temp_min,
                                                                                      exp_temp_max); 
    return true;
  }
  
  //printf("Non-Human detected. Height:%6.2f | Temp:%6.2f | Exp Range: [%6.2f, %6.2f]\n", curr_height, 
  //                                                                                      temp,
  //                                                                                      exp_temp_min,
  //                                                                                      exp_temp_max);
  return false;
}
