--####################################################################
--# Usage of this software requires acceptance of the SMASH-CMU License,
--# which can be found at the following URL:
--#
--# https://code.google.com/p/smash-cmu/wiki/License
--######################################################################

require('Params')
require('Utils')

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Method called when the simulation starts.
--/////////////////////////////////////////////////////////////////////////////////////////////
function doInitialSetup()
    -- Setup Madara for communications.
    local myControllerId = 2000    
    local radioRange = g_radioRange
    local minAltitude = g_minAltitude
    simAddStatusbarMessage('Calling external method to set up Madara.')
    simExtMadaraSystemControllerSetup(myControllerId, radioRange, minAltitude)

    -- Set up the search area, getting the boundaries from the parameters.
    g_searchAreaId = 0
    seInDegrees = {}
    seInDegrees['latitude'] = g_seLat
    seInDegrees['longitude'] = g_seLong
    nwInDegrees = {}
    nwInDegrees['latitude'] = g_nwLat
    nwInDegrees['longitude'] = g_nwLong
    
    simAddStatusbarMessage("Search area: SE " ..
        tostring(seInDegrees['latitude']) .. ',' .. tostring(seInDegrees['longitude']) .. "; NW " ..
        tostring(nwInDegrees['latitude']) .. ',' .. tostring(nwInDegrees['longitude']))
    simExtMadaraSystemControllerSetupSearchArea(g_searchAreaId,
        tostring(nwInDegrees['longitude']), tostring(nwInDegrees['latitude']),
        tostring(seInDegrees['longitude']), tostring(seInDegrees['latitude']))

    -- Array used to ensure that we automatically request a bridge for a certain person only once
    -- Only useful to simplify the simulation.
    g_peopleFound = {}

    -- Used to identify each bridge request.
    g_bridgeRequestId = 0

    -- Tell drones to be part of this search area.
    addDronesToSearchArea(g_numDrones, g_searchAreaId)
   
    -- Move to initial position
    local position = getXYpos(g_laptopPosition, g_origin)
    local handle = simGetObjectHandle('laptop')
    position[1] = position['x']
    position[2] = position['y']
    position[3] = g_laptopPosition['altitude']
    simSetObjectPosition(handle, -1, position)
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Adds drones to a search area, by requesting that out of each of them.
--/////////////////////////////////////////////////////////////////////////////////////////////
function addDronesToSearchArea(numDrones, areaId)
    for currDroneIdx = 1, g_numDrones, 1 do
        local currDroneId = currDroneIdx-1         -- Actual drone IDs start at 0, but Lua table indexes start at 1.       
        simExtMadaraSystemControllerAreaCoverageRequest(currDroneId, areaId, 'inside_out')
    end
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Method called in each step of the simulation.
--/////////////////////////////////////////////////////////////////////////////////////////////
function runMainLogic()
    -- NOTE: This is done here just for convenience of simulation. In reality, it would be issued by a rescuer at any moment, not when someone is found.        
    if(g_performBridge ~= nil and g_performBridge == true) then
        checkForBridgeRequest()
    end
    
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
        
        simAddStatusbarMessage('Bridge req: ' ..
            '(' .. tostring(sourcePosition[1]) ..','.. tostring(sourcePosition[2]) ..')' ..
            '('..    tostring(sinkPosition[1]) ..','.. tostring(sinkPosition[2]) .. ')')
        
        -- Do the external bridge request.        
        simExtMadaraSystemControllerBridgeRequest(g_bridgeRequestId,
            tostring(sourcePosition[1]), tostring(sourcePosition[2]),
            tostring(sourcePosition[1]), tostring(sourcePosition[2]), 
            tostring(sinkPosition[1]), tostring(sinkPosition[2]),
            tostring(sinkPosition[1]), tostring(sinkPosition[2]))
                                                           
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
