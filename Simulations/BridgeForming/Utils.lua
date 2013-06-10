--####################################################################
--# Usage of this software requires acceptance of the SMASH-CMU License,
--# which can be found at the following URL:
--#
--# https://code.google.com/p/smash-cmu/wiki/License
--######################################################################

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Returns the position of the sink as a table with x,y,z
--/////////////////////////////////////////////////////////////////////////////////////////////
function getSinkInfo()
    -- Get position of sink.
    local sinkName = 'laptop'
    laptopHandle = simGetObjectHandle(sinkName .. '#')
    local sinkPosition = simGetObjectPosition(laptopHandle, -1)
    --simAddStatusbarMessage('Sink at '  .. sinkPosition[1] .. ', ' .. sinkPosition[2])
    
    return sinkName, sinkPosition
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
-- Returns the name and position (as a x,y,x table) of a drone with a given id (starting at 0).
--/////////////////////////////////////////////////////////////////////////////////////////////
function getDroneInfoFromId(id)
    local droneObjectName = 'Quadricopter#'

    -- For all drones but the first one (id 0), we have to add the suffix, which starts at 0 (id-1).
    if(id ~= 0) then
        droneObjectName = droneObjectName .. (id-1)
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

