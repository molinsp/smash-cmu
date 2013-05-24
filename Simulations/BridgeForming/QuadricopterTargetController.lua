--####################################################################
--# Usage of this software requires acceptance of the SMASH-CMU License,
--# which can be found at the following URL:
--#
--# https://code.google.com/p/smash-cmu/wiki/License
--######################################################################

function doInitialSetup()
	-- Indicates whether we are patrolling our zone or not.
	patrolling = true
	personChecked = false

	-- Indicates whether we have moved into bridge-forming mode or not.
	bridging = false

	-- Get general parameters and current object values
	handle = simGetObjectHandle('Quadricopter_target')
	name = simGetObjectName(handle)

	-- Get my name
	mySuffix = simGetNameSuffix(nil)
    myDroneName = getDroneInfoFromSuffix(mySuffix)

	x1=simGetScriptSimulationParameter(sim_handle_main_script,'x1')
	y1=simGetScriptSimulationParameter(sim_handle_main_script,'y1')
	x2=simGetScriptSimulationParameter(sim_handle_main_script,'x2')
	y2=simGetScriptSimulationParameter(sim_handle_main_script,'y2')
	numDrones=simGetScriptSimulationParameter(sim_handle_main_script,'numberOfDrones')

	-- Divide the search area into squares and select the grid that this
	-- drone will patrol.

	--GET ALL FACTORS FOR THE # OF DRONES ----------------------------
	nNumberToFactor = numDrones
	nCurrentUpper = numDrones
	factors = {1, numDrones}
	count=3
	i=2
	while (i<nCurrentUpper) do
		if((nNumberToFactor % i) == 0) then
			--if we found a factor, the upper number is the new upper limit 
			nCurrentUpper = nNumberToFactor / i
			factors[count]=i
			count=count+1
			factors[count]=nCurrentUpper
			count=count+1
		end
		i=i+1
	end

	--GET THE 2 CLOSEST FACTORS ----------------------------
	factorX=0
	factorY=0
	minDelta=9999;
	for i=1, table.getn(factors), 2 do
		a = factors[i]
		b = factors[i+1]
		delta = math.abs(a-b)

		if (delta<minDelta) then
				minDelta=delta
				factorX = a
				factorY = b
		end
	end

	--CALCULATE DRONES CELL ----------------------
	deltaX = (x2-x1)/factorX
	deltaY = (y2-y1)/factorY

	counter=0
		for i=0, factorX-1, 1 do
			for j=0, factorY-1, 1 do
				if(counter==0 and name=='Quadricopter_target') then
					startx = x1+(i*deltaX)
					starty = y1+(j*deltaY)
					endx = startx + deltaX
					endy = starty + deltaY
				elseif(name=='Quadricopter_target#' ..counter) then
					startx = x1+(i*deltaX)
					starty = y1+(j*deltaY)
					endx = startx + deltaX
					endy = starty + deltaY
					counter=counter+1
				else
					counter=counter+1
			end
		end
	end

	destinationx=startx
	destinationy=starty
	accuracy=0.02
	speed=0.02
	atdestinationx=false
	atdestinationy=false
	down=true

	--/////////////////////////////////////////////////////////////////////////////////////////////
	-- Setup the people's locations, so we are able to check when we find one.
	numPeople=simGetScriptSimulationParameter(sim_handle_main_script,'numberOfPeople')
	--simAddStatusbarMessage('People: ' .. numPeople)
	counter=1
	billCoords={}
	billposition={}
	for i=1,numPeople,1 do
		if(i==1) then
			personHandle=simGetObjectHandle('Bill#')
		else
			personHandle=simGetObjectHandle('Bill#' .. (i-2))
		end

		billposition=simGetObjectPosition(personHandle, -1)
		billCoords[counter]=billposition[1]
		billCoords[counter+1]=billposition[2]
		--simAddStatusbarMessage('Person ' .. counter .. ' : ' .. billCoords[counter] .. ', ' .. counter+1 .. ' : '..billCoords[counter+1])
		counter=counter+2
	end
	--/////////////////////////////////////////////////////////////////////////////////////////////
    
    -- Indicates if we are using the Madara client for communication with external "drones".
    madaraClientEnabled = simGetScriptSimulationParameter(sim_handle_main_script, 'madaraClientOn')      
    
    -- Setup Madara client.
    myControllerId = 100
    if(madaraClientEnabled) then  
        local radioRange = simGetScriptSimulationParameter(sim_handle_main_script, 'radioRange')      
        simExtMadaraClientSetup(myControllerId, radioRange)
    end

end

