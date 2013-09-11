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
    -- Get the total number of drones.
	g_numDrones = simGetScriptSimulationParameter(sim_handle_main_script, 'numberOfDrones')

    -- Setup Madara for communications.
    local myControllerId = 2000    
    local radioRange = simGetScriptSimulationParameter(sim_handle_main_script, 'radioRange')
    local minAltitude = simGetScriptSimulationParameter(sim_handle_main_script, 'minimumAltitude')
	local lineWidth = simGetScriptSimulationParameter(sim_handle_main_script, 'searchLineWidth')
	local heightDiff = simGetScriptSimulationParameter(sim_handle_main_script, 'searchHeightDiff')
    simAddStatusbarMessage('Calling external method to set up Madara.')
    simExtMadaraSystemControllerSetup(myControllerId, radioRange, minAltitude, lineWidth, heightDiff)

    -- Used to identify each bridge request.
    g_bridgeRequestId = 0
    
    -- Array used to ensure that we automatically request a bridge for a certain person only once. Only useful to simplify the simulation.
    g_peopleFound = {}
	
	-- To be used for bridge requests.
	loadPeoplePositions()
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Method called in each step of the simulation.
--/////////////////////////////////////////////////////////////////////////////////////////////
function runMainLogic()
    -- Check for user input.
    checkForButtonPress()

    -- NOTE: This is done here just for convenience of simulation. In reality, it would be issued by a rescuer at any moment, not when someone is found.        
    checkForBridgeRequest()
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Check for button presses.
--/////////////////////////////////////////////////////////////////////////////////////////////
function checkForButtonPress()
    local commandsUIHandle = simGetUIHandle('commandsUI')
    buttonHandle, eventDetails = simGetUIEventButton(commandsUIHandle)
    
    -- If the button handle is valid and the second event detail is 1, it means a button was pressed.
    if(not (buttonHandle == -1) and (eventDetails[2] == 0)) then
        local buttonLabel = simGetUIButtonLabel(commandsUIHandle, buttonHandle)
        --simAddStatusbarMessage('Button pressed and released! ' .. buttonLabel)
        
        -- Send network-wide parameters (radio range, num drones, min height).
        if(buttonLabel == 'Setup Network') then
			simExtMadaraSystemControllerUpdateStatus(g_numDrones)
        end		
		
        -- Start a search request if that button was pressed.
        if(buttonLabel == 'Start Search') then
            sendSearchRequest()
        end
		
        -- Start a bridge request if that button was pressed.
        if(buttonLabel == 'Form Bridge') then
            sendBridgeRequestForLastPersonFound()
        end
		
    end
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Sets up the search area and the drones to be in it through the network.
--/////////////////////////////////////////////////////////////////////////////////////////////
function sendSearchRequest()
	-- Setup the search area through the network.
	setupSearchArea()
    
    -- Tell drones to be part of this search area.
    addDronesToSearchArea(g_numDrones, g_searchAreaId)
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Sets up the search area for the whole network.
--/////////////////////////////////////////////////////////////////////////////////////////////
function setupSearchArea()
    -- Set up the search area, getting the boundaries from the parameters.
    g_searchAreaId = 0
    local x1 = simGetScriptSimulationParameter(sim_handle_main_script, 'x1')
    local y1 = simGetScriptSimulationParameter(sim_handle_main_script, 'y1')
    local x2 = simGetScriptSimulationParameter(sim_handle_main_script, 'x2')
    local y2 = simGetScriptSimulationParameter(sim_handle_main_script, 'y2')   

    -- Turn into degrees.
    local startPoint = {}
    startPoint['x'] = x1
    startPoint['y'] = y1
    local startInDegrees = getLatAndLong(startPoint)
    local endPoint = {}
    endPoint['x'] = x2
    endPoint['y'] = y2    
    local endInDegrees = getLatAndLong(endPoint)
    
	-- Send command through network.
    simAddStatusbarMessage("Search area: " .. tostring(startInDegrees['longitude']) .. ',' .. startInDegrees['latitude'] .. '; ' .. endInDegrees['longitude'] .. ',' .. endInDegrees['latitude'])
    simExtMadaraSystemControllerSetupSearchArea(g_searchAreaId, tostring(startInDegrees['longitude']), tostring(startInDegrees['latitude']), 
                                                tostring(endInDegrees['longitude']), tostring(endInDegrees['latitude']))
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Adds drones to a search area, by requesting that out of each of them.
--/////////////////////////////////////////////////////////////////////////////////////////////
function addDronesToSearchArea(numDrones, areaId)
	-- Get configurable parameters for the search.
	local coverageAlgorithm = simGetScriptSimulationParameter(sim_handle_main_script, 'coverageAlgorithm')
	local humanDetectionAlgorithm = simGetScriptSimulationParameter(sim_handle_main_script, 'humanDetectionAlgorithm')
	local waitForRest = simGetScriptSimulationParameter(sim_handle_main_script, 'waitForRest')	
    
    local droneIdsString = ''
    local firstInList = true
    for currDroneIdx = 1, numDrones, 1 do
        local currDroneId = currDroneIdx-1         -- Actual drone IDs start at 0, but Lua table indexes start at 1.               
        
        -- Add a comma if appropriate, and concatenate with the next drone id.
        if(firstInList) then
            firstInList = false
        else
            droneIdsString = droneIdsString .. ','
        end
        droneIdsString = droneIdsString .. currDroneId
    end
    simAddStatusbarMessage("Drone ids string: " .. droneIdsString)
    
    -- Ask Madara to send the search request.
    simExtMadaraSystemControllerAreaCoverageRequest(droneIdsString, areaId, coverageAlgorithm, waitForRest, humanDetectionAlgorithm)        
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Checks if a bridge request is required, and sends it if it is the case.
--/////////////////////////////////////////////////////////////////////////////////////////////
function checkForBridgeRequest()
	
	-- Only do check if there is a person if the auto bridge request is on.
	local autoBridgeRequest = simGetScriptSimulationParameter(sim_handle_main_script, 'autoBridgeRequest')
	if(autoBridgeRequest == 1) then	
		-- Check if a person was found in this step.	
		local personFoundId = simGetScriptSimulationParameter(sim_handle_main_script, 'personFoundId')
		if(personFoundId ~= -1) then
			-- We only need a bridge request if this person had not been found before.
			if(g_peopleFound[personFoundId] == nil) then
				sendBridgeRequestForLastPersonFound()
				local sourceSuffix = simGetScriptSimulationParameter(sim_handle_main_script, 'droneThatFound')
				simAddStatusbarMessage('Drone with suffix ' .. sourceSuffix .. ' found person ' .. personFoundId .. '!')
			end
			
			-- Mark this person as found.
			g_peopleFound[personFoundId] = true
		end
	end
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Sends a bridge request to the network.
--/////////////////////////////////////////////////////////////////////////////////////////////
function sendBridgeRequestForLastPersonFound()
	-- Only do this if at least one person has been found.
    local personFoundId = simGetScriptSimulationParameter(sim_handle_main_script, 'personFoundId')
	if(personFoundId ~= -1) then
		-- Get sink and source info.
		local sinkPosition = getSinkPosition()
		local sourcePosition = getPersonPosition(personFoundId)
		
		-- Do the external bridge request.
		simAddStatusbarMessage('Sending bridge request for last person found.')
		simExtMadaraSystemControllerBridgeRequest(g_bridgeRequestId, 
												  tostring(sourcePosition[1]), tostring(sourcePosition[2]), tostring(sourcePosition[1]), tostring(sourcePosition[2]), 
												  tostring(sinkPosition[1]), tostring(sinkPosition[2]), tostring(sinkPosition[1]), tostring(sinkPosition[2]))
														   
		-- Update the next bridge request id.
		g_bridgeRequestId = g_bridgeRequestId + 1
	else
		simAddStatusbarMessage('No person found yet!')
	end
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Returns the position of the sink as a table with x,y,z
--/////////////////////////////////////////////////////////////////////////////////////////////
function getSinkPosition()
    -- Get position of sink.
    local sinkName = 'laptop'
    laptopHandle = simGetObjectHandle(sinkName .. '#')
    local sinkPosition = getObjectPositionInDegrees(laptopHandle, -1)
    --simAddStatusbarMessage('Sink at '  .. sinkPosition[1] .. ', ' .. sinkPosition[2])
    
    return sinkPosition
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Returns the position of the source person found as a table with x,y,z.
--/////////////////////////////////////////////////////////////////////////////////////////////
function getPersonPosition(personFoundId)
	local personPosition = {g_personCoordsX[personFoundId], g_personCoordsY[personFoundId], 0}
	return personPosition
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Method called when the simulation ends.
--/////////////////////////////////////////////////////////////////////////////////////////////
function doCleanup()
    simExtMadaraSystemControllerCleanup()
end
