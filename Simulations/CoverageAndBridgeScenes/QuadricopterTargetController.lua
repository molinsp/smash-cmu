--####################################################################
--# Usage of this software requires acceptance of the SMASH-CMU License,
--# which can be found at the following URL:
--#
--# https://code.google.com/p/smash-cmu/wiki/License
--######################################################################

require("Utils")
    
-- The speed defines how far the target moves, and therefore how fast the drone will follow.
TARGET_SPEED = 0.0000003    -- This is rougly equivalent to 3 cm.

-- Altitude to reach when taking off.
TAKEOFF_ALTITUDE = 1.5

-- Altitude to reach when landing.
LAND_ALTITUDE = 1.0

-- This margin (in degrees) indicates how close to a person we use to declare that we found it.
PERSON_FOUND_ERROR_MARGIN = 0.000005    -- This is roughly equivalent to 50 cm.

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Method called when the simulation starts.
--/////////////////////////////////////////////////////////////////////////////////////////////
function doInitialSetup()   
	-- Get my name
	g_mySuffix = simGetNameSuffix(nil)
    g_myDroneName = getDroneInfoFromSuffix(g_mySuffix)
	g_myDroneId = g_mySuffix + 1     -- Drone ids start from 0, while suffixes start from -1.

    -- Load the positions of people on the grid, so we will know when we find one.
    loadPeoplePositions()
    
    -- Min default altitude.
    g_minAltitude = simGetScriptSimulationParameter(sim_handle_main_script, 'minimumAltitude')
    
    -- Control continuous movement.    
    g_myTargetPositionSetup = false
    g_myTargetLon = 0
    g_myTargetLat = 0
    g_myAssignedAlt = g_minAltitude    
	g_flying = false
    
    -- Setup the plugin to communicate to the network.
	simExtMadaraQuadrotorControlSetup(g_myDroneId)   
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Method called when the simulation ends.
--/////////////////////////////////////////////////////////////////////////////////////////////
function doCleanup()
    -- Stop the network plugin.
    simExtMadaraQuadrotorControlCleanup()
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
    
    -- "Thermal": Check if we have found a person to stop on top of it (only if we are patrolling).
    lookForPersonBelow()
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Updates the position of the drone to the network.
--/////////////////////////////////////////////////////////////////////////////////////////////
function updateDronePosition()
	local droneName, dronePosition = getDroneInfoFromId(g_myDroneId)
	if(dronePosition ~= nil) then
        --simAddStatusbarMessage('Sending pos ' ..tostring(dronePosition[1])..','.. tostring(dronePosition[2]))
		simExtMadaraQuadrotorControlUpdateStatus(g_myDroneId, tostring(dronePosition[2]), tostring(dronePosition[1]), tostring(dronePosition[3]))
	end
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Check if we have found a person to stop on top of it.
--/////////////////////////////////////////////////////////////////////////////////////////////
function lookForPersonBelow()
    -- Get my drone position.
    local droneName, dronePos = getDroneInfoFromSuffix(g_mySuffix)

    -- Check if we found a person, to stop.
    for i=1, g_numPeople, 1 do
        if( isPersonBelow(dronePos, g_personCoordsX[i], g_personCoordsY[i])) then
            -- Notify our shared memory that a person was found, and that I was the one to find it.
            local sourceSuffix, sourceName = simGetNameSuffix(nil)
            simSetScriptSimulationParameter(sim_handle_main_script, 'personFoundId', i)
            simSetScriptSimulationParameter(sim_handle_main_script, 'droneThatFound', sourceSuffix)
            --simAddStatusbarMessage('Drone with suffix ' .. sourceSuffix .. ' is seeing person ' .. i .. '!')
        end
    end
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Check if we have found a person to stop on top of it.
--/////////////////////////////////////////////////////////////////////////////////////////////
function isPersonBelow(dronePos, personCoordX, personCoordY)
    local margin = PERSON_FOUND_ERROR_MARGIN
    if( (dronePos[1] >= personCoordX - margin) and (dronePos[1] <= personCoordX + margin) ) then
        if((dronePos[2] >= personCoordY - margin) and (dronePos[2] <= personCoordY + margin)) then
            return true
        end
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
        simAddStatusbarMessage('Command: '..command)    
		
		-- Handle Go To Altitude commands.
        local isGoToAltCmd = simExtMadaraQuadrotorControlIsGoToAltCmd(command) 
        if(isGoToAltCmd) then
            myNewAlt = result1
        
			-- If there is no target position yet, set it to our current position (since the move function needs a target).
            if(g_myTargetPositionSetup == false) then
                setTargetPositionToCurrentPosition()
            end
            
            -- We only set the altitude, keeping the previously set long and lat.
            g_myAssignedAlt = tonumber(myNewAlt)            
        end          
        
		-- Handle Go To GPS commands.
        local isGoToCmd = simExtMadaraQuadrotorControlIsGoToCmd(command) 
        if(isGoToCmd) then
            myNewLon = result2
            myNewLat = result1
            
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
		
		-- Handle Take Off commands.
        local isTakeOffCmd = simExtMadaraQuadrotorControlIsTakeoffCmd(command) 
        if(isTakeOffCmd) then
			-- If we are flying, we ignore the takeoff command.
			if(g_flying) then
				simAddStatusbarMessage('(In ' .. g_myDroneName .. ', id=' .. g_myDroneId .. ') Ignoring take off command since we are already flying.')  
			end
		
			simAddStatusbarMessage('(In ' .. g_myDroneName .. ', id=' .. g_myDroneId .. ') Taking off!')  
			-- If there is no target position yet, set it to our current position (since the move function needs a target.
            if(g_myTargetPositionSetup == false) then
                setTargetPositionToCurrentPosition()
            end
            
			-- Indicate that we are now flying, and set the altitude to the takeoff default.
			g_myAssignedAlt = TAKEOFF_ALTITUDE
			g_flying = true
		end	

		-- Handle Land commands.
        local isLandCmd = simExtMadaraQuadrotorControlIsLandCmd(command) 
        if(isLandCmd) then
			-- If we are flying, we ignore the takeoff command.
			if(not g_flying) then
				simAddStatusbarMessage('(In ' .. g_myDroneName .. ', id=' .. g_myDroneId .. ') Ignoring land command since we are already landed.')  
			end
		
			simAddStatusbarMessage('(In ' .. g_myDroneName .. ', id=' .. g_myDroneId .. ') Landing!')  
            
			-- Indicate that we are now flying, and set the altitude to the land default.
			g_myAssignedAlt = LAND_ALTITUDE
			g_flying = false
		end			
    end
	
    -- Move if required.	
    if(g_myTargetPositionSetup) then
        moveTargetTowardsPosition(g_myTargetLon, g_myTargetLat, g_myAssignedAlt)
    end    
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Sets the target position (lat and long) to the current position of the drone. Needed if there
-- is no target position yet, but the drone has to move up.
--/////////////////////////////////////////////////////////////////////////////////////////////
function setTargetPositionToCurrentPosition()
	-- Get the current position of the target.
	local droneTargetHandle = simGetObjectHandle('Quadricopter_target')
	local droneTargetPosition = getObjectPositionInDegrees(droneTargetHandle, -1) 

	-- Set the current lat and long for the target.                
	g_myTargetLon = droneTargetPosition[1]
	g_myTargetLat = droneTargetPosition[2]
	simAddStatusbarMessage("Target lat and long: " .. g_myTargetLon .. "," .. g_myTargetLat)
	
	g_myTargetPositionSetup = true
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Moves the target to a new position, so the drone will follow it there.
--/////////////////////////////////////////////////////////////////////////////////////////////
function moveTargetTowardsPosition(newPositionLon, newPositionLat, newAltitude)
    -- Get the current position of the target.
    local droneTargetHandle = simGetObjectHandle('Quadricopter_target')
    local droneTargetPosition = getObjectPositionInDegrees(droneTargetHandle, -1)
    
    local speed = TARGET_SPEED
    --simAddStatusbarMessage('(In ' .. g_myDroneName .. ', id=' .. g_myDroneId .. ') Curr target position' .. droneTargetPosition[1] .. ',' .. droneTargetPosition[2]..':'..newAltitude)
    
    local deltaLon = newPositionLon - droneTargetPosition[1]
    local deltaLat = newPositionLat - droneTargetPosition[2]
    local deltaTotal = math.sqrt(math.pow(deltaLon, 2) + math.pow(deltaLat, 2))

    if(deltaTotal < speed) then
        droneTargetPosition[1] = newPositionLon
        droneTargetPosition[2] = newPositionLat
    else
        deltaLon = speed / deltaTotal * deltaLon
        deltaLat = speed / deltaTotal * deltaLat
        droneTargetPosition[1] = droneTargetPosition[1] + deltaLon 
        droneTargetPosition[2] = droneTargetPosition[2] + deltaLat
    end
    
    -- The altitude that we were assinged will be set directly, independently of our current one.
    droneTargetPosition[3] = newAltitude    

    -- Move the target to a new position, so the drone will follow it there.
    --simAddStatusbarMessage('(In ' .. g_myDroneName .. ', id=' .. g_myDroneId .. ') Final target position' .. (newPositionLon) .. ',' .. (newPositionLat)..', speed: '..speed)
    --simAddStatusbarMessage('(In ' .. g_myDroneName .. ', id=' .. g_myDroneId .. ') Moving target to position' .. ((droneTargetPosition[1]+79.9402)*10000) .. ',' .. ((droneTargetPosition[2]-40.4432)*1000))
    setObjectPositionFromDegrees(droneTargetHandle, -1, droneTargetPosition)
end
