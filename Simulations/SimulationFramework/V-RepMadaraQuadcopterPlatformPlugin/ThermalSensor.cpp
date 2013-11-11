/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

/*********************************************************************
* ThermalSensor.cpp - A simulated thermal sensor. 
*********************************************************************/

#include "ThermalSensor.h"
#include "SimUtils.h"
#include "PluginUtils.h"
#include "utilities/string_utils.h"
#include "v_repLib.h"
#include <math.h>
#include <time.h>

using namespace SMASHSim;

///////////////////////////////////////////////////////////////////////////////
// Sets up the search area for the whole network.
///////////////////////////////////////////////////////////////////////////////
std::string ThermalSensor::getThermalBuffer(int droneId)
{
  //simAddStatusbarMessage("Getting thermal buffer.");

  // Seed the random generator.
  srand (time(NULL) + droneId);

  // Get the drone name and position.
  std::string droneName = SimUtils::getDroneName(droneId);
  Location dronePos = SimUtils::getObjectPositionInDegrees(droneName);

  // Check if we found a person, to stop.
  bool humanFound = findPeopleBelow(droneName);

  // Simulate the thermal buffer, filling it with random low values.
  std::stringstream bufferString;
  for (int row=0; row<BUFFER_HEIGHT; row++) 
  {
    for (int col=0; col<BUFFER_WIDTH; col++)
    {
      // Add a comma to every but the first value.
      if(!(row == 0 && col == 0)) 
      {
        bufferString << ",";
      }

      // Calcualte a random ambient value for this cell.
      int thermalCellValue = rand() % (AMBIENT_MAX_TEMP+1) + AMBIENT_MIN_TEMP;

      // If there are humans, we will add them, only to the first cell.
      if(humanFound && row == 0 && col == 0) 
      {
        thermalCellValue = getValidHumanValue(dronePos.altitude);
      }

      // Add this value to the buffer string.
      bufferString << thermalCellValue;
    }
  }

  return bufferString.str();
}

///////////////////////////////////////////////////////////////////////////////
// Tries to find one person below the drone. One is enough; we won't make a 
// difference if two of them are really close together.
///////////////////////////////////////////////////////////////////////////////
bool ThermalSensor::findPeopleBelow(std::string droneName)
{
  // Get the position of the sensor (which is the position of the drone).
  float dronePos[3];
  VREP::PluginUtils::getObjectPosition(droneName, dronePos);

  // Check if we found a person, to stop.
  int humanFound = false;
  bool allPersonsChecked = false;
  for(int currPersonSuffix = 0; currPersonSuffix<2; currPersonSuffix++)
  {
    // Build the name of this person.
    std::string personName = "Bill#";
    if(currPersonSuffix != 0) 
    {
      personName = personName + NUM_TO_STR(currPersonSuffix-1);
    }

    // Try to get a handle for this person. If we don't get one, there are no
    // more people.
    int handle = simGetObjectHandle(personName.c_str());
    if(handle == -1)
    {
      allPersonsChecked = true;
      break;
    }

    // Get the position of the given person.
    float personPos[3];
    VREP::PluginUtils::getObjectPosition(personName, personPos);

    // Check if the person is within a square of side 2*SENSOR_VIEW_RADIUS, 
    // centered on the drone's position.
    double margin = ThermalSensor::SENSOR_VIEW_RADIUS;
    if((fabs(personPos[0] - dronePos[0]) <= margin) &&  
       (fabs(personPos[1] - dronePos[1]) <= margin)) 
    {
      // Notify our shared memory that a person was found.
      simSetScriptSimulationParameter(sim_handle_main_script, "personFoundName", 
        personName.c_str(), personName.length()+1);

      // Return indicating that there is at least one person below.
      return true;
    }
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Returns a valid human value (as interpreted by human detection algorithms)
// depending on the height.
///////////////////////////////////////////////////////////////////////////////
int ThermalSensor::getValidHumanValue(int height)
{
  if(height <= 0.5) 
    return 85;
  else if(height <= 1.0) 
    return 80;
  else if(height <= 2.0) 
    return 75;
  else
    return 70;
}
