require('Utils')

-- Environment parameters
g_nwLat = 40.444161
g_nwLong = -79.941011
g_seLat = 40.442497
g_seLong = -79.938853

-- SW corner of area; currently at CMU
g_origin = {}
g_origin['latitude'] = g_seLat
g_origin['longitude'] = g_nwLong

-- Bill Parameters
g_numPeople = 20

-- Drone Parameters
g_numDrones = 20
g_minAltitude = 1.5

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

-- Laptop parameters
g_laptopPosition = {}
g_laptopPosition['latitude'] = (3 * g_seLat + g_nwLat) / 4
g_laptopPosition['longitude'] = (3 * g_nwLong + g_seLong) / 4
g_laptopPosition['altitude'] = 0.0075

-- Laptop and Drone parameters
g_radioRange = 4.0
