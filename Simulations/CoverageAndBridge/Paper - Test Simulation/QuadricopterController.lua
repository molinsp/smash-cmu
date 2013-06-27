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

function doInitialSetup()

  -- Boolean for whether or not drone is part of problem
	inUse = true

  -- Get the drone name and suffix
  handle=simGetObjectHandle('Quadricopter')
  mySuffix = simGetNameSuffix(nil)
  if(mySuffix == -1) then
    myDroneName = 'Quadricopter#'
  else
    myDroneName = 'Quadricopter#' .. mySuffix
  end
  intSuffix = tonumber(mySuffix)

  --simAddStatusbarMessage('Initializing drone '..myDroneName)

  numDrones=simGetScriptSimulationParameter(sim_handle_main_script,'numberOfDrones')

  -- Now reset the manipulation sphere:
  targetObj=simGetObjectHandle('Quadricopter_target')
	d=simGetObjectHandle('Quadricopter_base')
	
  simSetObjectParent(targetObj,d,true)
  simSetObjectPosition(targetObj,sim_handle_parent,{0,0,0})
  simSetObjectOrientation(targetObj,sim_handle_parent,{0,0,0})

  -- Decide whether or not the drone is in use
  if (numDrones < intSuffix + 2) then
    --simAddStatusbarMessage('dont use '..mySuffix)
    inUse = false
  -- Jump to the middle of the search area
  else
  	x1=simGetScriptSimulationParameter(sim_handle_main_script,'x1')
    y1=simGetScriptSimulationParameter(sim_handle_main_script,'y1')
    x2=simGetScriptSimulationParameter(sim_handle_main_script,'x2')
    y2=simGetScriptSimulationParameter(sim_handle_main_script,'y2')
    
    jumpToX = (x2-x1)/2
    jumpToY = (y2-y1)/2
    jumpToZ = 1.4+0.4*(mySuffix+2)
    
    if(jumpToX < 0) then
      jumpToX = jumpToX * -1
    end
    if(jumpToY < 0) then
      jumpToY = jumpToY * -1
    end  
    --simAddStatusbarMessage('moving drone '..mySuffix)
    simSetObjectPosition(handle, -1, {jumpToX, jumpToY, jumpToZ})
  end

	-- Make sure we have version 2.4.13 or above (the particles are not supported otherwise)
	v=simGetIntegerParameter(sim_intparam_program_version)
	if (v<20413) then
		simDisplayDialog('Warning','The propeller model is only fully supported from V-REP version 2.4.13 and above.&&nThis simulation will not run as expected!',sim_dlgstyle_ok,false,'',nil,{0.8,0,0,0,0,0})
	end

	numPeople=simGetScriptSimulationParameter(sim_handle_main_script,'numberOfPeople')

	-- Detatch the manipulation sphere:
	simSetObjectParent(targetObj,-1,true)

	counter=1
	billCoords={}
	billposition={}
	for i=1,numPeople,1 do
		if(i==1) then
			personHandle=simGetObjectHandle('Bill#')
		else
			personHandle=simGetObjectHandle('Bill#' .. (i-2))
		end
			billposition=simGetObjectPosition(personHandle, -1)
			billCoords[counter]=billposition[1]
			billCoords[counter+1]=billposition[2]
			--simAddStatusbarMessage(counter .. ' : ' .. billCoords[counter] .. ', ' .. counter+1 .. ' : '..billCoords[counter+1])
			counter=counter+2
	end

	-- This control algo was quickly written and is dirty and not optimal. It just serves as a SIMPLE example

	particlesAreVisible=simGetScriptSimulationParameter(sim_handle_self,'particlesAreVisible')
	simSetScriptSimulationParameter(sim_handle_tree,'particlesAreVisible',tostring(particlesAreVisible))
	simulateParticles=simGetScriptSimulationParameter(sim_handle_self,'simulateParticles')
	simSetScriptSimulationParameter(sim_handle_tree,'simulateParticles',tostring(simulateParticles))

	propellerScripts={-1,-1,-1,-1}
	for i=1,4,1 do
		propellerScripts[i]=simGetScriptHandle('Quadricopter_propeller_respondable'..i)
	end
	heli=simGetObjectAssociatedWithScript(sim_handle_self)

	particlesTargetVelocities={0,0,0,0}

	pParam=2
	iParam=0
	dParam=0
	vParam=-2

	cumul=0
	lastE=0
	pAlphaE=0
	pBetaE=0
	psp2=0
	psp1=0

	prevEuler=0


	fakeShadow=simGetScriptSimulationParameter(sim_handle_self,'fakeShadow')
	if (fakeShadow) then
		shadowCont=simAddDrawingObject(sim_drawing_discpoints+sim_drawing_cyclic+sim_drawing_25percenttransparency+sim_drawing_50percenttransparency+sim_drawing_itemsizes,0.2,0,-1,1)
	end


	-- Prepare 2 floating views with the camera views:
	showCamera=simGetScriptSimulationParameter(sim_handle_self,'showCamera')
	if( showCamera ) then
		floorCam=simGetObjectHandle('Quadricopter_floorCamera')
		frontCam=simGetObjectHandle('Quadricopter_frontCamera')
		floorView=simFloatingViewAdd(0.9,0.9,0.2,0.2,0)
		frontView=simFloatingViewAdd(0.7,0.9,0.2,0.2,0)
		simAdjustView(floorView,floorCam,64)
		simAdjustView(frontView,frontCam,64)
	end

  return inUse
