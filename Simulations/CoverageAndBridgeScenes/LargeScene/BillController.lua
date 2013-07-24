require('Params')
require('BillLocations')

function setColor(objectTable, colorName, color)
    for i=1, #objectTable, 1 do
        if (simGetObjectType(objectTable[i]) == sim_object_shape_type) then
            simSetShapeColor(objectTable[i], colorName, 0, color)
        end
    end
end

g_hairColors = {4, {0.19, 0.15, 0.09}, {0.5, 0.5, 0.5}, {0.05, 0.05, 0.05}, {0.5, 0.45, 0.15}}
g_skinColors = {2, {0.41, 0.36, 0.29}, {0.35, 0.3, 0.23}}
g_shirtColors = {5, {0.18, 0.24, 0.36}, {0.36, 0.18, 0.18}, {0.21, 0.34, 0.22}, {0.31, 0.31, 0.31}, {0.12, 0.12, 0.12}}
g_trouserColors = {2, {0.27, 0.23, 0.13}, {0.08, 0.08, 0.08}}
g_shoeColors = {2, {0.08, 0.08, 0.08}, {0.17, 0.08, 0.03}}

function doInitialSetup()
    local modelHandle=simGetObjectAssociatedWithScript(sim_handle_self)

    -- determine index and create more if necessary
    local index = simGetNameSuffix(nil) + 1
    local handle = simGetObjectHandle('Bill')
    if(index == 0) then
        previousSelection = simGetObjectSelection()
        simRemoveObjectFromSelection(sim_handle_all, -1)
        simAddObjectToSelection(sim_handle_tree, modelHandle)
        for i = 2, g_numPeople do
            simCopyPasteSelectedObjects()
        end
    end

    -- set random colors
    -- Initialize to random colors
    -- First we just retrieve all objects in the model:
    previousSelection=simGetObjectSelection()
    simRemoveObjectFromSelection(sim_handle_all, -1)
    simAddObjectToSelection(sim_handle_tree, modelHandle)
    modelObjects=simGetObjectSelection()
    simRemoveObjectFromSelection(sim_handle_all, -1)
    simAddObjectToSelection(previousSelection)

    -- Now we set random colors:
    math.randomseed(simGetFloatingParameter(sim_floatparam_rand) * 10000)
    setColor(modelObjects, 'HAIR', g_hairColors[1 + math.random(g_hairColors[1])])
    setColor(modelObjects, 'SKIN', g_skinColors[1 + math.random(g_skinColors[1])])
    setColor(modelObjects, 'SHIRT', g_shirtColors[1 + math.random(g_shirtColors[1])])
    setColor(modelObjects, 'TROUSERS', g_trouserColors[1 + math.random(g_trouserColors[1])])
    setColor(modelObjects, 'SHOE',  g_shoeColors[1 + math.random(g_shoeColors[1])])

    -- set random location
    local index = simGetNameSuffix(nil) + 2;
    if(index ~= 1) then
        index = index - (g_rows * g_columns - 1) -- hack requires floors to be init first
    end
    if(index <= g_numPeople) then
        local position = g_billLocs[index]
        position[1] = position[1] - g_nwLong
        position[2] = position[2] - g_seLat
        simSetObjectPosition(handle,  -1,  position)
    end
end

function cleanUp()
    local modelHandle=simGetObjectAssociatedWithScript(sim_handle_self)

    -- Restore to initial colors:
    previousSelection=simGetObjectSelection()
    simRemoveObjectFromSelection(sim_handle_all, -1)
    simAddObjectToSelection(sim_handle_tree, modelHandle)
    modelObjects=simGetObjectSelection()
    simRemoveObjectFromSelection(sim_handle_all, -1)
    simAddObjectToSelection(previousSelection)
    setColor(modelObjects, 'HAIR', g_hairColors[2])
    setColor(modelObjects, 'SKIN', g_skinColors[2])
    setColor(modelObjects, 'SHIRT', g_shirtColors[2])
    setColor(modelObjects, 'TROUSERS', g_trouserColors[2])
    setColor(modelObjects, 'SHOE', g_shoeColors[2])
end
