--####################################################################
--# Usage of this software requires acceptance of the SMASH-CMU License,
--# which can be found at the following URL:
--#
--# https://code.google.com/p/smash-cmu/wiki/License
--######################################################################

require("Utils")
    
-- Indicates how much to move (in meters) the target for the drone to follow. The bigger this value,
-- the further the target will move, and the faster the drone will follow. Therefore,
-- higher values result in higher movement speeds (up to the max speed of the drone model).
-- This value has to be less than the GPS accuracy of the VRep platform in the simulated Drones.
TARGET_STEP = 0.05    -- 5 cm.

-- Altitude to reach when taking off.
TAKEOFF_ALTITUDE = 1.5

-- Altitude to reach when landing.
LAND_ALTITUDE = 1.0

-- This margin (in degrees) indicates how close to a person we use to declare that we found it.
PERSON_FOUND_ERROR_MARGIN = 0.000005    -- This is roughly equivalent to 50 cm.

-- This is the size of the thermal buffer we are simulating.
THERMAL_BUFFER_HEIGHT = 8
THERMAL_BUFFER_WIDTH = 8

-- Used when generating random ambient temperatures.
AMBIENT_MIN_TEMP = 10
AMBIENT_MAX_TEMP = 60

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Method called when the simulation starts.
--/////////////////////////////////////////////////////////////////////////////////////////////
function doInitialSetup()   
	-- Get my name
	g_mySuffix = simGetNameSuffix(nil)
	g_myDroneId = g_mySuffix + 1     -- Drone ids start from 0, while suffixes start from -1.
    g_myDroneName = getDroneInfoFromId(g_myDroneId)	
    
    -- Min default altitude.
    g_minAltitude = simGetScriptSimulationParameter(sim_handle_main_script, 'minimumAltitude')
    
    -- Control continuous movement.    
    g_myTargetPositionSetup = false
    g_myTargetLon = 0
    g_myTargetLat = 0
    g_myAssignedAlt = g_minAltitude    
	g_flying = false
	
	g_numPeople = 2
	
	-- Setup a random seed for thermals.
	math.randomseed( os.time() )
	math.random()
    
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
-- Returns the name and position (as a 'latitude', 'longitude', 'altitude' table) of a drone with a given id (starting at 0).
--/////////////////////////////////////////////////////////////////////////////////////////////
function getDroneInfoFromId(id)
    local droneObjectName = 'Quadricopter#'

    -- For all drones but the first one (id 0), we have to add the suffix, which starts at 0 (id-1).
    if(id ~= 0) then
        droneObjectName = droneObjectName .. (id-1)
    end
    
    -- Get the position from the drone object.
    local droneHandle = simGetObjectHandle(droneObjectName)
    local dronePosition = getObjectPositionInDegrees(droneHandle, -1)    
    
    return droneObjectName, dronePosition    
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Updates the position of the drone to the network.
--/////////////////////////////////////////////////////////////////////////////////////////////
function updateDronePosition()
	local droneName, dronePosition = getDroneInfoFromId(g_myDroneId)
	if(dronePosition ~= nil) then
        --simAddStatusbarMessage('Sending pos ' ..tostring(dronePosition[1])..','.. tostring(dronePosition[2]))
		simExtMadaraQuadrotorControlUpdateStatus(g_myDroneId, tostring(dronePosition['latitude']), tostring(dronePosition['longitude']), tostring(dronePosition['altitude']))
	end
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Check if we have found a person to stop on top of it.
--/////////////////////////////////////////////////////////////////////////////////////////////
function lookForPersonBelow()
    -- Get my drone position.
    local droneName, dronePos = getDroneInfoFromId(g_myDroneId)

    -- Check if we found a person, to stop.
	local humanFound = false
    for i=1, g_numPeople, 1 do
		-- Calculate the name of this person.
		personName = 'Bill#'
		if(i~=1) then
			personName = personName .. (i-2)
		end
	
		humanFound = isPersonBelow(dronePos, personName)
        if(humanFound) then
            -- Notify our shared memory that a person was found, and that I was the one to find it.
            local sourceSuffix, sourceName = simGetNameSuffix(nil)
            simSetScriptSimulationParameter(sim_handle_main_script, 'personFoundName', personName)
            simSetScriptSimulationParameter(sim_handle_main_script, 'droneThatFound', sourceSuffix)
            --simAddStatusbarMessage('Drone with id ' .. g_myDroneId .. ' is seeing person ' .. i .. '!')
			break
        end
    end
	
	-- Simulate the thermal buffer, filling it with random low values.
	local bufferString = ''
	for row=1, THERMAL_BUFFER_HEIGHT, 1 do
		for col=1, THERMAL_BUFFER_WIDTH, 1 do
			-- Add a comma to every but the first value.
			if(not (row == 1 and col == 1)) then
				bufferString = bufferString .. ','
			end
			
			-- Calcualte a random ambient value for this cell.
			local thermalCellValue = tostring(math.random(AMBIENT_MIN_TEMP, AMBIENT_MAX_TEMP))
			
			-- If there are humans, we will add them only to one specific location in the buffer.
			if(humanFound and row == 1 and col == 1) then
				thermalCellValue = getHumanValue(dronePos['altitude'])			
				--simAddStatusbarMessage('Drone with id ' .. g_myDroneId .. ' at height ' .. dronePos[3] .. ' found a thermal and will set it to' .. thermalCellValue)
			end
			
			-- Add this value to the buffer string.
			bufferString = bufferString .. thermalCellValue
		end
	end		

	-- Set the thermal buffer.
	simExtMadaraQuadrotorControlUpdateThermals(g_myDroneId, THERMAL_BUFFER_HEIGHT, THERMAL_BUFFER_WIDTH, bufferString)
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Check if we have found a person to stop on top of it.
--/////////////////////////////////////////////////////////////////////////////////////////////
function isPersonBelow(dronePos, personName)
	personCoord = getPersonPositionInDegrees(personName)
    local margin = PERSON_FOUND_ERROR_MARGIN
    if( (dronePos['latitude'] >= personCoord['latitude'] - margin) and (dronePos['latitude'] <= personCoord['latitude'] + margin) ) then
        if((dronePos['longitude'] >= personCoord['longitude'] - margin) and (dronePos['longitude']<= personCoord['longitude'] + margin)) then
            return true
        end
    end
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Returns a valid human value depending on the height.
--/////////////////////////////////////////////////////////////////////////////////////////////
function getHumanValue(height)
	if(height <= 0.5) then
		return 85
	elseif(height <= 1.0) then
		return 80
	elseif(height <= 2.0) then
		return 75
	else
		return 70
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
            simAddStatusbarMessage('(In ' .. g_myDroneName .. ', id=' .. g_myDroneId .. ') In Lua, target position found: ' .. myNewLat .. ',' .. myNewLon)            
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
	g_myTargetLon = droneTargetPosition['longitude']
	g_myTargetLat = droneTargetPosition['latitude']
	simAddStatusbarMessage("Target lat and long: " .. g_myTargetLat .. "," .. g_myTargetLon)
	
	g_myTargetPositionSetup = true
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Moves the target to a new position, so the drone will follow it there.
--/////////////////////////////////////////////////////////////////////////////////////////////
function moveTargetTowardsPosition(newPositionLon, newPositionLat, newAltitude)
    -- Turn the target position into meters for simplicity.
    local newPosition = {}
    newPosition['longitude'] = newPositionLon
    newPosition['latitude'] = newPositionLat
    local newPositionCartesian = getXYpos(newPosition)
    
    -- Get the current position of the target. Work in meters for simplicity.
    local droneTargetHandle = simGetObjectHandle('Quadricopter_target')
    local droneTargetPosition = simGetObjectPosition(droneTargetHandle, -1)
    
    local targetStep = TARGET_STEP
    --simAddStatusbarMessage('(In ' .. g_myDroneName .. ', id=' .. g_myDroneId .. ') Curr target position' .. droneTargetPosition[1] .. ',' .. droneTargetPosition[2])
    
    -- Calculate the distance between the current and new position as the diagonal distance between these two points.
    -- Note that they are both in meters, and so is the distance.
    local distanceInX = newPositionCartesian['x'] - droneTargetPosition[1]
    local distanceInY = newPositionCartesian['y'] - droneTargetPosition[2]
    local distanceToNewPosition = math.sqrt(math.pow(distanceInX, 2) + math.pow(distanceInY, 2))

    -- Check if the distance to the new position is less than the steps that the target takes.
    if(distanceToNewPosition < targetStep) then
        -- If so, we just move the target to the new position, since it will be less or equal than a regular step anyway.
        droneTargetPosition[1] = newPositionCartesian['x']
        droneTargetPosition[2] = newPositionCartesian['y']
    else
        -- Calculate how much to move the target in X and Y to achieve an actual movement distance of the target of targetStep
        targetStepX = targetStep * (distanceInX / distanceToNewPosition)
        targetStepY = targetStep * (distanceInY / distanceToNewPosition)
        
        -- Update the new position of the target.
        droneTargetPosition[1] = droneTargetPosition[1] + targetStepX 
        droneTargetPosition[2] = droneTargetPosition[2] + targetStepY
    end
    
    -- The altitude that we were assinged will be set directly, independently of our current one.
    droneTargetPosition[3] = newAltitude    

    -- Move the target to a new position, so the drone will follow it there.
    --simAddStatusbarMessage('(In ' .. g_myDroneName .. ', id=' .. g_myDroneId .. ') Final target position' .. droneTargetPosition[1] .. ',' .. droneTargetPosition[2])
    simSetObjectPosition(droneTargetHandle, -1, droneTargetPosition)
end