end

function runMainLogic()
  s=simGetObjectSizeFactor(d)

  pos=simGetObjectPosition(d,-1)
  if (fakeShadow) then
    itemData={pos[1],pos[2],0.002,0,0,1,0.2*s}
    simAddDrawingObjectItem(shadowCont,itemData)
  end

  -- Vertical control:
  targetPos=simGetObjectPosition(targetObj,-1)
  pos=simGetObjectPosition(d,-1)
  l=simGetVelocity(heli)
  e=(targetPos[3]-pos[3])
  cumul=cumul+e
  pv=pParam*e
  thrust=5.335+pv+iParam*cumul+dParam*(e-lastE)+l[3]*vParam
  lastE=e

  -- Horizontal control: 
  sp=simGetObjectPosition(targetObj,d)
  m=simGetObjectMatrix(d,-1)
  vx={1,0,0}
  vx=simMultiplyVector(m,vx)
  vy={0,1,0}
  vy=simMultiplyVector(m,vy)
  alphaE=(vy[3]-m[12])
  alphaCorr=0.25*alphaE+2.1*(alphaE-pAlphaE)
  betaE=(vx[3]-m[12])
  betaCorr=-0.25*betaE-2.1*(betaE-pBetaE)
  pAlphaE=alphaE
  pBetaE=betaE
  alphaCorr=alphaCorr+sp[2]*0.005+1*(sp[2]-psp2)
  betaCorr=betaCorr-sp[1]*0.005-1*(sp[1]-psp1)
  psp2=sp[2]
  psp1=sp[1]

  -- Rotational control:
  euler=simGetObjectOrientation(d,targetObj)
  rotCorr=euler[3]*0.1+2*(euler[3]-prevEuler)
  prevEuler=euler[3]

  -- Decide of the motor velocities:
  particlesTargetVelocities[1]=thrust*(1-alphaCorr+betaCorr+rotCorr)
  particlesTargetVelocities[2]=thrust*(1-alphaCorr-betaCorr-rotCorr)
  particlesTargetVelocities[3]=thrust*(1+alphaCorr-betaCorr+rotCorr)
  particlesTargetVelocities[4]=thrust*(1+alphaCorr+betaCorr-rotCorr)
    
    counter=1
    mypos=simGetObjectPosition(d, -1)
    for i=1,numPeople,1 do

    --simAddStatusbarMessage(mypos[1] .. ', ' .. mypos[2])
    if( (mypos[1] >= billCoords[counter]-0.2) and (mypos[1] <= billCoords[counter]+0.2) ) then
      if((mypos[2] >= billCoords[counter+1]-0.2) and (mypos[2] <= billCoords[counter+1]+0.2)) then
        --simAddStatusbarMessage('found person at: ' .. mypos[1] .. ', ' .. mypos[2] )
      end
    end
      counter=counter+2
    end

  -- Send the desired motor velocities to the 4 rotors:
  for i=1,4,1 do
    simSetScriptSimulationParameter(propellerScripts[i],'particleVelocity',particlesTargetVelocities[i])
  end

  if (simGetSimulationState()==sim_simulation_advancing_lastbeforestop) then
    -- Now reset the manipulation sphere:
    simSetObjectParent(targetObj,d,true)
    simSetObjectPosition(targetObj,sim_handle_parent,{0,0,0})
    simSetObjectOrientation(targetObj,sim_handle_parent,{0,0,0})
  end
end