function runMainLogic()
    -- If enabled, update the status in each step through Madara.
    if(madaraClientEnabled) then  
        simExtMadaraClientUpdateStatus()
    end

    --/////////////////////////////////////////////////////////////////////////////////////////////
    -- If a person was found by someone else, recalculate new location so that we create a bridge to the sink.
    personHasBeenFound = simGetScriptSimulationParameter(sim_handle_main_script,'personFound')
    if(not personChecked and patrolling and personHasBeenFound) then
        simAddStatusbarMessage('(In ' .. name .. ') Someone found a person, check if I have to stop patrolling and move into bridge-forming mode')
        startTime = simGetSimulationTime()
        startSystemTime = simGetSystemTimeInMilliseconds()

        -- Get position of sink and source
        local sinkPosition = getSinkPosition()
        local droneSourceName, sourcePosition = getSourceInfo()
        simAddStatusbarMessage('Source at '  .. sourcePosition[1] .. ', ' .. sourcePosition[2])
        
        -- Get the radio range
        local radioRange = simGetScriptSimulationParameter(sim_handle_main_script,'radioRange')   

        -- Get all drone positions
        local myDroneId = mySuffix + 1
        local availableDroneIdsIdx = 1;
        availableDroneIds = {}
        availableDronePositionsMap = {}
        availableDronePositionsArray = {}
        for i=1, numDrones, 1 do
            droneName = ''
            if (i == 1) then
                droneName = 'Quadricopter#'
            else
                droneName = 'Quadricopter#' .. i-2
            end

            currentDroneHandle = simGetObjectHandle(droneName)
            currentDronePos = simGetObjectPosition(currentDroneHandle, -1)
            
            if(droneName ~= droneSourceName) then
                -- We have to store the IDs in a separate, simple table to be able to pass this to the C function in the plugin (which doesnt accept nested tables).
                local currDroneId = i-1
                availableDroneIds[availableDroneIdsIdx] = currDroneId
                
                -- We have to store the positions in a single-level array to pass this to the C function in the plugin (which doesnt accept nested tables).
                availableDronePositionsArray[2*availableDroneIdsIdx-1] = currentDronePos[1]
                availableDronePositionsArray[2*availableDroneIdsIdx-1+1] = currentDronePos[2]
                
                -- This array is only used by the internal implementation of the algorithm if enabled.
                availableDronePositionsMap[droneName] = currentDronePos
                
                availableDroneIdsIdx = availableDroneIdsIdx + 1                
            end
        end
        
        -- Obtains the position I have to go to to form the bridge, if I am best suited to help with the bridge.
        local useExternalPlugin = simGetScriptSimulationParameter(sim_handle_main_script, 'useExternalPlugin')
        if(useExternalPlugin) then        
            -- Call external C function to calculate the position.
            myNewX, myNewY = simExtGetPositionInBridge(myDroneId, radioRange, sourcePosition, sinkPosition, availableDroneIds, availableDronePositionsArray)        
        else        
            -- Call internal function to calculate bridge position.        
            myNewX, myNewY = getPositionInBridge(myDroneName, radioRange, sourcePosition, sinkPosition, availableDronePositionsMap)
        end

        if(myNewX ~= nil) then
            simAddStatusbarMessage('In Lua, position found: ' .. myNewX .. ',' .. myNewY)
            -- Overwrite the next destination variables to make the drone move to its brige location.
            destinationx = myNewX
            destinationy = myNewY

            -- Overwrite the final destination variable so the drone will just stop there.
            endx = myNewX
            endy = myNewY        
        
            patrolling = false
            bridging = true            
        end

        personChecked = true
        --/////////////////////////////////////////////////////////////////////////////////////////////
    end
    --/////////////////////////////////////////////////////////////////////////////////////////////

    --/////////////////////////////////////////////////////////////////////////////////////////////
    -- Check if we have found a person to stop on top of it.
    if(patrolling) then
        position=simGetObjectPosition(handle,-1)

        -- First check if we found a person, to stop.
        dronePos = position
        counter=1
        for i=1,numPeople,1 do
        if( (dronePos[1] >= billCoords[counter]-0.2) and (dronePos[1] <= billCoords[counter]+0.2) ) then
            if((dronePos[2] >= billCoords[counter+1]-0.2) and (dronePos[2] <= billCoords[counter+1]+0.2)) then
                -- We found someone. First mark area coverage as done, to prevent further movements.
                patrolling = false
                bridging = false

                -- Notifiy our shared memory that a person was found, and that I was the one to find it.
                suffix, name = simGetNameSuffix(nil)
                simSetScriptSimulationParameter(sim_handle_main_script,'personFound','true')
                simSetScriptSimulationParameter(sim_handle_main_script,'droneThatFound', suffix)
                simAddStatusbarMessage('Person found! ' .. tostring(simGetScriptSimulationParameter(sim_handle_main_script,'personFound')))
                simAddStatusbarMessage('By ' ..suffix)

                break
            end
        end
            counter=counter+2
        end
    end
    --/////////////////////////////////////////////////////////////////////////////////////////////

    -- If I have not found a person, move to next scheduled position.
    if(patrolling or bridging) then
        deltax=math.abs(position[1]-destinationx)
        deltay=math.abs(position[2]-destinationy)

        if(position[1] > destinationx and deltax > accuracy) then
            position[1]=position[1]-speed
            atdestinationx=false
        elseif(position[1] < destinationx and deltax > accuracy) then
            position[1]=position[1]+speed
            atdestinationx=false
        else
            atdestinationx=true
        end

        if(position[2] > destinationy and deltay > accuracy) then
            position[2]=position[2]-speed
            atdestinationy=false
        elseif(position[2] < destinationy and deltay > accuracy) then
            position[2]=position[2]+speed
            atdestinationy=false
        else
            atdestinationy=true
        end

        if(atdestinationx and atdestinationy) then

            deltax=math.abs(position[1]-endx)
            deltay=math.abs(position[2]-endy)

            if(deltax < 0.3 and deltay < 0.3) then
                patrolling=false
                bridging=false
            end

            if(down) then
                if (destinationy==starty) then 
                    destinationy=endy 
                else 
                    destinationy=starty 
                end
                down=false
            else
                destinationx=destinationx-0.5
                down=true
            end
            atdestinationx=false
            atdestinationy=false
        end

        -- Actually move the object to the new position.
        simSetObjectPosition(handle, -1, position)
    end

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
                --simAddStatusbarMessage('I am ' .. myDroneName .. ' going to form bridge at ' .. myNewX .. ', ' .. myNewY)
                endTime = simGetSimulationTime()
                elapsedTime = endTime - startTime
                endSystemTime = simGetSystemTimeInMilliseconds()
                elapsedSystemTime = endSystemTime - startSystemTime
                simAddStatusbarMessage('Drone ' .. currDroneId .. ' took ' .. elapsedTime .. 's and ' .. elapsedSystemTime .. ' system ms')
                
                -- Stop the loop since we only care about where we have to go.
                return myNewX, myNewY
            end
        end
    
    end
    
    return nil, nil

