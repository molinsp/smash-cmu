/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

/*********************************************************************
* MovementActuator.h - A simulated movement actuator.
*********************************************************************/

#pragma once

#ifndef _MOVEMENT_ACTUATOR_PLUGIN_H
#define _MOVEMENT_ACTUATOR_PLUGIN_H

#include "Location.h"
#include "utilities\Position.h"
#include <string>

// Simulates a movement actuator or controller that reacts to remote commands.
namespace SMASHSim
{
  class MovementActuator
  {
  public:
    // Indicates how much to move (in meters) the target for the drone to 
    // follow. The bigger this value, the further the target will move, and the 
    // faster the drone will follow. Therefore, higher values result in higher 
    // movement speeds (up to the max speed of the drone model). This value has 
    // to be less than the GPS accuracy of the VRep platform in the simulated 
    // drones.
    static const int TARGET_STEP = 0.05 ;

    // Altitude to reach when taking off (and hovering at that height).
    static const int TAKEOFF_ALTITUDE = 1.5;

    // Altitude to reach when landing (they don't really land to prevent 
    // collisions with the floor).
    static const int LAND_ALTITUDE = 1.0;

    // Default constructor.
    MovementActuator(int droneId);

    // Methods to handle the different commands required by a drone.
    void goToAltitude(double height);
    void goToPosition(SMASH::Utilities::Position targetPosition);
    void goToLocation(SMASHSim::Location targetLocation);
    void takeOff();
    void land();
  private:
    // Information about the drone we belong to.
    int m_droneId;
    std::string m_droneTargetName;

    // Flag to check whether we are flying or not.
    bool m_flying;

    // Stores the next location the drone wants to eventually get to.
    SMASHSim::Location m_nextDroneLocation;

    // Checks if there is a person currently below the sensor.
    void moveTargetObjectTowardsNextDroneLocation();
  };
}

#endif
