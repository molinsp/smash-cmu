--####################################################################
--# Usage of this software requires acceptance of the SMASH-CMU License,
--# which can be found at the following URL:
--#
--# https://code.google.com/p/smash-cmu/wiki/License
--######################################################################

require("Utils")

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Method called when the simulation starts.
--/////////////////////////////////////////////////////////////////////////////////////////////
function doInitialSetup()
	-- Indicates whether we are g_patrolling our zone or not.
	g_patrolling = true

	-- Indicates whether we have moved into bridge-forming mode or not.
	g_bridging = false

    -- Indicates whether a person has been found or not.
   	g_personChecked = false
    
	-- Get my name
	g_mySuffix = simGetNameSuffix(nil)
    g_myDroneName = getDroneInfoFromSuffix(g_mySuffix)
	g_myDroneId = g_mySuffix + 1

    -- Get the total number of drones.
	g_numDrones = simGetScriptSimulationParameter(sim_handle_main_script, 'numberOfDrones')
    
    -- Flag to indicate whether it is flying or not.
    --g_flying = true

    -- Load the positions of people on the grid, so we will know when we find one.
    loadPeoplePositions()
    
    -- Indicates if we are using the Madara client for communication with external "drones".
    g_madaraClientEnabled = simGetScriptSimulationParameter(sim_handle_main_script, 'madaraClientOn')       

    -- Setup the search pattern, either on the drone itself or internally.
    if(g_madaraClientEnabled) then
		simExtMadaraQuadrotorControlSetup()
	
		-- This should be done in LaptopController.lua for all drones, since it is a SystemController action.
        local myDroneId = g_mySuffix + 1 
        g_searchAreaId = 0
        simExtMadaraClientSearchRequest(myDroneId, g_searchAreaId)

        -- Set patrolling as false, as in this case we will get this value from the external drone.
        g_patrolling = false
    else    
        setupSearchPattern()
    end
    
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Method called when the simulation ends.
--/////////////////////////////////////////////////////////////////////////////////////////////
function doCleanup()
    simExtMadaraQuadrotorControlCleanup()
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Load the people's locations, so we are able to check when we find one.
--/////////////////////////////////////////////////////////////////////////////////////////////
function loadPeoplePositions()
	g_numPeople = simGetScriptSimulationParameter(sim_handle_main_script, 'numberOfPeople')
	g_personCoords = {}
    
	local counter = 1
	for i=1, g_numPeople, 1 do
		if(i==1) then
			personHandle = simGetObjectHandle('Bill#')
		else
			personHandle = simGetObjectHandle('Bill#' .. (i-2))
		end

        local billposition = simGetObjectPosition(personHandle, -1)
		g_personCoords[counter] = billposition[1]
		g_personCoords[counter+1] = billposition[2]
		--simAddStatusbarMessage('Person ' .. counter .. ' : ' .. g_personCoords[counter] .. ', ' .. counter+1 .. ' : '..g_personCoords[counter+1])
		counter = counter + 2
	end
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Method called in each step of the simulation.
--/////////////////////////////////////////////////////////////////////////////////////////////
function runMainLogic()
    if(g_patrolling) then
        -- Check if we have found a person to stop on top of it (only if we are patrolling).
        lookForPersonBelow()

        if(not g_madaraClientEnabled) then         
            -- Update the waypoints if required, depending on how far we have moved on our search path.
            updateAreaCoverageWaypoint()        
        end
    end
    
    -- Check if we got new movement target, or if we have to calculate our position in the bridge.
    loadNewMovementCommand()    
        
    -- If we are still patrolling or bridging, check if we have to move and move.
    --simAddStatusbarMessage('(In ' .. g_myDroneName .. ') flags (p,b): ' .. tostring(g_patrolling) .. ', ' .. tostring(g_bridging) )
    if(g_patrolling or g_bridging) then
        if(not g_madaraClientEnabled) then      
            -- Check if we have reached our end location.
            checkIfEndWasReached()
        end
    
        -- Move to our next waypoint.
        moveTargetToPosition(g_nextWaypointx, g_nextWaypointy)
    end

