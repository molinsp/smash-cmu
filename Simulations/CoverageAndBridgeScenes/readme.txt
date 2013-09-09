######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################

- Scene Overview

This folder contains a V-Rep simulation scene, "area_coverage_and_bridge_building.ttt",
for area coverage and bridge-building. Area coverage refers to flying over every part 
of a certain region. Bridge building is a proposed secondary mode of operation for the 
drones, in which the drones will try to create a "bridge" of wireless connections so that 
a data stream (such as video) can be sent from a drone that found something of interest 
(source) to a person that wants to see it (sink).

Right now, the scene works in the following way: 
 * The simulation has to be started before anything else is done.
 * Once it is started, external drones configured to use V-Rep as thei hardware platform
   have to be started. This assumes that there will be external executables for each of the 
   simulated drones, which will communicate with the scene through an isolated knowledge base.
 * If the communicate is set up correctly, the drones will automatically take off and hover
   over the ground.
 * If the Start Search button is pressed on the System Controller, a command is sent to all 
   drones to start covering the search area (visually shown as the sandy square).
 * Each drone will assign itself a cell or quadrant in the search area to cover.
 * Each drone will go to the beginning of that cell, and start covering it with a pattern 
   dependent on the search algoritm parameter (see below).
 * When a drone finds a person (flies over it), it will show up in the mini map as a red square.
 * If auto-bridge mode is enabled, a bridge request will be sent automatically. Otherwise, 
   when the Form Bridge button is pressed, a bridge request will be sent for the last person found.
 * In both previous cases, when a bridge request is sent, the System Controller will send 
   a bridge request to all drones.
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
   to use this same simulation platform.  Note that this requires the v_repExtMadaraQuadrotorControlPlugin.dll 
   plugin.
 * System controller: the UI with the buttons simulates the System Controller, sending the requests when
   necessary. This is done through a common Madara knowledge base using multicast, very similar to the 
   knowledge base the drones would use to communicate among each other and a System Controller in reality.
   Note that this requires the v_repExtMadaraSystemController.dll plugin.
 
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

    There are some simulation parameters that have to match the physical objects in
    the scene. They have to be changed if these objects are changed. The parameters are:
     - numberOfDrones: the number of drones in the simulation
     - numberOfPeople: the number of people in the simulation
     - (x1,y1) to (x2,y2): bounding box of area being covered.

    The following are parameters that can be used to play with the simulation:
	 - minimumAltitude: height the drones have to reach before they start an area
	   coverage pattern. In reality, drones will choose different heights to avoid
	   crashing onto each other, but this will be the absolute minimum one.
	 - coverageAlgorithm: can be "random", "snake" or "inside_out", and determines
	   which algorithm will be used for the area coverage.
     - radioRange: the range (radius) of the Wi-Fi radio on the drones,
       required to calculate how many drones are needed for the bridge. 
     - autoBridgeRequest: used to automatically send a bridge request to the drones when
	   a person is found (basically equivalent to pressing the Form Bridge button
	   right after a person is found).
	 - searchLineWidth: the width of each search "line" a drone will follow, for the 
	   algorithms that follow lines (such as snake and inside_out). Determines how far
	   the drone would move to its side for the next "line". In degrees.
	 - searchHeightDiff: the amount of vertical distance to leave between drones searching
	   the same area, to avoid collisions. In meters.
	 - waitForRest: 1 means that after reaching each of its search targets, each drone will wait for the
	   rest to reach their corresponding target too. Only then it will start moving towards its next
	   target. 0 means move to your next target right after you get to your current one.
       
    Moving the locations of the laptop and the people on the scene can also be useful
    to check different behaviours.   

- NOTES

 * This simulation relies on several external .lua files which are present in this folder,
   the same one where the main scene file is. However, V-Rep needs to find these .lua files, 
   and will only look in the current folder or its internal folders. These external .lua files 
   will only be loaded correctly in one of two cases:
   > The simulation is started by double-clicking on the .ttt file, having this extension
   associated with V-Rep (in Windows), or
   > The .lua files are copied over to the main folder of V-Rep, or to its \lua subfolder.
