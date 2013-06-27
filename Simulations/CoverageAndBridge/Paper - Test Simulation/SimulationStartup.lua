--####################################################################
--# Usage of this software requires acceptance of the SMASH-CMU License,
--# which can be found at the following URL:
--#
--# https://code.google.com/p/smash-cmu/wiki/License
--######################################################################

function writeToFile(string)
  fileOutput = io.open("output.csv", "a")
	fileOutput:write(string .. "\n")
	fileOutput:flush ()
	fileOutput:close ()
end

function writeToSummaryFile(string)
  fileSummary = io.open("summary.csv", "a")
	fileSummary:write(string .. "\n")
	fileSummary:flush ()
	fileSummary:close ()
end

function writeToFutureInputFile(string)
  fileInput = io.open("optimalInput.csv", "a")
	fileInput:write(string .. "\n")
	fileInput:flush ()
	fileInput:close ()
end

function setupNewScenario(drones, range, x1, y1, x2, y2, billx, billy)
	
	numberOfSimulations = simGetScriptSimulationParameter(sim_handle_main_script,'numberOfSimulations')

	simAddStatusbarMessage('\nScenario #'..numberOfSimulations)
  writeToFile('\nScenario #'..numberOfSimulations)
  writeToSummaryFile('\nScenario #'..numberOfSimulations)
  writeToFutureInputFile('Scenario #'..numberOfSimulations..',Scenario #'..numberOfSimulations)
    
	--reset parameters for next scenario
	simSetScriptSimulationParameter(sim_handle_main_script, 'numberOfDrones', drones, drones.len)
	simSetScriptSimulationParameter(sim_handle_main_script, 'radioRange', range, range.len)
	simSetScriptSimulationParameter(sim_handle_main_script, 'x1', x1, x1.len)
	simSetScriptSimulationParameter(sim_handle_main_script, 'y1', y1, y1.len)
	simSetScriptSimulationParameter(sim_handle_main_script, 'x2', x2, x2.len)
	simSetScriptSimulationParameter(sim_handle_main_script, 'y2', y2, y2.len)
	
	billhandle = simGetObjectHandle('Bill#')
	simSetObjectPosition(billhandle, -1, {billx, billy, 1.2})
	
	laptophandle = simGetObjectHandle('laptop#')
	simSetObjectPosition(laptophandle, -1, {0.5, 0.5,  1.2})
end

function checkReset()

  dronesDone = simGetScriptSimulationParameter(sim_handle_main_script,'dronesDone')
  dronesNeededForBridge = simGetScriptSimulationParameter(sim_handle_main_script,'dronesNeededForBridge')
  
  if (dronesDone == dronesNeededForBridge) then
    resetStep = simGetScriptExecutionCount()
    resetStep = resetStep + 30
    --simAddStatusbarMessage('Setting up next scenario on step'..resetStep)
    simSetScriptSimulationParameter(sim_handle_main_script, 'resetStep', tostring(resetStep), tostring(resetStep).len)
    
     --always reset
    thatFound = '-2'
    simSetScriptSimulationParameter(sim_handle_main_script, 'droneThatFound', thatFound, thatFound.len)
	
    personFound = 'false'
    simSetScriptSimulationParameter(sim_handle_main_script, 'personFound', personFound, personFound.len)
    
    dronesNeededForBridge = -1
    simSetScriptSimulationParameter(sim_handle_main_script, 'dronesNeededForBridge', tostring(dronesNeededForBridge), tostring(dronesNeededForBridge).len)
	
    dronesDone = 0
    simSetScriptSimulationParameter(sim_handle_main_script, 'dronesDone', tostring(dronesDone), tostring(dronesDone).len)
	
  end

end

function setupNext()

  numberOfSimulations = simGetScriptSimulationParameter(sim_handle_main_script,'numberOfSimulations')

  --simAddStatusbarMessage('setup next number: ' .. numberOfSimulations)

  numsDrones = {'4', '8', '16', '24'}
  areaXandYs = {'10', '20'}
  
  billXs1stXandY = {'2', '9', '9', '1', '5'}
  billYs1stXandY = {'2', '9', '1', '9', '5'}  
  
  billXs2ndXandY = {'2', '19', '19', '1', '10'}
  billYs2ndXandY = {'2', '19', '1', '19', '10'}
  
	-- With 4 drones and 10 x 10.
  if (numberOfSimulations == 1) then 
		setupNewScenario('4','4','10','0','0','10', '9', '9')
	elseif (numberOfSimulations == 2) then 
		setupNewScenario('4','4','10','0','0','10', '9', '1')	
	elseif (numberOfSimulations == 3) then 
		setupNewScenario('4','4','10','0','0','10', '1', '9')	
	elseif (numberOfSimulations == 4) then 
		setupNewScenario('4','4','10','0','0','10', '5', '5')
		
	-- With 8 drones and 10 x 10.
  elseif (numberOfSimulations == 5) then 
		setupNewScenario('8','4','10','0','0','10', '9', '9')
	elseif (numberOfSimulations == 6) then 
		setupNewScenario('8','4','10','0','0','10', '9', '1')	
	elseif (numberOfSimulations == 7) then 
		setupNewScenario('8','4','10','0','0','10', '1', '9')	
	elseif (numberOfSimulations == 8) then 
		setupNewScenario('8','4','10','0','0','10', '5', '5')
		
	-- With 8 drones and 20 x 20.
  elseif (numberOfSimulations == 9) then 
		setupNewScenario('8','4','20','0','0','20', '19', '19')
	elseif (numberOfSimulations == 10) then 
		setupNewScenario('8','4','20','0','0','20', '19', '1')	
	elseif (numberOfSimulations == 11) then 
		setupNewScenario('8','4','20','0','0','20', '1', '19')	
	elseif (numberOfSimulations == 12) then 
		setupNewScenario('8','4','20','0','0','20', '10', '10')
		
	-- With 16 drones and 10 x 10.
  elseif (numberOfSimulations == 13) then 
		setupNewScenario('16','4','10','0','0','10', '9', '9')
	elseif (numberOfSimulations == 14) then 
		setupNewScenario('16','4','10','0','0','10', '9', '1')	
	elseif (numberOfSimulations == 15) then 
		setupNewScenario('16','4','10','0','0','10', '1', '9')	
	elseif (numberOfSimulations == 16) then 
		setupNewScenario('16','4','10','0','0','10', '5', '5')
		
	-- With 16 drones and 20 x 20.
  elseif (numberOfSimulations == 17) then 
		setupNewScenario('16','4','20','0','0','20', '19', '19')
	elseif (numberOfSimulations == 18) then 
		setupNewScenario('16','4','20','0','0','20', '19', '1')	
	elseif (numberOfSimulations == 19) then 
		setupNewScenario('16','4','20','0','0','20', '1', '19')	
	elseif (numberOfSimulations == 20) then 
		setupNewScenario('16','4','20','0','0','20', '10', '10')
	
	
	-- Finished Simulating.
	else
    simulationDone = 'true'
    simSetScriptSimulationParameter(sim_handle_main_script, 'simulationDone', simulationDone, simulationDone.len)
	end
	
	numberOfSimulations = numberOfSimulations + 1
	simSetScriptSimulationParameter(sim_handle_main_script, 'numberOfSimulations', 
    tostring(numberOfSimulations), tostring(numberOfSimulations).len)	
	
  simSetScriptSimulationParameter(sim_handle_main_script, 'resetStep', tostring(resetStep), tostring(resetStep).len)
	
end