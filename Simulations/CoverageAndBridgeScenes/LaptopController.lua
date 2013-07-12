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
    simAddStatusbarMessage('Calling external method to set up Madara.')
    simExtMadaraSystemControllerSetup(myControllerId, radioRange)

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
    
    simAddStatusbarMessage("Search area: " .. tostring(startInDegrees['longitude']) .. ',' .. startInDegrees['latitude'] .. '; ' .. endInDegrees['longitude'] .. ',' .. endInDegrees['latitude'])
    simExtMadaraSystemControllerSetupSearchArea(g_searchAreaId, tostring(startInDegrees['longitude']), tostring(startInDegrees['latitude']), 
                                                tostring(endInDegrees['longitude']), tostring(endInDegrees['latitude']))
    
    -- Tell drones to be part of this search area.
    addDronesToSearchArea(g_numDrones, g_searchAreaId)

    -- Used to identify each bridge request.
    g_bridgeRequestId = 0
    
    -- Array used to ensure that we automatically request a bridge for a certain person only once. Only useful to simplify the simulation.
    g_peopleFound = {}
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Adds drones to a search area, by requesting that out of each of them.
--/////////////////////////////////////////////////////////////////////////////////////////////
function addDronesToSearchArea(numDrones, areaId)
    for currDroneIdx = 1, numDrones, 1 do
        local currDroneId = currDroneIdx-1         -- Actual drone IDs start at 0, but Lua table indexes start at 1.       
        simExtMadaraSystemControllerSearchRequest(currDroneId, areaId)        
    end
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Method called in each step of the simulation.
--/////////////////////////////////////////////////////////////////////////////////////////////
function runMainLogic()
    -- NOTE: This is done here just for convinience of simulation. In reality, it would be issued by a rescuer at any moment, not when someone is found.        
    checkForBridgeRequest()
    
    -- Update the drone status to the network.        
    simExtMadaraSystemControllerUpdateStatus(g_numDrones)
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Checks if a bridge request is required, and sends it if it is the case.
--/////////////////////////////////////////////////////////////////////////////////////////////
function checkForBridgeRequest()
    -- Check if a person was found in this step.
    local sendNewBridgeRequest = false
    local personFoundId = simGetScriptSimulationParameter(sim_handle_main_script, 'personFoundId')
    if(personFoundId ~= -1) then
        -- We only need a bridge request if this person had not been found before.
        if(g_peopleFound[personFoundId] == nil) then
            sendNewBridgeRequest = true
        end
        
        -- Mark this person as found and clean the 'person found' flag.
        g_peopleFound[personFoundId] = true
        simSetScriptSimulationParameter(sim_handle_main_script, 'personFoundId', -1)
    end

    -- Send a bridge request if necessary.
    if(sendNewBridgeRequest) then
        -- Get sink and source info.
        local sinkName, sinkPosition = getSinkInfo()
        local sourceName, sourcePosition = getSourceInfo()
        
        -- Do the external bridge request.        
        simExtMadaraSystemControllerBridgeRequest(g_bridgeRequestId, 
                                                  tostring(sourcePosition[1]), tostring(sourcePosition[2]), tostring(sourcePosition[1]), tostring(sourcePosition[2]), 
                                                  tostring(sinkPosition[1]), tostring(sinkPosition[2]), tostring(sinkPosition[1]), tostring(sinkPosition[2]))
                                                           
        -- Update the next bridge request id.
        g_bridgeRequestId = g_bridgeRequestId + 1
    end
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Method called when the simulation ends.
--/////////////////////////////////////////////////////////////////////////////////////////////
function doCleanup()
    simExtMadaraSystemControllerCleanup()
end