end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Updates the position of the drone to the network.
--/////////////////////////////////////////////////////////////////////////////////////////////
function updateDronePosition()
	local droneName, dronePosition = getDroneInfoFromId(g_myDroneId)
	if(droneTargetPosition ~= nil) then
		simExtMadaraQuadrotorControlUpdateStatus(g_myDroneId, dronePosition[1], dronePosition[2], dronePosition[3])
	end
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Check if we have found a person to stop on top of it.
--/////////////////////////////////////////////////////////////////////////////////////////////
function lookForPersonBelow()
    -- Get my drone position.
    local droneName, dronePos = getDroneInfoFromSuffix(g_mySuffix)

    -- Check if we found a person, to stop.
    local counter = 1
    local margin = 0.2
    for i=1, g_numPeople, 1 do
        if( (dronePos[1] >= g_personCoords[counter] - margin) and (dronePos[1] <= g_personCoords[counter] + margin) ) then
            if((dronePos[2] >= g_personCoords[counter + 1] - margin) and (dronePos[2] <= g_personCoords[counter + 1] + margin)) then
                -- Notifiy our shared memory that a person was found, and that I was the one to find it.
                local sourceSuffix, sourceName = simGetNameSuffix(nil)
                simSetScriptSimulationParameter(sim_handle_main_script, 'personFound', 'true')
                simSetScriptSimulationParameter(sim_handle_main_script, 'droneThatFound', sourceSuffix)
                simSetScriptSimulationParameter(sim_handle_main_script, 'personFoundId', i)
                simAddStatusbarMessage('Drone with suffix ' .. sourceSuffix .. ' found person ' .. i .. '!')
            end
        end
        counter = counter + 2
    end
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Checks if there is movement to be done, or if a bridge has to be built.
--/////////////////////////////////////////////////////////////////////////////////////////////
function loadNewMovementCommand()
    local myNewX = nil
    local myNewY = nil
    
    -- Drone ids start from 0, while suffixes start from -1.
    local myDroneId = g_mySuffix + 1       
    
    -- Behavior will depend on whether external Madara drones perform the calculations, or not.
    if(g_madaraClientEnabled) then      
        -- We wait to get the coordinates of our new position, if any, from the external drones.
        --simAddStatusbarMessage('(In ' .. g_myDroneName .. ') Checking movement status.')
        --simAddStatusbarMessage('Calling external C++ Madara plugin to get remotely calculated position for drone ' .. g_myDroneName .. ' with id ' .. myDroneId .. '.')

		-- Altitude and command are ignored for now, we assume all commands are move_to_gps.
        command, myNewX, myNewY, myNewAlt = simExtMadaraQuadrotorControlGetNewCmd(myDroneId)
        if(simExtMadaraQuadrotorControlIsGoToCmd(command) and myNewX ~= nil) then
            --simAddStatusbarMessage('(In ' .. g_myDroneName .. ') Got new movement.')
            -- We also get the bridging status. If the device is now bridging, we set the corresponding flags for the simulation.
			bridging = simExtMadaraClientIsBridging(myDroneId)
            if(bridging == 1) then  
                simAddStatusbarMessage('(In ' .. g_myDroneName .. ') Stopped patrolling, now bridging.')            
                g_patrolling = false
                g_bridging = true 
            else
                g_patrolling = true
            end            
        end
    else
        -- If a person was found, recalculate new location so that we create a bridge to the sink.
        local personHasBeenFound = simGetScriptSimulationParameter(sim_handle_main_script, 'personFound')
    
        -- Actually calculate if we are in the bridge.
        if(personHasBeenFound and not g_personChecked) then
            simAddStatusbarMessage('(In ' .. g_myDroneName .. ') Someone found a person, check if I have to stop patrolling and move into bridge-forming mode')
            myNewX, myNewY = buildBridge()

            -- If we are going to a bridge, upate our internal flags to reflect that.
            if(myNewX ~= nil) then
                g_patrolling = false
                g_bridging = true    

                -- Overwrite the final destination variable so the drone will just stop there.
                g_endx = myNewX
                g_endy = myNewY
            end            

            -- Flag to mark that bridges will only be made for the first person found.
            g_personChecked = true  
        end
    end

    -- If we have to move to a new location, set everything to move to our position in it.
    if(myNewX ~= nil) then
        simAddStatusbarMessage('(In ' .. g_myDroneName .. ', id=' .. myDroneId .. ') In Lua, target position found: ' .. myNewX .. ',' .. myNewY)
        -- Overwrite the next destination variables to make the drone move to its brige location.
        g_nextWaypointx = myNewX
        g_nextWaypointy = myNewY
    end
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Moves the target to a new position, so the drone will follow it there.
--/////////////////////////////////////////////////////////////////////////////////////////////
function moveTargetToPosition(newPositionX, newPositionY)
    -- Get the current position of the target.
    local droneTargetHandle = simGetObjectHandle('Quadricopter_target')
    local droneTargetPosition = simGetObjectPosition(droneTargetHandle, -1)
    
    -- The speed defines how far the target moves, and therefore how fast the drone will follow.
	local speed = 0.02        
    
    --simAddStatusbarMessage('(In ' .. g_myDroneName .. ') init target pos: ' .. droneTargetPosition[1] .. ', ' .. droneTargetPosition[2] )
    
    -- Check if the target is already at the required X position. If not, define that the
    -- new X position is our current plus the speed we move at in the correct direction.
    if(droneTargetPosition[1] > newPositionX) then        
        droneTargetPosition[1] = droneTargetPosition[1] - speed
        --simAddStatusbarMessage('(In ' .. g_myDroneName .. ') minus x speed')
    else
        droneTargetPosition[1] = droneTargetPosition[1] + speed
        --simAddStatusbarMessage('(In ' .. g_myDroneName .. ') plus x speed')
    end

    -- Check if the target is already at the required Y position. If not, define that the
    -- new Y position is our current plus the speed we move at in the correct direction.    
    if(droneTargetPosition[2] > newPositionY) then
        droneTargetPosition[2] = droneTargetPosition[2] - speed
        --simAddStatusbarMessage('(In ' .. g_myDroneName .. ') minus y speed')
    else
        droneTargetPosition[2] = droneTargetPosition[2] + speed
        --simAddStatusbarMessage('(In ' .. g_myDroneName .. ') plus y speed')
    end
       
    --simAddStatusbarMessage('(In ' .. g_myDroneName .. ') final target pos: ' .. droneTargetPosition[1] .. ', ' .. droneTargetPosition[2] )
       
    -- Move the target to a new position, so the drone will follow it there.
    simSetObjectPosition(droneTargetHandle, -1, droneTargetPosition)
