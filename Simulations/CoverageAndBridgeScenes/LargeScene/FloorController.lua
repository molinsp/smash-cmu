require('Params')

-- move floor to desired area
function doInitialSetup()
    -- determine index in floor and create more if necessary
    local index = simGetNameSuffix(nil) + 1
    local handle = {}
    if(index == 0) then
        handle = simGetObjectHandle('Plane')
        previousSelection = simGetObjectSelection()
        simRemoveObjectFromSelection(sim_handle_all, -1)
        simAddObjectToSelection(sim_handle_tree, handle)
        for i = 1, g_rows * g_columns - 1 do
            simCopyPasteSelectedObjects()
        end
    end

    --simAddStatusbarMessage('index: '..index)
    if(index < g_rows * g_columns) then
        -- get handle
        local handle = simGetObjectHandle('Plane')

        -- set position
        local x = index % g_columns
        local y = math.floor(index / g_columns)
        local position = {}
        position[1] = g_floorSideLengthMeters / 2 + x * g_floorSideLengthMeters
        position[2] = g_floorSideLengthMeters / 2 + y * g_floorSideLengthMeters
        position[3] = 0
        --simAddStatusbarMessage('index: '..index..' x: '..x..' rows: '..g_rows..' y: '..y..' cols: '..g_columns..' posX: '..position[1]..' posY: '..position[2])
        simSetObjectPosition(handle, -1, position)

        -- create checkerboard pattern
        local color = {}
        if((x + y) % 2 == 0) then
            color[1] = 0.3
            color[2] = 0.3
            color[3] = 0.3
        else
            color[1] = 0.5
            color[2] = 0.5
            color[3] = 0.5
        end
        simSetShapeColor(handle, nil, 0, color)
    end
end

function cleanUp()
end
