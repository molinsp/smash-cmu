######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################

- Scene Overview

This folder contains a V-Rep simulation scene, "coverage_and_bridge.ttt",
for area coverage and bridge-building. Area coverage refers to flying over every part 
of a certain region. Bridge building is a proposed secondary mode of operation for the 
drones, in which the drones will try to create a "bridge" of wireless connections so that 
a data stream (such as video) can be sent from a drone that found something of interest 
(source) to a person that wants to see it (sink).

Right now, the scene works in the following way: 
 * The simulation can be started at any point.
 * Once it is started, external drones configured to use V-Rep as their hardware platform
   have to be started. This assumes that there will be external executables for each of the 
   simulated drones, which will communicate with the scene through an isolated knowledge base.
 * The Setup Network button has to be used to send configuration parameters to the drones. If
   this is not pressed, drones will use whatever default values they have for their configuration
   (see Scene Simulation Parameters for a description of several of the parameters sent here).
 * The Take Off button has to be pressed to make the drones take off. If this works properly, the
   drones should move a bit up, up to an altitude of 1.5 m.
 * If the Start Search button is pressed on the System Controller, a command is sent to all 
   drones to start covering the search area (visually shown as the sandy square).
 * Each drone will assign itself a cell in the search area to cover (can be the whole area depending on the algorithm).
 * Each drone will go to the beginning of that cell, and start covering it with a pattern 
   dependent on the search algoritm parameter (see below).
 * When a drone finds a person (flies over it), it will show up in the mini map as a red square.
 * When the Form Bridge button is pressed, a bridge request will be sent for the last person found (if any).
 * Each drone will execute the same algorithm to calculate if it has to move to 
   create a bridge between the person and the laptop (sink). If it calculates it should
   be part of the bridge, it will move to the location it calculated and stop there, lowering its altitude.
 * All drones which do not become part of a bridge will continue covering their areas 
   until they reach the end of the cell, and then they will go back to covering their search areas again.
   
In its current implementation, the scene has no algorithms executed inside the scene. It has to distinct
functions:
 * Hardware platform: the scene simulates a hardware platform for each of the drones, including their sensors and
   actuators (moving them). This is shown visually with the moving drones and in the minimap.
   This communication is done by multicast through an isolated Madara knowledge base to drones configured
   to use this same simulation platform.  Note that this requires the v_repExtMadaraQuadcopterPlatformPlugin.dll 
   plugin.
 * System controller: the UI with the buttons simulates the System Controller, sending the requests when
   necessary. This is done through a common Madara knowledge base using multicast, very similar to the 
   knowledge base the drones would use to communicate among each other and a System Controller in reality.
   Note that this requires the v_repExtMadaraSystemSimpleController.dll plugin.
 
The plugins have to be inside the main folder of V-Rep before V-Rep is started to be loaded.
The source code for the plugins is in smash-cmu\Simulations\SimulationFramework.

- Bridge Building Algorithm   
   
    The current simulation attempts to create a line-bridge (which may not be the
    optimal way of creating a bridge) that minimizes the distances that the drones
    have to move, in order to reduce the amount of battery they spend to do this.
    One of its assumptions is that there are going to be enough drones 
    to create a bridge.

    The method it uses to select the best drones to build the bridge is to calculate all 
    distances from all drones to all locations, sorting them, and using the smaller 
    distances to chose which drone to send to each location. The complexity should be 
    O(n*m*log(n*m)), where n is the number of drones, and m the number of drones needed 
    for the bridge, so it can be O(n^2*log(n^2)) in the worst case.

- Scene Simulation Parameters

	The buttons in the SystemControllerGUI are:
	 - Setup Network: sends some basic parameters to all drones (see parameters below).
	 - Take Off: sends the command to all drones to lift from the ground and hover.
	 - Start Search: request all drones to search the area according to the search
	 parameters (see below).
	 - Form Brige: requests drones to form a bridge for the last person that has been found.
	 - Land: sends the command to all drones to land (get to a lower altitude, they won't actually
	 land since the floor is too uneven).

    There are some simulation parameters that have to match the physical objects in
    the scene. They have to be changed if these objects are changed. The parameters are:
     - numberOfDrones: the number of drones in the simulation
     - numberOfPeople: the number of people in the simulation
     - (x1,y1) to (x2,y2): bounding box of area being covered.

	There are also parameters used to translate between VRep cartesian model and a lat/long model: 
	 - referenceLat and referenceLong: the latitude and longitude (in degrees) of the reference point 
	   used to translate VRep x-y coordinates into lat,long coordinates.
	 
    The following are parameters that can be used to play with the simulation:
	
	Bridge Forming Parameters:
     - radioRange: the range (radius) of the Wi-Fi radio on the drones (in meters),
       required to calculate how many drones are needed for the bridge. 

	Area Coverage Search Parameters:
 	 - minimumAltitude: height (in meters) that the drones have to reach before they start an area
	   coverage pattern. In reality, drones will choose different heights to avoid
	   crashing onto each other, but this will be the absolute minimum one.
     - heightDiff: the amount of vertical distance (in meters) to leave between drones searching
	   the same area, to avoid collisions.
	 - coverageAlgorithm: can be "random", "snake" or "inside_out" (without quotes), and determines
	   which algorithm will be used for the area coverage.
	 - searchLineWidth: the width (in meters) of each search "line" a drone will follow, for the 
	   algorithms that follow lines (such as snake and inside_out). Determines how far
	   the drone would move to its side for the next "line".
	 - waitForRest: 1 means that after reaching each of its search targets, each drone will wait for the
	   rest to reach their corresponding target too. Only then it will start moving towards its next
	   target. 0 means move to your next target right after you get to your current one.
	 - humanDetectionAlgorithm: currently can only be "basic" (without quotes) to enable the basic
	   human detection. Can be set to 0 to disable human detection.
	 - coverageTracking: 1 to enable tracking % of area covered in each iteration of the main loop,
	   0 to disable.
	 - coverageTrackingFile: 1 to enable outputting the & of area covered in each iteration of the main
	   loop into a CSV file, 0 to disable.
	 - sensorAngle: angle (in degrees) of the sensor used to cover the area. Used to calculate the area
	   being seen at any moment by the drone.
    
    The "droneThatFound" and "personFoundName" parameters are used internally as communication between
	scripts and plugins in VRep.
	
    Moving the locations of the laptop and the people on the scene can also be useful
    to check different behaviours.   
