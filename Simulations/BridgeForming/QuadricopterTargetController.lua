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

    -- Get the total number of drones.
	g_numDrones = simGetScriptSimulationParameter(sim_handle_main_script, 'numberOfDrones')

    -- Setup the search pattern, by defining the next and final destinations for the targets.
    setupSearchPattern()

    -- Load the positions of people on the grid, so we will know when we find one.
    loadPeoplePositions()
    
    -- Indicates if we are using the Madara client for communication with external "drones".
    g_madaraClientEnabled = simGetScriptSimulationParameter(sim_handle_main_script, 'madaraClientOn')       

end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Private method used to setup the search pattern (area coverage) variables.
--/////////////////////////////////////////////////////////////////////////////////////////////
function setupSearchPattern()
	local x1 = simGetScriptSimulationParameter(sim_handle_main_script, 'x1')
	local y1 = simGetScriptSimulationParameter(sim_handle_main_script, 'y1')
	local x2 = simGetScriptSimulationParameter(sim_handle_main_script, 'x2')
	local y2 = simGetScriptSimulationParameter(sim_handle_main_script, 'y2')
    
	local droneTargetHandle = simGetObjectHandle('Quadricopter_target')
	local droneTargetName = simGetObjectName(droneTargetHandle)    
    
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

    -- Set the next destination for the target as the initial location.
	g_destinationx = g_startx
	g_destinationy = g_starty

    -- Indicate that the target starts trying to move "down" Y, which is the first direction it will go,
	g_down = true

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
    -- If a person was found by someone else, recalculate new location so that we create a bridge to the sink.
    local personHasBeenFound = simGetScriptSimulationParameter(sim_handle_main_script, 'personFound')
    if(g_patrolling and personHasBeenFound and not g_personChecked) then
        buildBridge()
    end

    -- Check if we have found a person to stop on top of it (only if we are patrolling).
    if(g_patrolling) then
        lookForPersonBelow()
    end

    -- If we are still patrolling or bridging, move to next scheduled position.
    if(g_patrolling or g_bridging) then
        moveTargetToNextPosition()
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
                -- We found someone. First mark area coverage as done, to prevent further movements.
                g_patrolling = false
                g_bridging = false

                -- Notifiy our shared memory that a person was found, and that I was the one to find it.
                local sourceSuffix, sourceName = simGetNameSuffix(nil)
                simSetScriptSimulationParameter(sim_handle_main_script, 'personFound', 'true')
                simSetScriptSimulationParameter(sim_handle_main_script, 'droneThatFound', sourceSuffix)
                simAddStatusbarMessage('Person found! ' .. tostring(simGetScriptSimulationParameter(sim_handle_main_script, 'personFound')))
                simAddStatusbarMessage('By ' ..sourceSuffix)
                
                -- If enabled, notify through Madara that we need a bridge.
                if(g_madaraClientEnabled) then
                    -- Madara Drone IDs start at 0, and V-Rep suffixes start at -1.
                    local sourceDroneId = sourceSuffix + 1
                    
                    -- Do the actual call to Madara.
                    simExtMadaraClientBridgeRequest(sourceDroneId)
                end

                break
            end
        end
        counter = counter + 2
    end
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Calculates the drones required for a bridge, and if I am one, set everything so I will go to
-- my location.
--/////////////////////////////////////////////////////////////////////////////////////////////
function buildBridge()
    simAddStatusbarMessage('(In ' .. g_myDroneName .. ') Someone found a person, check if I have to stop patrolling and move into bridge-forming mode')
    local myNewX = nil
    local myNewY = nil

    local myDroneId = g_mySuffix + 1   
    
    -- Behavior will depend on whether external Madara drones perform the calculations, or not.
    if(g_madaraClientEnabled) then  
        -- We wait to get the coordinates of our new position, if any, from the external drones.
        simAddStatusbarMessage('Calling external, C++ Madara plugin to get remotely calculated position.')
        myNewX, myNewY = simExtMadaraClientGetPositionInBridge(myDroneId)
        if(myNewX == nil and myNewY == nil) then
            simAddStatusbarMessage('Returned position is nil.')
        end
    else
        -- In this case we will not be using the external Madara drones to build the bridge, we will make the call locally for each drone.
        g_startTime = simGetSimulationTime()
        g_startSystemTime = simGetSystemTimeInMilliseconds()

        -- Get position of sink and source
        local sinkPosition = getSinkPosition()
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
            -- Call external C function to calculate the position.
            simAddStatusbarMessage('Calling internal bridge-building function.')
            myNewX, myNewY = simExtGetPositionInBridge(myDroneId, radioRange, sourcePosition, sinkPosition, availableDroneIds, availableDronePositionsArray)        
        else        
            -- Call internal function to calculate bridge position.        
            simAddStatusbarMessage('Calling external, C++ bridge-building function.')
            myNewX, myNewY = getPositionInBridge(g_myDroneName, radioRange, sourcePosition, sinkPosition, availableDronePositionsMap)
        end
    end

    -- If we are part of the new bridge, set everything to move to our position in it.
    if(myNewX ~= nil) then
        simAddStatusbarMessage('In Lua, position found: ' .. myNewX .. ',' .. myNewY)
        -- Overwrite the next destination variables to make the drone move to its brige location.
        g_destinationx = myNewX
        g_destinationy = myNewY

        -- Overwrite the final destination variable so the drone will just stop there.
        g_endx = myNewX
        g_endy = myNewY        
    
        g_patrolling = false
        g_bridging = true            
    end

    --if(not g_madaraClientEnabled) then  
        -- Flag to mark that bridges will only be made for the first person found.
        g_personChecked = true
    --end
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Moves the target to a new position, so the drone will follow it there.
--/////////////////////////////////////////////////////////////////////////////////////////////
function moveTargetToNextPosition()
    -- Get the current position of the target, and how far it is from the desination we are headed to.
    local droneTargetHandle = simGetObjectHandle('Quadricopter_target')
    local droneTargetPosition = simGetObjectPosition(droneTargetHandle, -1)
    local deltax = math.abs(droneTargetPosition[1] - g_destinationx)
    local deltay = math.abs(droneTargetPosition[2] - g_destinationy)
    
    -- Define limits.
	local accuracy = 0.02
	local speed = 0.02    

    -- Check if the target is already at the required X position. If not, define that the
    -- new X position is our current plus the speed we move at in the correct direction.
    local atdestinationx = false
    if(droneTargetPosition[1] > g_destinationx and deltax > accuracy) then
        droneTargetPosition[1] = droneTargetPosition[1] - speed
        atdestinationx = false
    elseif(droneTargetPosition[1] < g_destinationx and deltax > accuracy) then
        droneTargetPosition[1]=droneTargetPosition[1] + speed
        atdestinationx = false
    else
        atdestinationx = true
    end

    -- Check if the target is already at the required Y position. If not, define that the
    -- new Y position is our current plus the speed we move at in the correct direction.    
    local atdestinationy = false
    if(droneTargetPosition[2] > g_destinationy and deltay > accuracy) then
        droneTargetPosition[2] = droneTargetPosition[2] - speed
        atdestinationy = false
    elseif(droneTargetPosition[2] < g_destinationy and deltay > accuracy) then
        droneTargetPosition[2]=droneTargetPosition[2] + speed
        atdestinationy = false
    else
        atdestinationy = true
    end

    -- Check if the target reached its destination.
    if(atdestinationx and atdestinationy) then
        -- If we enter here, it means the target has reached its next step or destination in the
        -- search pattern path.
    
        -- Check how far the target is from the final end of its path.
        deltax = math.abs(droneTargetPosition[1] - g_endx)
        deltay = math.abs(droneTargetPosition[2] - g_endy)

        -- If the target is within a certain range of the final path, indicate that 
        -- we are no longer g_patrolling or g_bridging, since we will be stopped.
        if(deltax < 0.3 and deltay < 0.3) then
            g_patrolling = false
            g_bridging = false
        end

        -- Alternate between changing the Y and X destinations.
        if(g_down) then
            -- Define the next Y coordinate of the next destination/step of the target.
            if (g_destinationy == g_starty) then 
                -- Indicates that the new destination is the lower end of the quadrant, in terms of Y (we have to go "down" through the quadrant).
                g_destinationy = g_endy 
            else 
                -- Indicates that the new destination is the lower end of the quadrant, in terms of Y (we have to go back "up").
                g_destinationy = g_starty 
            end
            
            g_down = false
        else
            -- If we had reached our Y destination, now it is time to move on X as well... toward the next vertical line of our search.
            g_destinationx = g_destinationx - 0.5
            g_down = true
        end
    end

    -- Move the target to a new position, so the drone will follow it there.
    simSetObjectPosition(droneTargetHandle, -1, droneTargetPosition)
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
