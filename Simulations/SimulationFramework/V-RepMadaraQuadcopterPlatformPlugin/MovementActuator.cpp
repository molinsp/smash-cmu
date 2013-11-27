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
#include "gps_utils.h"
#include "string_utils.h"
#include "v_repLib.h"
#include <utility>
#include <math.h>

using namespace SMASHSim;
using namespace SMASH;
using namespace SMASH::Utilities;

// We define the values for the coordinates here since we can't do it in the 
// header file.
const double MovementActuator::TARGET_STEP = 0.05;
const double MovementActuator::TAKEOFF_ALTITUDE = 1.5;
const double MovementActuator::LAND_ALTITUDE = 1.0;

///////////////////////////////////////////////////////////////////////////////
// Default constructor.
///////////////////////////////////////////////////////////////////////////////
MovementActuator::MovementActuator(int droneId) 
  : m_droneId(droneId), m_flying(false)
{
  m_droneName = SMASHSim::SimUtils::getDroneName(m_droneId);
  m_droneTargetName = SMASHSim::SimUtils::getDroneTargetName(m_droneId);

  // The default next location is the current location of the target.
  m_nextDroneLocation = 
    SMASHSim::SimUtils::getObjectPositionInDegrees(m_droneTargetName);
}

///////////////////////////////////////////////////////////////////////////////
// Goes to a particular altitude without changing the current lat and long.
///////////////////////////////////////////////////////////////////////////////
void MovementActuator::goToAltitude(double altitude)
{
  m_nextDroneLocation.altitude = altitude;
}

///////////////////////////////////////////////////////////////////////////////
// Goes to a particular position (lat, long), not changing the current altitude.
///////////////////////////////////////////////////////////////////////////////
void MovementActuator::goToPosition(SMASH::Utilities::Position targetPosition)
{
  m_nextDroneLocation.latAndLong = targetPosition;
}

///////////////////////////////////////////////////////////////////////////////
// Makes the drone take off.
///////////////////////////////////////////////////////////////////////////////
void MovementActuator::takeOff()
{
  // Only take off if we were not flying.
  if(!m_flying)
  {
    m_nextDroneLocation.altitude = TAKEOFF_ALTITUDE;
    m_flying = true;
  }
  else
  {
    simAddStatusbarMessage("Ignoring takeoff command since drone is already flying.");
  }
}

///////////////////////////////////////////////////////////////////////////////
// Lands the drone.
///////////////////////////////////////////////////////////////////////////////
void MovementActuator::land()
{
  // Only land if we were flying.
  if(m_flying)
  {
    m_nextDroneLocation.altitude = LAND_ALTITUDE;
    m_flying = false;
  }
  else
  {
    simAddStatusbarMessage("Ignoring land command since drone is already landed.");
  }
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
  //simAddStatusbarMessage("***************************");
  //simAddStatusbarMessage(("Drone: " + NUM_TO_STR(m_droneId)).c_str());

  //simAddStatusbarMessage(("Next drone position: " + 
  //  m_nextDroneLocation.toString()).c_str());

  // Transform the next drone location into cartesian coordinates for VRep.
  Position referencePoint = SMASHSim::SimUtils::getReferencePoint();
  CartesianPosition nextDroneCartesianPos = Utilities::getCartesianCoordinates(
    m_nextDroneLocation.latAndLong, 
    referencePoint);
  //simAddStatusbarMessage(("Next drone cartesian position: " + 
  //  nextDroneCartesianPos.toString()).c_str());

  // Get the current position of the target object in cartesian coordinates.
  float currPos[3];
  VREP::PluginUtils::getObjectPosition(m_droneTargetName, currPos);
  CartesianPosition currentTargetCartPosition(currPos[0], currPos[1]);
  //simAddStatusbarMessage(("Curr target cartesian position: " + 
  //  nextDroneCartesianPos.toString()).c_str());

  SMASH::Utilities::Position currTargetCoords = SMASH::Utilities::getLatAndLong(currPos[0], currPos[1], referencePoint);
  //Location currTargetCoords = SimUtils::getObjectPositionInDegrees(m_droneTargetName);
  //simAddStatusbarMessage(("Curr target position: " + 
  //  currTargetCoords.toString()).c_str());

  // Calculate the distance between the current and final position of the target.
  double distanceInX = nextDroneCartesianPos.x - currentTargetCartPosition.x;
  double distanceInY = nextDroneCartesianPos.y - currentTargetCartPosition.y;
  double targetDistToNextLocation = sqrt(pow(distanceInX,2)) + sqrt(pow(distanceInY,2));
  //simAddStatusbarMessage(("Distance from target to pos: " + 
  //  NUM_TO_STR(targetDistToNextLocation)).c_str());

  // Check if we are close enough to the final position to avoid jitter with
  // the target moving slightly around the final position.
  SMASH::Utilities::CartesianPosition nextTargetObjectPosition;
  if(targetDistToNextLocation < TARGET_STEP)
  {
    // Just move the target to the drone's objective, we are close enough.
    //simAddStatusbarMessage("Moving target directly to next drone pos.");
    nextTargetObjectPosition.x = nextDroneCartesianPos.x;
    nextTargetObjectPosition.y = nextDroneCartesianPos.y;
  }
  else
  {
    // If we are not that close, we have to calculate how much to move the
    // target to maintain some sort of consistent speed.

    // Calculate how much to move the target in X and Y to achieve an actual 
    // movement distance of TARGET_STEP meters for the target.
    double targetStepX = distanceInX * (TARGET_STEP / targetDistToNextLocation);
    double targetStepY = distanceInY * (TARGET_STEP / targetDistToNextLocation);

    // Set the new position of the target.
    nextTargetObjectPosition.x = currentTargetCartPosition.x + targetStepX;
    nextTargetObjectPosition.y = currentTargetCartPosition.y + targetStepY;
  }

  // The altitude will be the next altitude right away (this will always make
  // us move fast in the z plane).
  nextTargetObjectPosition.z = m_nextDroneLocation.altitude;

  // Physically move the target object to its new location.
  //simAddStatusbarMessage(("New target position: " + 
  //  nextTargetObjectPosition.toString()).c_str());
  //simAddStatusbarMessage(("New target coord position: " + 
  //  SMASH::Utilities::getLatAndLong(nextTargetObjectPosition.x, nextTargetObjectPosition.y, referencePoint).toString()).c_str());
  VREP::PluginUtils::setObjectPosition(m_droneTargetName, 
    nextTargetObjectPosition.x, nextTargetObjectPosition.y, 
    nextTargetObjectPosition.z); 
}