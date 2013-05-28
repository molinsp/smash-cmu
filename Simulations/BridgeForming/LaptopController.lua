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
    
    -- Indicates if we are using the Madara client for communication with external "drones".
    g_madaraClientEnabled = simGetScriptSimulationParameter(sim_handle_main_script, 'madaraClientOn')      
    
    -- Setup Madara client.
    local myControllerId = 100
    if(g_madaraClientEnabled) then  
        local radioRange = simGetScriptSimulationParameter(sim_handle_main_script, 'radioRange')
        simAddStatusbarMessage('Calling external method to set up Madara.')
        simExtMadaraClientSetup(myControllerId, radioRange)
    end
    
    g_sinkPosition = getSinkPosition()

end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Method called in each step of the simulation.
--/////////////////////////////////////////////////////////////////////////////////////////////
function runMainLogic()
    -- If enabled, update the status in each step through Madara.
    if(g_madaraClientEnabled) then  
        droneIds, dronePositions, droneFlyingStatus = getDronesInfo(g_numDrones)
        
        --for key, value in pairs(droneIds) do
        --    simAddStatusbarMessage('Drone index ' .. key .. ' id ' .. value)
        --end

        --for key, value in pairs(dronePositions) do
        --    simAddStatusbarMessage('Drone index ' .. key .. ' position ' .. value)
        --end

        --for key, value in pairs(droneFlyingStatus) do
        --    simAddStatusbarMessage('Drone index ' .. key .. ' flying ' .. tostring(value))
        --end       
        
        simExtMadaraClientUpdateStatus(g_sinkPosition[1], g_sinkPosition[2], droneIds, dronePositions, droneFlyingStatus)    
    end
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Returns the ids, positions and flying status of all drones.
--/////////////////////////////////////////////////////////////////////////////////////////////
function getDronesInfo(numDrones)
    -- Get all drone positions
    local droneIds = {}
    local dronePositionsArray = {}
    local droneFlyingStatus = {}
    for currDroneIdx = 1, numDrones, 1 do
        local currDroneId = currDroneIdx-1         -- Actual drone IDs start at 0, but Lua table indexes start at 1.
        local curentDroneName, currentDronePos = getDroneInfoFromId(currDroneId)
        
        -- We have to store the IDs in a separate, simple table to be able to pass this to the C function in the plugin (which doesnt accept nested tables).
        droneIds[currDroneIdx] = currDroneId
        droneFlyingStatus[currDroneIdx] = true
        
        -- We have to store the positions in a single-level array to pass this to the C function in the plugin (which doesnt accept nested tables).
        dronePositionsArray[2*currDroneIdx-1] = currentDronePos[1]
        dronePositionsArray[2*currDroneIdx-1+1] = currentDronePos[2]
    end
    
    return droneIds, dronePositionsArray, droneFlyingStatus
end