end

--/////////////////////////////////////////////////////////////////////////////////////////////
--/////////////////////////////////////////////////////////////////////////////////////////////
-- NATIVE AREA COVERAGE FUNCTIONS!
--/////////////////////////////////////////////////////////////////////////////////////////////
--/////////////////////////////////////////////////////////////////////////////////////////////

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Private method used to setup the search pattern (area coverage) variables.
--/////////////////////////////////////////////////////////////////////////////////////////////
function setupSearchPattern()
	-- Divide the search area into squares and select the grid that this
	-- drone will patrol.

	--GET ALL FACTORS FOR THE # OF DRONES ----------------------------
	local nNumberToFactor = g_numDrones
	local nCurrentUpper = g_numDrones
	local factors = {1, g_numDrones}
	local count = 3
	local i = 2
	while (i<nCurrentUpper) do
		if((nNumberToFactor % i) == 0) then
			--if we found a factor, the upper number is the new upper limit 
			nCurrentUpper = nNumberToFactor / i
			factors[count] = i
			count = count + 1
			factors[count] = nCurrentUpper
			count = count + 1
		end
		i = i + 1
	end

	--GET THE 2 CLOSEST FACTORS ----------------------------
	local factorX = 0
	local factorY = 0
	local minDelta = 9999;
	for i=1, table.getn(factors), 2 do
		local a = factors[i]
		local b = factors[i+1]
		local delta = math.abs(a-b)

		if (delta < minDelta) then
				minDelta = delta
				factorX = a
				factorY = b
		end
	end

    --GET AREA AND CURRENT DRONE INFO  ----------------------
	local x1 = simGetScriptSimulationParameter(sim_handle_main_script, 'x1')
	local y1 = simGetScriptSimulationParameter(sim_handle_main_script, 'y1')
	local x2 = simGetScriptSimulationParameter(sim_handle_main_script, 'x2')
	local y2 = simGetScriptSimulationParameter(sim_handle_main_script, 'y2')
    
	local droneTargetHandle = simGetObjectHandle('Quadricopter_target')
	local droneTargetName = simGetObjectName(droneTargetHandle)     
    
	--CALCULATE DRONES CELL ----------------------
	local deltaX = (x2-x1)/factorX
	local deltaY = (y2-y1)/factorY

	local counter = 0
    for i=0, factorX - 1, 1 do
        for j=0, factorY - 1, 1 do
            if(counter==0 and droneTargetName=='Quadricopter_target') then
                g_startx = x1 + (i*deltaX)
                g_starty = y1 + (j*deltaY)
                g_endx = g_startx + deltaX
                g_endy = g_starty + deltaY
            elseif(droneTargetName=='Quadricopter_target#' ..counter) then
                g_startx = x1 + (i*deltaX)
                g_starty = y1 + (j*deltaY)
                g_endx = g_startx + deltaX
                g_endy = g_starty + deltaY
                counter = counter+1
            else
                counter = counter+1
            end
        end
    end

    -- Indicate that the target starts trying to move "down" Y, which is the first direction it will go,
	g_down = true
    
    -- Set the next destination for the target as the initial location.
	g_nextWaypointx = g_startx
	g_nextWaypointy = g_starty

