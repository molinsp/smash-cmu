require('random_locations')

function doInitialSetup()
    -- set random location
    local handle = simGetObjectHandle('Bill')
    local index = simGetNameSuffix(nil) + 2;
    local position = g_bill_locs[index]
    simSetObjectPosition(handle, -1, position)
end

function cleanUp()
end
