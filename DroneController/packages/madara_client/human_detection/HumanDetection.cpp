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
#include "sensors/platform_sensors.h"
#include <stdio.h>

using namespace SMASH::HumanDetection;

bool HumanDetection::check_if_human (double temp)
{
  // Get current height.
  double curr_height = read_ultrasound();
  
  // Based on current height, compare given temperature to expected 
  // human temperature range.
  if (curr_height <= 0.5 && temp >= 82 && temp <= 92)
    return true;

  if (curr_height > 0.5 && curr_height <= 1 && temp >= 78 && temp <= 88)
    return true;

  if (curr_height > 1 && curr_height <= 2 && temp >= 70 && temp <= 78)
    return true;

  if (curr_height > 2 && temp >= 68 && temp <= 75)
    return true;  

  return false;
}
