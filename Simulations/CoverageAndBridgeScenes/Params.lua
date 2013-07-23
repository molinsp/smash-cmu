-- Environment parameters
g_nwLat = 200
g_nwLong = -200
g_seLat = 0
g_seLong = 0
g_numBills = 15

-- Floor consts and globals
g_floorDeltaLat = 10
g_floorDeltaLong = 10
g_sizeLat = g_nwLat - g_seLat
g_sizeLong = g_seLong - g_nwLong
g_columns = math.ceil(g_sizeLong / g_floorDeltaLong)
g_rows = math.ceil(g_sizeLat / g_floorDeltaLat)