end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Moves the target to a new position, so the drone will follow it there.
--/////////////////////////////////////////////////////////////////////////////////////////////
function updateAreaCoverageWaypoint()
    -- Check if the target reached its next waypoint.
    local atNextWaypoint = isTargetAtLocation(g_nextWaypointx, g_nextWaypointy)
    if(atNextWaypoint) then
        -- If we enter here, it means the target has reached its next waypoint in the search pattern path.
        -- Alternate between changing the Y and X destinations.
        if(g_down) then
            -- Define the next Y coordinate of the next destination/step of the target.
            if (g_nextWaypointy == g_starty) then 
                -- Indicates that the new destination is the lower end of the quadrant, in terms of Y (we have to go "down" through the quadrant).
                g_nextWaypointy = g_endy 
            else 
                -- Indicates that the new destination is the lower end of the quadrant, in terms of Y (we have to go back "up").
                g_nextWaypointy = g_starty 
            end
            
            g_down = false
        else
            -- If we had reached our Y destination, now it is time to move on X as well... toward the next vertical line of our search.
            g_nextWaypointx = g_nextWaypointx - 0.5
            g_down = true
        end
    end
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Checks if the drone's target has reached a certain location.
--/////////////////////////////////////////////////////////////////////////////////////////////
function isTargetAtLocation(locationx, locationy)
    -- Get the current position of the target.
    local droneTargetHandle = simGetObjectHandle('Quadricopter_target')
    local droneTargetPosition = simGetObjectPosition(droneTargetHandle, -1)    
    
    -- Accuracy of how close to the waypoint we define as actually at the waypoint.
	local accuracy = 0.02

    -- Check if the target is already at the required X position.
    local atLocationX = false
    local deltax = math.abs(droneTargetPosition[1] - locationx)
    if(deltax <= accuracy) then
        atLocationX = true
    end

    -- Check if the target is already at the required Y position.
    local atLocationY = false
    local deltay = math.abs(droneTargetPosition[2] - locationy)
    if(deltay <= accuracy) then
        atLocationY = true
    end
    
    -- We are at that location if we are there for both coordinates.
    return atLocationX and atLocationY
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Checks if the we have reached our final destination.
--/////////////////////////////////////////////////////////////////////////////////////////////
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
        g_bridging = false

        -- We just return, since we won't move anymore.
        return        
    end        
