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
    local sinkPosition = getObjectPositionInDegrees(laptopHandle, -1)
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
    local dronePosition = getObjectPositionInDegrees(droneHandle, -1)    
    
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
    local dronePosition = getObjectPositionInDegrees(droneHandle, -1)    
    
    return droneObjectName, dronePosition    
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Helper function used to sort a table by the third value in each tuple.
--/////////////////////////////////////////////////////////////////////////////////////////////
function compare(a,b)
  return a[3] < b[3]
end

--/////////////////////////////////////////////////////////////////////////////////////////////
--
--/////////////////////////////////////////////////////////////////////////////////////////////
function asRadians(degrees)
    return degrees * math.pi / 180
end

--/////////////////////////////////////////////////////////////////////////////////////////////    
-- This reference point is chosen to get better latitudes. Now it is at CMU.
--/////////////////////////////////////////////////////////////////////////////////////////////
g_referencePoint = {}
g_referencePoint['latitude'] = 40.44319
g_referencePoint['longitude'] =  -79.94033

--/////////////////////////////////////////////////////////////////////////////////////////////    
-- Gets 
--/////////////////////////////////////////////////////////////////////////////////////////////
function getObjectPositionInDegrees(objectHandle, relativeTo)
    -- Get the cartesian position first.
    local vrepPosition = simGetObjectPosition(objectHandle, relativeTo)
    --simAddStatusbarMessage('Pos original: ' .. vrepPosition[1] .. ',' .. vrepPosition[2] .. ',' .. vrepPosition[3])
    local cartesianPosition = {}
    cartesianPosition['x'] = vrepPosition[1]
    cartesianPosition['y'] = vrepPosition[2]

    -- Get the long and lat now.
    local latAndLong = getLatAndLong(cartesianPosition)
    
    -- Return the new coordindates. Note that the height was already in meters, and will be still in meters.
    local newPosition = {latAndLong['longitude'], latAndLong['latitude'], vrepPosition[3]}
    return newPosition
end

--/////////////////////////////////////////////////////////////////////////////////////////////    
-- Gets 
--/////////////////////////////////////////////////////////////////////////////////////////////
function setObjectPositionFromDegrees(objectHandle, relativeTo, latAndLongPosition)
    -- Turn into a named table.
    local degreePosition = {}
    degreePosition['longitude'] = latAndLongPosition[1]
    degreePosition['latitude'] = latAndLongPosition[2]

    -- Get the cartesian position.
    local cartesianPosition = getXYpos(degreePosition)
    
    -- Return the new coordindates. Note that the height was already in meters.
    local vrepPosition = {cartesianPosition['x'], cartesianPosition['y'], latAndLongPosition[3]}
    simAddStatusbarMessage('Moving to pos in cart: ' .. vrepPosition[1] .. ',' .. vrepPosition[2] .. ',' .. vrepPosition[3])
    simSetObjectPosition(objectHandle, relativeTo, vrepPosition)
end    
    
--/////////////////////////////////////////////////////////////////////////////////////////////    
-- These values are based on the size of the Earth.
--/////////////////////////////////////////////////////////////////////////////////////////////
g_degressInCircumference = 360
g_earthPolesPerimeter = 40008000
g_earthEquatorialPerimter = 40075160    

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Calculates X and Y distances in meters.
--/////////////////////////////////////////////////////////////////////////////////////////////    
function getXYpos(point, relativeNullPoint)
    -- Get the default reference point if none was provided.
    relativeNullPoint = relativeNullPoint or g_referencePoint

    --simAddStatusbarMessage("Delta between: " .. point['latitude'] .. "," .. point['longitude'] .. ' and ' .. relativeNullPoint['latitude'] .. "," .. relativeNullPoint['longitude'])
    local deltaLatitude = point['latitude'] - relativeNullPoint['latitude']
    local deltaLongitude = point['longitude'] - relativeNullPoint['longitude']
    --simAddStatusbarMessage("Delta is: " .. deltaLatitude .. "," .. deltaLongitude)
    
    -- Actually do the calculation.
    local latitudePerimeter =  g_earthEquatorialPerimter * math.cos(asRadians(relativeNullPoint['latitude']))
    local resultX = latitudePerimeter * deltaLongitude / g_degressInCircumference
    local resultY = g_earthPolesPerimeter * deltaLatitude / g_degressInCircumference
    
    -- Turn this into a table.
    local result = {}
    result['x'] = resultX
    result['y'] = resultY

    return result
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Calculates latitude and longitude distances from meters.
--/////////////////////////////////////////////////////////////////////////////////////////////    
function getLatAndLong(point, relativeNullPoint)
    -- Get the default reference point if none was provided.
    relativeNullPoint = relativeNullPoint or g_referencePoint
    
    -- Get the deltas from reference point.
    local latitudePerimeter =  g_earthEquatorialPerimter * math.cos(asRadians(relativeNullPoint['latitude']))
    local deltaLongitude = point['x'] * g_degressInCircumference / latitudePerimeter
    local deltaLatitude = point['y'] * g_degressInCircumference / g_earthPolesPerimeter
    
    -- Get the lat and long.
    local latitude = deltaLatitude + relativeNullPoint['latitude']
    local longitude = deltaLongitude + relativeNullPoint['longitude']
    
    -- Turn this into a table.
    local result = {}
    result['latitude'] = latitude
    result['longitude'] = longitude

    return result
end 