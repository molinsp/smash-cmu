/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

/*********************************************************************
* ThermalSensor.h - A simulated thermal sensor.
*********************************************************************/

#pragma once

#ifndef _THERMAL_SENSOR_PLUGIN_H
#define _THERMAL_SENSOR_PLUGIN_H

#include <string>

// Simulates a thermal sensor associated with a certain drone.
namespace SMASHSim
{
  class ThermalSensor
  {
  public:
    // The size of the thermal buffer.
    static const int THERMAL_BUFFER_HEIGHT = 3;
    static const int THERMAL_BUFFER_WIDTH = 3;

    // Gets a string with the thermal buffer information.
    std::string ThermalSensor::getThermalBuffer(int droneId);
  private:
    // How many meters the sensor can see around its center.
    static const int SENSOR_VIEW_RADIUS = 0.5;

    // Ambient border temperatures for generation of the thermal buffer.
    static const int AMBIENT_MAX_TEMP = 60;
    static const int AMBIENT_MIN_TEMP = 10;

    // Checks if there is a person currently below the sensor.
    bool findPeopleBelow(std::string droneName);

    // Gets a simulated value that will be recognized as a person for the given
    // height.
    int getValidHumanValue(int height);
  };
}

#endif