end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Returns the position of the sink as a table with x,y,z
--/////////////////////////////////////////////////////////////////////////////////////////////
function getSinkPosition()
    -- Get position of sink.
    laptopHandle = simGetObjectHandle('laptop#')
    local sinkPosition = simGetObjectPosition(laptopHandle, -1)
    simAddStatusbarMessage('Sink at '  .. sinkPosition[1] .. ', ' .. sinkPosition[2])
    
    return sinkPosition
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Returns the position of the source drone as a table with x,y,z, as well as the drone's name.
--/////////////////////////////////////////////////////////////////////////////////////////////
function getSourceInfo()
    local sourceSuffix = simGetScriptSimulationParameter(sim_handle_main_script, 'droneThatFound')
    return getDroneInfoFromSuffix(sourceSuffix)
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Returns the name and position (as a x,y,x table) of a drone with a given suffix.
--/////////////////////////////////////////////////////////////////////////////////////////////
function getDroneInfoFromSuffix(suffix)
    local droneObjectName = 'Quadricopter#'

    -- For all drones but the first one (suffix -1), we have to add the suffix, which starts at 0.
    if(suffix ~= -1) then
        droneObjectName = droneObjectName .. suffix
    end
    
    -- Get the position from the drone object.
    local droneHandle = simGetObjectHandle(droneObjectName)
    local dronePosition = simGetObjectPosition(droneHandle, -1)    
    
    return droneObjectName, dronePosition    
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Returns the name and position (as a x,y,x table) of a drone with a given index (starting at 1).
--/////////////////////////////////////////////////////////////////////////////////////////////
function getDroneInfoFromIndex(index)
    local droneObjectName = 'Quadricopter#'

    -- For all drones but the first one (index 1), we have to add the suffix, which starts at 0 (index-2).
    if(suffix ~= 1) then
        droneObjectName = droneObjectName .. (index-2)
    end
    
    -- Get the position from the drone object.
    local droneHandle = simGetObjectHandle(droneObjectName)
    local dronePosition = simGetObjectPosition(droneHandle, -1)    
    
    return droneObjectName, dronePosition    
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Helper function used to sort a table by the third value in each tuple.
function compare(a,b)
  return a[3] < b[3]
end
--/////////////////////////////////////////////////////////////////////////////////////////////

