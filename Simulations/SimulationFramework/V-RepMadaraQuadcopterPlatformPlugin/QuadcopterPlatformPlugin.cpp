/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

/*********************************************************************
* QuadcopterPlatformPlugin.cpp - Specifies a QuadcopterPlatform plugin for VRep.
*********************************************************************/

#include "QuadcopterPlatformPlugin.h"
#include "v_repLib.h"
#include "PluginUtils.h"
#include "SimUtils.h"

#include "ThermalSensor.h"
#include "MovementActuator.h"

#include "utilities/gps_utils.h"
#include "utilities/string_utils.h"
#include "platforms/v_rep/v-rep_sim_madara_variables.h"
#include <string>
#include <vector>

using namespace SMASHSim;

///////////////////////////////////////////////////////////////////////////////
// We have to create an actual object of this type so that the plugin DLL entry
// points will have access to it.
///////////////////////////////////////////////////////////////////////////////
VREP::ISimplePlugin* createPlugin()
{
  QuadcopterPlatformPlugin* plugin = new QuadcopterPlatformPlugin();
  return plugin;
}

///////////////////////////////////////////////////////////////////////////////
// Default constructor.
///////////////////////////////////////////////////////////////////////////////
QuadcopterPlatformPlugin::QuadcopterPlatformPlugin()
{
  m_madaraController = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// ISimpleInterface: Called when plugin is initialized.
// Note that this will be called once per object using this plugin.
///////////////////////////////////////////////////////////////////////////////
void QuadcopterPlatformPlugin::initialize(int suffix)
{
  simAddStatusbarMessage(
    "QuadcopterPlatformPlugin::initialize: Initializing Quadcopter Platform.");

  // The object"s suffixes will start at 1. The ids start at 0.
  int droneId = suffix + 1;

  // Setup Madara for communications.
  // Only set this up once (this has to be checked since this void will be 
  // called by many drones.
  if(m_madaraController == NULL)
  {
    m_madaraController = new MadaraQuadrotorControl(droneId);
  }
  else
  {
    m_madaraController->incrementNumDrones();
  }

  // Add a movement actuators.
  SMASHSim::MovementActuator mover(droneId);
  m_droneMovementActuators.push_back(mover);
}

///////////////////////////////////////////////////////////////////////////////
// ISimpleInterface: Called when plugin is closing.
///////////////////////////////////////////////////////////////////////////////
void QuadcopterPlatformPlugin::cleanup(int suffix)
{
  simAddStatusbarMessage(
    "QuadcopterPlatformPlugin::cleanup: Cleaning up Quadcopter Platform.");

  if(m_madaraController != NULL)
  {
    m_madaraController->decrementNumDrones();

    // Try to terminate the controller. If unsuccesful, it is beacuse other 
    // drones are still referencing it.
    if(m_madaraController->terminate())
    {
      simAddStatusbarMessage("QuadcopterPlatformPlugin: removing controller");
      delete m_madaraController;
      m_madaraController = NULL;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// ISimpleInterface: Called in each step of the simulation.
///////////////////////////////////////////////////////////////////////////////
void QuadcopterPlatformPlugin::executeStep(int suffix)
{
  int droneId = suffix + 1;
  simulateSensors(droneId);
  simulateMovement(droneId);
}

///////////////////////////////////////////////////////////////////////////////
// ISimpleInterface: Returns a textual ID of this plugin.
///////////////////////////////////////////////////////////////////////////////
std::string QuadcopterPlatformPlugin::getId()
{
  return "QuadcopterPlatform";
}

///////////////////////////////////////////////////////////////////////////////
// Gets data from the simulated sensors.
///////////////////////////////////////////////////////////////////////////////
void QuadcopterPlatformPlugin::simulateSensors(int droneId)
{
  // "GPS": Update our current position to the real drone.
  updateDronePosition(droneId);

  // "Thermal": Check if we have found a person to stop on top of it (only if 
  // we are patrolling).
  updateThermals(droneId);
}

///////////////////////////////////////////////////////////////////////////////
// Sets up the search area for the whole network.
///////////////////////////////////////////////////////////////////////////////
void QuadcopterPlatformPlugin::updateDronePosition(int droneId)
{
  Location droneLocation = SimUtils::getObjectPositionInDegrees(
    SimUtils::getDroneName(droneId));
  m_madaraController->updateQuadrotorPosition(droneId, droneLocation);
}

///////////////////////////////////////////////////////////////////////////////
// Sets up the search area for the whole network.
///////////////////////////////////////////////////////////////////////////////
void QuadcopterPlatformPlugin::updateThermals(int droneId)
{
  // Set the thermal buffer.
  ThermalSensor tSensor;
  std::string thermalBuffer = tSensor.getThermalBuffer(droneId);
  m_madaraController->setNewThermalScan(droneId, thermalBuffer, 
    ThermalSensor::BUFFER_HEIGHT, ThermalSensor::BUFFER_WIDTH);
}

///////////////////////////////////////////////////////////////////////////////
// Moves the drone in the simulation.
///////////////////////////////////////////////////////////////////////////////
void QuadcopterPlatformPlugin::simulateMovement(int droneId)
{
  // Get the current command, if any.
  SMASHSim::MadaraQuadrotorControl::Command* newCommand = 
    m_madaraController->getNewCommand(droneId);

  // Set the actual return values depending on whether we found a position or 
  // not.
  SMASHSim::MovementActuator mover = m_droneMovementActuators[droneId];
  if(newCommand != NULL)
  {
    VREP::PluginUtils::addStatusbarMessage("Received command: " + 
      newCommand->m_command);

    // Check the command and call the corresponding function.
    if(strcmp(newCommand->m_command.c_str(), MO_MOVE_TO_GPS_CMD) == 0)
    {
      mover.goToPosition(newCommand->m_loc.latAndLong);
    }
    else if(strcmp(newCommand->m_command.c_str(), MO_MOVE_TO_ALTITUDE_CMD) == 0)
    {
      mover.goToAltitude(newCommand->m_loc.altitude);
    }
    else if(strcmp(newCommand->m_command.c_str(), MO_TAKEOFF_CMD) == 0)
    {
      mover.takeOff();
    }
    else if(strcmp(newCommand->m_command.c_str(), MO_LAND_CMD) == 0)
    {
      mover.land();
    }
  }

  // Update the target.
  mover.moveTargetObjectTowardsNextDroneLocation();
  m_droneMovementActuators[droneId] = mover;
}