end  

--/////////////////////////////////////////////////////////////////////////////////////////////
--/////////////////////////////////////////////////////////////////////////////////////////////
-- NATIVE BRIDGE FUNCTIONS!
--/////////////////////////////////////////////////////////////////////////////////////////////
--/////////////////////////////////////////////////////////////////////////////////////////////

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Calculates the drones required for a bridge, and if I am one, set everything so I will go to
-- my location.
--/////////////////////////////////////////////////////////////////////////////////////////////
function buildBridge()
    simAddStatusbarMessage('(In ' .. g_myDroneName .. ') locally calculating bridge.')
    local myNewX = nil
    local myNewY = nil
    
    -- In this case we will not be using the external Madara drones to build the bridge, we will make the call locally for each drone.
    g_startTime = simGetSimulationTime()
    g_startSystemTime = simGetSystemTimeInMilliseconds()

    -- Get position of sink and source
    local sinkName, sinkPosition = getSinkInfo()
    local droneSourceName, sourcePosition = getSourceInfo()
    simAddStatusbarMessage('Source at '  .. sourcePosition[1] .. ', ' .. sourcePosition[2])
    
    -- Get the radio range
    local radioRange = simGetScriptSimulationParameter(sim_handle_main_script,'radioRange')   

    -- Get all drone positions
    local availableDroneIdsIdx = 1;
    local availableDroneIds = {}
    local availableDronePositionsMap = {}
    local availableDronePositionsArray = {}
    for i=1, g_numDrones, 1 do
        local currDroneId = i-1         -- Actual drone IDs start at 0, but Lua table indexes start at 1.
        local curentDroneName, currentDronePos = getDroneInfoFromId(currDroneId)
        
        if(curentDroneName ~= droneSourceName) then
            -- We have to store the IDs in a separate, simple table to be able to pass this to the C function in the plugin (which doesnt accept nested tables).
            availableDroneIds[availableDroneIdsIdx] = currDroneId
            
            -- We have to store the positions in a single-level array to pass this to the C function in the plugin (which doesnt accept nested tables).
            availableDronePositionsArray[2*availableDroneIdsIdx-1] = currentDronePos[1]
            availableDronePositionsArray[2*availableDroneIdsIdx-1+1] = currentDronePos[2]
            
            -- This array is only used by the internal implementation of the algorithm if enabled.
            availableDronePositionsMap[curentDroneName] = currentDronePos
            
            availableDroneIdsIdx = availableDroneIdsIdx + 1                
        end
    end
    
    -- Obtains the position I have to go to to form the bridge, if I am best suited to help with the bridge.
    local useExternalPlugin = simGetScriptSimulationParameter(sim_handle_main_script, 'useExternalPlugin')
    if(useExternalPlugin) then        
        -- Drone ids start from 0, while suffixes start from -1.
        local myDroneId = g_mySuffix + 1           

        -- Call external C function to calculate the position.
        simAddStatusbarMessage('Calling external, C++ bridge-building function.')        
        myNewX, myNewY = simExtGetPositionInBridge(myDroneId, radioRange, sourcePosition, sinkPosition, availableDroneIds, availableDronePositionsArray)        
    else        
        -- Call internal function to calculate bridge position.        
        simAddStatusbarMessage('Calling internal bridge-building function.')        
        myNewX, myNewY = getPositionInBridge(g_myDroneName, radioRange, sourcePosition, sinkPosition, availableDronePositionsMap)
    end

    return myNewX, myNewY
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Calculate if this drone has to move somewhere, and if so where, to be part of a bridge.
--/////////////////////////////////////////////////////////////////////////////////////////////
function getPositionInBridge(myId, radioRange, sourcePosition, sinkPosition, availableDronePositions)   
    -- Calculate how many drones are required for the bridge.
    distanceSourceSink = math.sqrt((sourcePosition[1] - sinkPosition[1])^2 + (sourcePosition[2] - sinkPosition[2])^2)
    dronesNeededForBridge = math.ceil(distanceSourceSink/radioRange - 1)
    --simAddStatusbarMessage('Distance source sink ' .. sourcePosition[1] .. ',' ..  sinkPosition[1])
    simAddStatusbarMessage('Drones required: ' .. dronesNeededForBridge .. ' for radio range ' .. radioRange)

    -- Calculate the locations for each drone to form the bridge.
    relayCoords = {}
    for i=1, dronesNeededForBridge, 1 do
        relayCoords[i] = {}
        relayCoords[i][1] = sinkPosition[1] + (sourcePosition[1] - sinkPosition[1])/(dronesNeededForBridge+1)*(i)
        relayCoords[i][2] = sinkPosition[2] + (sourcePosition[2] - sinkPosition[2])/(dronesNeededForBridge+1)*(i)
        simAddStatusbarMessage('Location ' .. i .. ': ' .. relayCoords[i][1] ..','..relayCoords[i][2])
    end
    
    -- Calculate distances from all drones to each relay position.
    distanceTuples = {}
    for currDroneId, currentDronePos in pairs(availableDronePositions) do
        -- Go over each relay location.
        for j=1,dronesNeededForBridge,1 do
            currRelayCoords = relayCoords[j]
            distanceToLocation = math.sqrt((currentDronePos[1] - currRelayCoords[1])^2 + (currentDronePos[2] - currRelayCoords[2])^2)
            --simAddStatusbarMessage('Distance from drone ' .. droneName .. ' to location ' .. currRelayCoords[1] ..','..currRelayCoords[2] .. ' is ' .. distanceToLocation)

            -- Store everything in a table.
            distanceTuple = {currDroneId, j, distanceToLocation}
            table.insert(distanceTuples,distanceTuple)
        end
    end	

    -- Sort the table with all the distances (from all drones to all relay locations) in ascending order by distance.
    table.sort(distanceTuples, compare)

    -- Find the best drone-location pairs
    local usedDrones = {}
    local usedLocations = {}
    for index, val in ipairs(distanceTuples) do
        currDroneId = val[1]
        currLocationIdx = val[2]
        currDistance = val[3]

        -- Ignore drones that have already been assigned to a location, and locations which already have drones assigned to them.
        if(not usedDrones[currDroneId] and not usedLocations[currLocationIdx]) then
            -- Mark this drone and this location as assigned.
            usedDrones[currDroneId] = true
            usedLocations[currLocationIdx] = true
            simAddStatusbarMessage("Selected drone "..currDroneId.." to move to location "..currLocationIdx)

            -- If I am the drone that has just been selected, the update my new target location.
            if myId == currDroneId then
                -- Get the location coords to set as my future target.
                myNewX = relayCoords[currLocationIdx][1]
                myNewY = relayCoords[currLocationIdx][2]
                --simAddStatusbarMessage('I am ' .. g_myDroneName .. ' going to form bridge at ' .. myNewX .. ', ' .. myNewY)
                endTime = simGetSimulationTime()
                elapsedTime = endTime - g_startTime
                endSystemTime = simGetSystemTimeInMilliseconds()
                elapsedSystemTime = endSystemTime - g_startSystemTime
                simAddStatusbarMessage('Drone ' .. currDroneId .. ' took ' .. elapsedTime .. 's and ' .. elapsedSystemTime .. ' system ms')
                
                -- Stop the loop since we only care about where we have to go.
                return myNewX, myNewY
            end
        end
    
    end
    
    return nil, nil
end
