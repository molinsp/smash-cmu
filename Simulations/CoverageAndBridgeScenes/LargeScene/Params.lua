-- Environment parameters
g_nwLat = 200
g_nwLong = -200
g_seLat = 0
g_seLong = 0
g_numPeople = 20
g_numDrones = 20
g_minAltitude = 1.5

-- Floor consts and globals
g_floorDeltaLat = 10
g_floorDeltaLong = 10
g_sizeLat = g_nwLat - g_seLat
g_sizeLong = g_seLong - g_nwLong
g_columns = math.ceil(g_sizeLong / g_floorDeltaLong)
g_rows = math.ceil(g_sizeLat / g_floorDeltaLat)

-- Origin; currently at CMU
g_origin = {}
g_origin['latitude'] = 40.44319
g_origin['longitude'] =  -79.94033
