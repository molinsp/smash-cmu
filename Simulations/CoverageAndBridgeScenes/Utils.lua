--####################################################################
--# Usage of this software requires acceptance of the SMASH-CMU License,
--# which can be found at the following URL:
--#
--# https://code.google.com/p/smash-cmu/wiki/License
--######################################################################

-- This reference point is chosen to get better latitudes. Now it is at CMU.
g_referencePoint = {}
g_referencePoint['latitude'] = simGetScriptSimulationParameter(sim_handle_main_script, 'referenceLat')
g_referencePoint['longitude'] = simGetScriptSimulationParameter(sim_handle_main_script, 'referenceLong')

-- Real Earth measurements are required to transform between cartesian and lat/long positions.
g_degressInCircumference = 360
g_earthPolesPerimeter = 40008000
g_earthEquatorialPerimter = 40075160    

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Returns the position of the source person found as a table with x,y,z.
--/////////////////////////////////////////////////////////////////////////////////////////////
function getPersonPositionInDegrees(personFoundName)
	personHandle = simGetObjectHandle(personFoundName)
	local personPosition = getObjectPositionInDegrees(personHandle, -1)
	return personPosition
end

--/////////////////////////////////////////////////////////////////////////////////////////////
--
--/////////////////////////////////////////////////////////////////////////////////////////////
function asRadians(degrees)
    return degrees * math.pi / 180
end

--/////////////////////////////////////////////////////////////////////////////////////////////    
-- Returns the position of a given object in degrees.
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
    
    -- Return the new coordinates. Note that the height was already in meters, and will be still in meters.
    local newPosition = {}
	newPosition['latitude'] = latAndLong['latitude']
	newPosition['longitude'] = latAndLong['longitude']
	newPosition['altitude'] = vrepPosition[3]
    return newPosition
end

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