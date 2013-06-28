--####################################################################
--# Usage of this software requires acceptance of the SMASH-CMU License,
--# which can be found at the following URL:
--#
--# https://code.google.com/p/smash-cmu/wiki/License
--######################################################################

-- Returns the name and position (as a x,y,x table) of a drone with a given suffix.
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

-- Returns the name and position (as a x,y,x table) of a drone with a given id (starting at 0).
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
