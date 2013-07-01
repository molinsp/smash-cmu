--####################################################################
--# Usage of this software requires acceptance of the SMASH-CMU License,
--# which can be found at the following URL:
--#
--# https://code.google.com/p/smash-cmu/wiki/License
--######################################################################

require("Utils")

-- Method called when the simulation starts.
function doInitialSetup()
	-- Get my name
	g_mySuffix = simGetNameSuffix(nil)
  g_myDroneId = g_mySuffix + 1
  g_myDroneName = getDroneInfoFromSuffix(g_mySuffix)

  -- Get the total number of drones.
	--g_numDrones = simGetScriptSimulationParameter(sim_handle_main_script, 'numberOfDrones')
	g_numDrones = 1

  if(g_myDroneId == 0) then
    simExtMadaraQuadrotorControlSetup()
  end
end

-- Method called in each step of the simulation.
function runMainLogic()
  -- Check if we got new movement target, or if we have to calculate our position in the bridge.
  updateDronePosition()
  loadNewCmd()
  if(g_nextWaypointX ~= nil and g_nextWaypointY ~= nil and g_nextWaypointZ ~= nil) then
    if(isTargetAtLocation(g_nextWaypointX, g_nextWaypointY, g_nextWaypointZ) == false) then
      moveTargetToPosition(g_nextWaypointX, g_nextWaypointY, g_nextWaypointZ)
    end
  end
end

function updateDronePosition()
  local droneTargetHandle = simGetObjectHandle('Quadricopter_target')
  local droneTargetPosition = simGetObjectPosition(droneTargetHandle, -1)
  if(droneTargetPosition ~= nil) then
    simExtMadaraQuadrotorControlUpdateStatus(g_myDroneId, droneTargetPosition[1], droneTargetPosition[2], droneTargetPosition[3])
  end
end

-- Checks if there is movement to be done, or if a bridge has to be built.
function loadNewCmd()
    local cmd, x, y, z = simExtMadaraQuadrotorControlGetNewCmd(g_myDroneId)
    
    -- If we have to move to a new location, set everything to move to our position in it.
    if(cmd ~= nil) then
      if(simExtMadaraQuadrotorControlIsGoToCmd(cmd)) then
        simAddStatusbarMessage('(In ' .. g_myDroneName .. ', id=' .. g_myDroneId .. ') In QuadrotorTargetControl.lua::loadNewCommand, found: ' .. cmd .. ',' .. x .. ',' .. y .. ',' .. z)
        g_nextWaypointX = x
        g_nextWaypointY = y
        g_nextWaypointZ = z
      end
    end
end

-- Moves the target to a new position, so the drone will follow it there.
function moveTargetToPosition(newPosX, newPosY, newPosZ)
  -- Get the current position of the target
  local droneTargetHandle = simGetObjectHandle('Quadricopter_target')
  local droneTargetPosition = simGetObjectPosition(droneTargetHandle, -1)

  -- The speed defines how far the target moves, and therefore how fast the drone will follow.
	local speed = simGetFloatingParameter(sim_floatparam_simulation_time_step)

  -- get desired offsets
  local deltaX = newPosX - droneTargetPosition[1]
  local deltaY = newPosY - droneTargetPosition[2]
  local deltaZ = newPosZ - droneTargetPosition[3]
  local totalDelta = math.abs(deltaX) + math.abs(deltaY) + math.abs(deltaZ)
  if(totalDelta < speed) then
    speed = totalDelta
  end

  -- update drone target position if outside of tolerance
  local tolerance = 0.02
  if(totalDelta > tolerance) then
    droneTargetPosition[1] = droneTargetPosition[1] + (deltaX / totalDelta * speed)
    droneTargetPosition[2] = droneTargetPosition[2] + (deltaY / totalDelta * speed)
    droneTargetPosition[3] = droneTargetPosition[3] + (deltaZ / totalDelta * speed)
  else
    droneTargetPosition[1] = newPosX
    droneTargetPosition[2] = newPosY
    droneTargetPosition[3] = newPosZ
  end

  -- Move the target to a new position, so the drone will follow it there
  simSetObjectPosition(droneTargetHandle, -1, droneTargetPosition)
end

-- Checks if the drone's target has reached a certain location.
function isTargetAtLocation(locX, locY, locZ)
  -- Get the current position of the target.
  local droneTargetHandle = simGetObjectHandle('Quadricopter_target')
  local droneTargetPosition = simGetObjectPosition(droneTargetHandle, -1)    
  
  -- Accuracy of how close to the waypoint we define as actually at the waypoint.
	local accuracy = 0.02

  -- Check if the target is already at the required X position.
  local atLocationX = false
  local delta = math.abs(droneTargetPosition[1] - locX)
  if(delta <= accuracy) then
      atLocationX = true
  end

  -- Check if the target is already at the required Y position.
  local atLocationY = false
  delta = math.abs(droneTargetPosition[2] - locY)
  if(delta <= accuracy) then
      atLocationY = true
  end
  
  -- Check if the target is already at the required Z position.
  local atLocationZ = false
  delta = math.abs(droneTargetPosition[3] - locZ)
  if(delta <= accuracy) then
      atLocationZ = true
  end

  -- We are at that location if we are there for both coordinates.
  return atLocationX and atLocationY and atLocationZ
end

-- Checks if the we have reached our final destination.
function checkIfEndWasReached()
    -- Get the current position of the target.
    local droneTargetHandle = simGetObjectHandle('Quadricopter_target')
    local droneTargetPosition = simGetObjectPosition(droneTargetHandle, -1)
    
    -- Check how far the target is from the final end of its path.
    local deltax = math.abs(droneTargetPosition[1] - g_endx)
    local deltay = math.abs(droneTargetPosition[2] - g_endy)

    -- If the target is within a certain range of the final path, indicate that 
    -- we are no longer g_patrolling or g_bridging, since we will be stopped.
    local endAccuracy = 0.3
    if(deltax < endAccuracy and deltay < endAccuracy) then
        g_patrolling = false
        
        -- Mark this drone as stopped.
        local myDroneId = g_mySuffix + 1  
        --simExtMadaraClientStopDrone(myDroneId) 

        -- We just return, since we won't move anymore.
        return
    end        
end  
