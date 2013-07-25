--####################################################################
--# Usage of this software requires acceptance of the SMASH-CMU License,
--# which can be found at the following URL:
--#
--# https://code.google.com/p/smash-cmu/wiki/License
--######################################################################

require('Params')
require('Utils')
require('QuadricopterLocations')
    
-- The speed defines how far the target moves, and therefore how fast the drone will follow.
--TARGET_SPEED = 0.0000003    -- This is rougly equivalent to 3 cm.
TARGET_SPEED = 0.0000008

-- This margin (in degrees) indicates how close to a person we use to declare that we found it.
PERSON_FOUND_ERROR_MARGIN = 0.000008    -- This is roughly equivalent to 80 cm.

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Method called when the simulation starts.
--/////////////////////////////////////////////////////////////////////////////////////////////
function doInitialSetup()   
    simResetDynamicObject(sim_handle_all)

    -- determine index and create more if necessary
    local index = simGetNameSuffix(nil) + 1
    local handle = simGetObjectHandle('Quadricopter')
	-- Get my name
	g_mySuffix = simGetNameSuffix(nil)
    g_myDroneName = getDroneInfoFromSuffix(g_mySuffix)
	g_myDroneId = g_mySuffix + 1     -- Drone ids start from 0, while suffixes start from -1.

    -- spawn copies if necessary
    if(index == 0) then
        --simPauseSimulation()
        previousSelection = simGetObjectSelection()
        simRemoveObjectFromSelection(sim_handle_all, -1)
        simAddObjectToSelection(sim_handle_tree, handle)
        for i = 2, g_numDrones do
            simCopyPasteSelectedObjects()
        end
    end

    -- Control continuous movement.    
    g_myTargetPositionSetup = false
    g_myTargetLon = 0
    g_myTargetLat = 0
    g_myAssignedAlt = g_minAltitude
    
    -- update index and set random location
    index = simGetNameSuffix(nil) + 2;
    if(index ~= 1) then
        index = index - (g_rows * g_columns - 1) -- hack requires floors to be init first
        index = index - (g_numPeople - 1) -- requires Bills be init first
        g_myDroneId = g_myDroneId - (g_rows * g_columns - 1)
        g_myDroneId = g_myDroneId - (g_numPeople - 1)
    end
    if(index <= g_numDrones) then
        local position = g_droneLocs[index]
        position['longitude'] = position[1]
        position['latitude'] = position[2]
        position = getXYpos(position, g_origin)
        position[1] = position['x']
        position[2] = position['y']
        position[3] = g_minAltitude
        simSetObjectPosition(handle, -1, position)
        g_myTargetLon = position[1]
        g_myTargetLat = position[2]
        g_myAssignedAlt = g_minAltitude
    end

    -- Setup the plugin to communicate to the network.
	simExtMadaraQuadrotorControlSetup(g_myDroneId)   

    -- zero the angles
    local zero = {}
    zero[1] = 0
    zero[2] = 0
    zero[3] = 0
    local targetHandle = simGetObjectHandle('Quadricopter_target')
    simSetObjectOrientation(targetHandle, -1, zero)
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Method called in each step of the simulation.
--/////////////////////////////////////////////////////////////////////////////////////////////
function runMainLogic()   
    -- Sensors simulation.
    simulateSensors()

    -- Movement simulation.
    simulateMovementCommands()
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Gets information about position and people found to send back to the drone.
--/////////////////////////////////////////////////////////////////////////////////////////////
function simulateSensors()
    -- "GPS": Update our current position to the real drone.
    updateDronePosition()
    
    -- "Thermal": Check if we have found a person to stop on top of it (only if we are patrolling)
    --lookForPersonBelow()
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Updates the position of the drone to the network.
--/////////////////////////////////////////////////////////////////////////////////////////////
function updateDronePosition()
	local droneName, dronePosition = getDroneInfoFromId(g_myDroneId)
	if(dronePosition ~= nil) then
        --simAddStatusbarMessage('Sending pos ' ..tostring(dronePosition[1])..','.. tostring(dronePosition[2]))
		simExtMadaraQuadrotorControlUpdateStatus(g_myDroneId, tostring(dronePosition[1]), tostring(dronePosition[2]), tostring(dronePosition[3]))
	end
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Checks if there is movement to be done, and do it.
--/////////////////////////////////////////////////////////////////////////////////////////////
function simulateMovementCommands()
    local myNewLon = nil
    local myNewLat = nil
    local myNewAlt = nil
    local command = ''
    
    -- We check if there is a new command.
    command, result1, result2, result3 = simExtMadaraQuadrotorControlGetNewCmd(g_myDroneId)    
    if(not (command == nil)) then
        --simAddStatusbarMessage('Command: '..command)    
        local isGoToAltCmd = simExtMadaraQuadrotorControlIsGoToAltCmd(command) 
        if(isGoToAltCmd) then
            myNewAlt = result1
        
            if(g_myTargetPositionSetup == false) then
                
                -- Get the current position of the target.
                local droneTargetHandle = simGetObjectHandle('Quadricopter_target')
                local droneTargetPosition = getObjectPositionInDegrees(droneTargetHandle, -1) 

                -- If no position has been set up, set the current lat and long for the target.                
                g_myTargetLon = droneTargetPosition[1]
                g_myTargetLat = droneTargetPosition[2]
                simAddStatusbarMessage("Target lat and long: " .. g_myTargetLon .. "," .. g_myTargetLat)
            end
            
            -- We only set the altitude, keeping the previously set long and lat.
            g_myTargetPositionSetup = true
            g_myAssignedAlt = tonumber(myNewAlt)            
        end          
        
        local isGoToCmd = simExtMadaraQuadrotorControlIsGoToCmd(command) 
        if(isGoToCmd) then
            myNewLon = result1
            myNewLat = result2
            
            -- If we have to move to a new location, move our target there so the drone will follow it. Altitude is ignored.
            g_myTargetPositionSetup = true
            simAddStatusbarMessage('(In ' .. g_myDroneName .. ', id=' .. g_myDroneId .. ') In Lua, target position found: ' .. myNewLon .. ',' .. myNewLat)            
            g_myTargetLon = tonumber(myNewLon)
            g_myTargetLat = tonumber(myNewLat)
            
            local targetPoint = {}
            targetPoint['longitude'] = g_myTargetLon            
            targetPoint['latitude'] = g_myTargetLat
            cartesianPoint = getXYpos(targetPoint)
            simAddStatusbarMessage('(In ' .. g_myDroneName .. ', id=' .. g_myDroneId .. ') In Lua, cartesian target position: ' ..cartesianPoint['x'] .. ',' .. cartesianPoint['y'])            
        end
    end
	
    -- Move if required.	
    if(g_myTargetPositionSetup) then
        moveTargetTowardsPosition(g_myTargetLon, g_myTargetLat, g_myAssignedAlt)
    end    
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Moves the target to a new position, so the drone will follow it there.
--/////////////////////////////////////////////////////////////////////////////////////////////
function moveTargetTowardsPosition(newPositionLon, newPositionLat, newAltitude)
    -- Get the current position of the target.
    local droneTargetHandle = simGetObjectHandle('Quadricopter_target')
    local droneTargetPosition = getObjectPositionInDegrees(droneTargetHandle, -1)
    
    local speed = TARGET_SPEED
    --simAddStatusbarMessage('(In ' .. g_myDroneName .. ', id=' .. g_myDroneId .. ') Curr target position' .. droneTargetPosition[1] .. ',' .. droneTargetPosition[2]..':'..speed)
    
    local deltaLon = newPositionLon - droneTargetPosition[1]
    local deltaLat = newPositionLat - droneTargetPosition[2]
    local deltaTotal = math.abs(deltaLon) + math.abs(deltaLat)

    if(deltaTotal < speed) then
      droneTargetPosition[1] = newPositionLon
      droneTargetPosition[2] = newPositionLat
    else
      droneTargetPosition[1] = droneTargetPosition[1] + deltaLon / deltaTotal * speed
      droneTargetPosition[2] = droneTargetPosition[2] + deltaLat / deltaTotal * speed
    end
    
    -- The altitude that we were assinged will be set directly, independently of our current one.
    droneTargetPosition[3] = newAltitude    

    -- Move the target to a new position, so the drone will follow it there.
    --simAddStatusbarMessage('(In ' .. g_myDroneName .. ', id=' .. g_myDroneId .. ') Final target position' .. (newPositionLon) .. ',' .. (newPositionLat)..', speed: '..speed)
    --simAddStatusbarMessage('(In ' .. g_myDroneName .. ', id=' .. g_myDroneId .. ') Moving target to position' .. ((droneTargetPosition[1]+79.9402)*10000) .. ',' .. ((droneTargetPosition[2]-40.4432)*1000))
    setObjectPositionFromDegrees(droneTargetHandle, -1, droneTargetPosition)
end

function cleanUp()
end
