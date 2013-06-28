--####################################################################
--# Usage of this software requires acceptance of the SMASH-CMU License,
--# which can be found at the following URL:
--#
--# https://code.google.com/p/smash-cmu/wiki/License
--######################################################################

function writeToFile(string)
  fileOutput = io.open("output.csv", "a")
	fileOutput:write(string .. "\n")
	fileOutput:flush ()
	fileOutput:close ()
end

function writeToSummaryFile(string)
  fileSummary = io.open("summary.csv", "a")
	fileSummary:write(string .. "\n")
	fileSummary:flush ()
	fileSummary:close ()
end

function writeToFutureInputFile(string)
  fileInput = io.open("optimalInput.csv", "a")
	fileInput:write(string .. "\n")
	fileInput:flush ()
	fileInput:close ()
end

function doInitialSetup()

	-- Indicates whether we are patrolling our zone or not.
	patrolling = true
	personChecked = false
	inUse = true

	-- Indicates whether we have moved into bridge-forming mode or not.
	bridging = false

	-- Get general parameters and current object values
	handle=simGetObjectHandle('Quadricopter_target')
	name=simGetObjectName(handle)

	-- Get my name
	mySuffix = simGetNameSuffix(nil)
	if(mySuffix == -1) then
		myDroneName = 'Quadricopter#'
	else
		myDroneName = 'Quadricopter#' .. mySuffix
	end
	
  --simAddStatusbarMessage('Initializing drone target '..myDroneName)
  
  -- Jump target to drone.
  --droneHandle = simGetObjectHandle(myDroneName)
  --dronePosition = simGetObjectPosition(droneHandle, -1)
  --simSetObjectPosition(handle, -1, {dronePosition[1], dronePosition[2], dronePosition[3]})
  
	x1=simGetScriptSimulationParameter(sim_handle_main_script,'x1')
	y1=simGetScriptSimulationParameter(sim_handle_main_script,'y1')
	x2=simGetScriptSimulationParameter(sim_handle_main_script,'x2')
	y2=simGetScriptSimulationParameter(sim_handle_main_script,'y2')
	numDrones=simGetScriptSimulationParameter(sim_handle_main_script,'numberOfDrones')

  intSuffix = tonumber(mySuffix)

  if (numDrones < intSuffix + 2) then
    --simAddStatusbarMessage('dont use '..mySuffix)
    inUse = false
  end
  
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

end


function doResetTarget()
	
	-- Indicates whether we are patrolling our zone or not.
	patrolling = true
	personChecked = false
	inUse = true
	
	printerClaimed = false
  simSetScriptSimulationParameter(sim_handle_main_script, 'printerClaimed', tostring(printerClaimed), tostring(printerClaimed).len)

  printerName = 'empty'
    
	-- Indicates whether we have moved into bridge-forming mode or not.
	bridging = false

	x1=simGetScriptSimulationParameter(sim_handle_main_script,'x1')
  y1=simGetScriptSimulationParameter(sim_handle_main_script,'y1')
  x2=simGetScriptSimulationParameter(sim_handle_main_script,'x2')
  y2=simGetScriptSimulationParameter(sim_handle_main_script,'y2')
    
  jumpToX = (x2-x1)/2
  jumpToY = (y2-y1)/2
  jumpToZ = 1.4+0.4*(mySuffix+2)
    
  if(jumpToX < 0) then
    jumpToX = jumpToX * -1
  end
  if(jumpToY < 0) then
    jumpToY = jumpToY * -1
  end  
  --simAddStatusbarMessage('jumping target'..mySuffix)
  --simSetObjectPosition(handle, -1, {jumpToX, jumpToY, jumpToZ})

  doInitialSetup()

end


