require('Utils')
require('IndependentParams')

-- Floor consts and globals
g_degreesInCircumference = 360
g_earthPolesPerimeter = 40008000
g_earthEquatorialPerimeter = 40075160    
g_latitudePerimeter = g_earthEquatorialPerimeter * math.cos(asRadians(g_seLat))
g_floorSideLengthMeters = 10
g_floorDeltaLat = g_floorSideLengthMeters / g_earthPolesPerimeter * g_degreesInCircumference
g_floorDeltaLong = g_floorSideLengthMeters / g_latitudePerimeter * g_degreesInCircumference
g_sizeLat = g_nwLat - g_seLat
g_sizeLong = g_seLong - g_nwLong
g_rows = math.ceil(g_sizeLat / g_floorDeltaLat)
g_columns = math.ceil(g_sizeLong / g_floorDeltaLong)
