/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

/*********************************************************************
* MovementActuator.cpp - A simulated movement actuator.
*********************************************************************/

#include "MovementActuator.h"
#include "SimUtils.h"
#include "PluginUtils.h"
#include "utilities/gps_utils.h"
#include "utilities/string_utils.h"
#include "v_repLib.h"
#include <utility>
#include <math.h>

using namespace SMASHSim;

// We define the values for the coordinates here since we can't do it in the 
// header file.
const double MovementActuator::TARGET_STEP = 0.05 ;
const double MovementActuator::TAKEOFF_ALTITUDE = 1.5;
const double MovementActuator::LAND_ALTITUDE = 1.0;

///////////////////////////////////////////////////////////////////////////////
// Default constructor.
///////////////////////////////////////////////////////////////////////////////
MovementActuator::MovementActuator(int droneId) 
  : m_droneId(droneId), m_flying(false)
{
  // The default next location is the current location of the target.
  m_droneTargetName = SMASHSim::SimUtils::getDroneTargetName(m_droneId);
  m_nextDroneLocation = 
    SMASHSim::SimUtils::getObjectPositionInDegrees(m_droneTargetName);
}

///////////////////////////////////////////////////////////////////////////////
// Goes to a particular altitude without changing the current lat and long.
///////////////////////////////////////////////////////////////////////////////
void MovementActuator::goToAltitude(double altitude)
{
  m_nextDroneLocation.altitude = altitude;
  moveTargetObjectTowardsNextDroneLocation();
}

///////////////////////////////////////////////////////////////////////////////
// Goes to a particular position (lat, long), not changing the current altitude.
///////////////////////////////////////////////////////////////////////////////
void MovementActuator::goToPosition(SMASH::Utilities::Position targetPosition)
{
  m_nextDroneLocation.latAndLong = targetPosition;
  moveTargetObjectTowardsNextDroneLocation();
}

///////////////////////////////////////////////////////////////////////////////
// Makes the drone take off.
///////////////////////////////////////////////////////////////////////////////
void MovementActuator::takeOff()
{
  m_nextDroneLocation.altitude = TAKEOFF_ALTITUDE;
  moveTargetObjectTowardsNextDroneLocation();
}

///////////////////////////////////////////////////////////////////////////////
// Lands the drone.
///////////////////////////////////////////////////////////////////////////////
void MovementActuator::land()
{
  m_nextDroneLocation.altitude = LAND_ALTITUDE;
  moveTargetObjectTowardsNextDroneLocation();
}

///////////////////////////////////////////////////////////////////////////////
// Goes to a particular location (lat, long, and altitude).
///////////////////////////////////////////////////////////////////////////////
void MovementActuator::goToLocation(SMASHSim::Location targetLocation)
{
  m_nextDroneLocation = targetLocation;
}

///////////////////////////////////////////////////////////////////////////////
// Moves to the next location that has been set up.
///////////////////////////////////////////////////////////////////////////////
void MovementActuator::moveTargetObjectTowardsNextDroneLocation()
{
  // Turn the next location into cartesian coords for simpler handling.
  SMASH::Utilities::Position referencePoint = 
    SMASHSim::SimUtils::getReferencePoint();
  SMASH::Utilities::CartesianPosition nextDronePositionCoords = 
    SMASH::Utilities::getCartesianCoordinates(m_nextDroneLocation.latAndLong, 
    referencePoint);
  nextDronePositionCoords.z = m_nextDroneLocation.altitude;

  // Get the current position of the target object.
  float currPos[3];
  VREP::PluginUtils::getObjectPosition(m_droneTargetName, currPos);
  SMASH::Utilities::CartesianPosition currentTargetPosition(currPos[0], currPos[1]);

  // Calculate the distance between the current and new position of the target
  // object as the diagonal distance between these two points.
  // Note that they are both in meters, and so is the distance.
  double distanceInX = nextDronePositionCoords.x - currentTargetPosition.x;
  double distanceInY = nextDronePositionCoords.y - currentTargetPosition.y;
  double distanceToNewPosition = sqrt(pow(distanceInX, 2) + 
    pow(distanceInY, 2));

  // Check if the distance to the new position is less than the steps that the 
  // target uses when moving.
  SMASH::Utilities::CartesianPosition nextTargetObjectPosition;
  if(distanceToNewPosition < TARGET_STEP)
  {
    // If so, we just move the target to the new position, since it will be 
    // less or equal than a regular step anyway.
    nextTargetObjectPosition.x = nextDronePositionCoords.x;
    nextTargetObjectPosition.y = nextDronePositionCoords.y;
  }
  else
  {
    // Calculate how much to move the target in X and Y to achieve an actual 
    // movement distance of TARGET_STEP meters for the target.
    double targetStepX = TARGET_STEP * (distanceInX / distanceToNewPosition);
    double targetStepY = TARGET_STEP * (distanceInY / distanceToNewPosition);

    // Set the new position of the target.
    nextTargetObjectPosition.x = currentTargetPosition.x + targetStepX;
    nextTargetObjectPosition.y = currentTargetPosition.y + targetStepY;
  }

  // The altitude will be the next altitude right away (this will always make
  // us move fast in the z plane).
  nextTargetObjectPosition.z = nextDronePositionCoords.z;

  // Physically move the target object to its new location.
  simAddStatusbarMessage(nextTargetObjectPosition.toString().c_str());
  VREP::PluginUtils::setObjectPosition(m_droneTargetName, 
    nextTargetObjectPosition.x, nextTargetObjectPosition.y, 
    nextTargetObjectPosition.z); 
}