function runMainLogic()

    if (not inUse) then
      return
    end
    
    --/////////////////////////////////////////////////////////////////////////////////////////////
    -- If a person was found by someone else, recalculate new location so that we create a bridge to the sink.
    personHasBeenFound = simGetScriptSimulationParameter(sim_handle_main_script,'personFound')
    if(not personChecked and patrolling and personHasBeenFound) then
        --simAddStatusbarMessage('(In ' .. name .. ') Someone found a person, check if I have to stop patrolling and move into bridge-forming mode')
        startTime = simGetSimulationTime()
        startSystemTime = simGetSystemTimeInMilliseconds()

        -- Get position of sink.
        laptopHandle = simGetObjectHandle('laptop#')
        sinkPosition = simGetObjectPosition(laptopHandle, -1)
        
        claimed = simGetScriptSimulationParameter(sim_handle_main_script,'printerClaimed')
        if (not claimed) then
            claimed = 'true'
            simSetScriptSimulationParameter(sim_handle_main_script, 'printerClaimed', tostring(claimed), tostring(claimed).len)
            printerName = myDroneName
        end
        
        if (myDroneName == printerName) then
           writeToFile('Sink Location,'  .. sinkPosition[1] .. ', ' .. sinkPosition[2])
        end

        -- Get position of source
        sourceSuffix = simGetScriptSimulationParameter(sim_handle_main_script,'droneThatFound')

        if(sourceSuffix == -1) then
            sourceName = 'Quadricopter_target#'
            droneSourceName = 'Quadricopter#'
        else
            sourceName = 'Quadricopter_target#' .. sourceSuffix
            droneSourceName = 'Quadricopter#' .. sourceSuffix
        end

        sourceHandle = simGetObjectHandle(sourceName)
        sourcePosition = simGetObjectPosition(sourceHandle, -1)
        
        if (myDroneName == printerName) then
          writeToFile('Source Location,'  .. sourcePosition[1] .. ', ' .. sourcePosition[2])
        end

        --/////////////////////////////////////////////////////////////////////////////////////////////
        -- Calculate which drones have to move where.
        
        -- Calculate how many drones are required for the bridge.
        radioRange = simGetScriptSimulationParameter(sim_handle_main_script,'radioRange')
        distanceSourceSink = math.sqrt((sourcePosition[1] - sinkPosition[1])^2 + (sourcePosition[2] - sinkPosition[2])^2)
        dronesNeededForBridge = math.ceil(distanceSourceSink/radioRange - 1)
  
        -- Save the number of drones needed.
        --simAddStatusbarMessage('Saving number of drones needed')
        simSetScriptSimulationParameter(sim_handle_main_script, 'dronesNeededForBridge', tostring(dronesNeededForBridge), tostring(dronesNeededForBridge).len)
	
        --simAddStatusbarMessage('Distance source-sink: ' .. distanceSourceSink)
        if (myDroneName == printerName) then
          writeToFile('Drones required,' .. dronesNeededForBridge)
          writeToFile('Radio range,' .. radioRange .. '\n')
 
          writeToSummaryFile('Drones required,' .. dronesNeededForBridge)
          
          writeToFutureInputFile('Drones ,' .. numDrones)
          writeToFutureInputFile('Num of bridge locations,' .. dronesNeededForBridge)          
          writeToFutureInputFile('Drone range,' .. radioRange)
        end
        
        -- Calculate the locations for each drone to form the bridge.
        relayCoords = {}
        for i=1, dronesNeededForBridge, 1 do
            relayCoords[i] = {}
            relayCoords[i][1] = sinkPosition[1] + (sourcePosition[1] - sinkPosition[1])/(dronesNeededForBridge+1)*(i)
            relayCoords[i][2] = sinkPosition[2] + (sourcePosition[2] - sinkPosition[2])/(dronesNeededForBridge+1)*(i)
            --simAddStatusbarMessage('Location ' .. i .. ': ' .. relayCoords[i][1] ..','..relayCoords[i][2])
        end
        
        -- Calculate distances from all drones to each relay position.
        distanceTuples = {}
        for i=1, numDrones, 1 do
            droneName = ''
            if (i == 1) then
                droneName = 'Quadricopter#'
            else
                droneName = 'Quadricopter#' .. i-2
            end

            currentDroneHandle = simGetObjectHandle(droneName)
            currentDronePos = simGetObjectPosition(currentDroneHandle, -1)
            
            if (myDroneName == droneName) then
              originalPosition = currentDronePos
            end
            
            if (myDroneName == printerName) then
              writeToFile('Drone ' .. droneName .. ' location,' .. currentDronePos[1] ..', '..currentDronePos[2])
              end
            
            -- Ignore the drone that found the person, since it won't have to move; only calculate distances for the rest.            
            if(droneName ~= droneSourceName) then
                -- Go over each relay location.
                distancesString = ''
                for j=1,dronesNeededForBridge,1 do
                    currRelayCoords = relayCoords[j]
                    distanceToLocation = math.sqrt((currentDronePos[1] - currRelayCoords[1])^2 + (currentDronePos[2] - currRelayCoords[2])^2)
                    if (myDroneName == printerName) then
                      writeToFile('Distance from drone ' .. droneName .. ' to location ' .. j .. ',' .. currRelayCoords[1] ..', '..currRelayCoords[2] .. ' , ' .. distanceToLocation)
                      distancesString = distancesString .. distanceToLocation .. ','
                    end
                    
                    -- Store everything in a table.
                    distanceTuple = {droneName, j, distanceToLocation}
                    table.insert(distanceTuples,distanceTuple)  
                end
                
                if (myDroneName == printerName) then
                  writeToFutureInputFile('Distances for '.. droneName .. ',' .. distancesString)
                end
            end
        end	
        


        -- Sort the table with all the distances (from all drones to all relay locations) in ascending order by distance.
        --simAddStatusbarMessage("Sorting")
        table.sort(distanceTuples, compare)

        --simAddStatusbarMessage("Sorted values:")
        --for index, val in ipairs(distanceTuples) do
        --	simAddStatusbarMessage(index.." : "..val[1]..", loc: "..val[2]..", distance: "..val[3])
        --end

        totalDistance = 0
        distanceToBridge = 0

        -- Find the best drone-location pairs
        local usedDrones = {}
        local usedLocations = {}
        for index, val in ipairs(distanceTuples) do
            currDrone = val[1]
            currLocationIdx = val[2]
            currDistance = val[3]

            -- Ignore drones that have already been assigned to a location, and locations which already have drones assigned to them.
            if(not usedDrones[currDrone] and not usedLocations[currLocationIdx]) then
                -- Mark this drone and this location as assigned.
                usedDrones[currDrone] = true
                usedLocations[currLocationIdx] = true
                
                totalDistance = totalDistance + currDistance
                
                -- If I am the drone that has just been selected, the update my new target location.
                if (myDroneName == currDrone) then
                    -- Get the location coords to set as my future target.
                    myNewX = relayCoords[currLocationIdx][1]
                    myNewY = relayCoords[currLocationIdx][2]
                    
                    distanceToBridge = currDistance
                                        
                    --simAddStatusbarMessage(myDroneName)
                    --simAddStatusbarMessage('Bridge point    : ' .. myNewX .. ', ' .. myNewY)
                    --simAddStatusbarMessage('Distance to 1     : ' .. distanceToBridge)
                    
                    endTime = simGetSimulationTime()
                    elapsedTime = endTime - startTime
                    endSystemTime = simGetSystemTimeInMilliseconds()
                    elapsedSystemTime = endSystemTime - startSystemTime
                    
                    --simAddStatusbarMessage('Simulation      : ' .. elapsedTime .. ' s') 
                    --simAddStatusbarMessage('System (calc)   : ' .. elapsedSystemTime .. ' ms')

                    -- Overwrite the next and final destination variables to make the drone move to its brige location.
                    startToBridgeTime = simGetSystemTimeInMilliseconds()
                    
                    destinationx = myNewX
                    destinationy = myNewY

                    endx = myNewX
                    endy = myNewY

                    
                    --simSetObjectPosition(currentDroneHandle, -1, {myNewX, myNewY, 2})

                    -- Indicate that we will stop patrolling and start bridge-forming.
                    patrolling = false
                    bridging = true
                    
                    -- Stop the loop since we only care about where we have to go.
                    --break
                end
                
            end
        
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
                --simAddStatusbarMessage('Person found! ' .. tostring(simGetScriptSimulationParameter(sim_handle_main_script,'personFound')))
                --simAddStatusbarMessage('By ' ..suffix)

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
                
                if (bridging) then
                  dronesDone = simGetScriptSimulationParameter(sim_handle_main_script,'dronesDone')
                  dronesDone = dronesDone + 1
                  --simAddStatusbarMessage('Drone done!')
                  
                  endToBridgeTime = simGetSystemTimeInMilliseconds()
                  totalToBridgeTime = endToBridgeTime - startToBridgeTime
                  
                  writeToFile('\n' .. myDroneName)
                  writeToFile('Current location,' .. originalPosition[1] .. ', ' .. originalPosition[2])
                  writeToFile('Bridge point    ,' .. myNewX .. ', ' .. myNewY)
                  writeToFile('Distance to     ,' .. distanceToBridge)
                  
                  writeToFile('Time to solve   ,' .. elapsedSystemTime .. ',ms')    
                  writeToFile('Time to bridge  ,' .. totalToBridgeTime .. ',ms')
                  writeToSummaryFile(myDroneName .. ' time to bridge  ,' .. totalToBridgeTime .. ',ms')
                  
                  writeToFile('Total Distance to bridge,' .. totalDistance)
                  writeToSummaryFile('Total Distance to bridge,' .. totalDistance)
                  
                  simSetScriptSimulationParameter(sim_handle_main_script, 'dronesDone', tostring(dronesDone), 
                    tostring(dronesDone).len)
                end
                  
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
-- Helper function used to sort a table by the third value in each tuple.
function compare(a,b)
  return a[3] < b[3]
end
--/////////////////////////////////////////////////////////////////////////////////////////////

