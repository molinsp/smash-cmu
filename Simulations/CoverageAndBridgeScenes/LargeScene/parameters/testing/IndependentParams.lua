-- Environment parameters
g_nwLat = 40.443395
g_nwLong = -79.940784
g_seLat = 40.443044
g_seLong = -79.940044

-- SW corner of area; currently at CMU
g_origin = {}
g_origin['latitude'] = g_seLat
g_origin['longitude'] = g_nwLong

-- Bill Parameters
g_numPeople = 1

-- Drone Parameters
g_numDrones = 9
g_minAltitude = 1.5

-- Laptop and Drone parameters
g_radioRange = 0.0002
g_performBridge = false

-- Laptop parameters
g_laptopPosition = {}
g_laptopPosition['latitude'] = (3 * g_seLat + g_nwLat) / 4
g_laptopPosition['longitude'] = (3 * g_nwLong + g_seLong) / 4
g_laptopPosition['altitude'] = 0.0075
