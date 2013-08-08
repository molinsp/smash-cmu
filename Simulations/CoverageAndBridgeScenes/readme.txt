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
 * The laptop, which acts as the System Controller, tells all drone to start
   covering the search area (visually shown as the sandy square on the ground).
 * Each drone will assign itself a cell or quadrant in the search area to cover.
 * Each drone will go to the beggining of that cell, and start zig-zagging to cover
   all that cell.
 * When a drone finds a person (flies over it), it will notify the laptop (System Controller).
 * Immediately, the System Controller will send a bridge request to all drones.
   In reality this would be triggerd by the laptop user, not automatically.
 * Each drone will execute the same algoritghm to calculate if it has to move to 
   create a bridge between the person and the laptop (sink). If it is supposed
   to be part of the bridge, it will move to the location it calculated and stop there.
 * All drones which do not become part of a bridge will continue covering their areas 
   untill they reach the end of the cell, and then they will stop.
   
In its current implementation, the scene will only act as the System Controller and as the eyes and actuators 
of the drones, with no algorithms executed inside the scene. The scene will continously 
disseminate the position of the simulated drones through Madara, and send area coverage 
and bridge requests when required automatically (as well as handling the results to order the drones 
to move to a certain location). Note that it requires the v_repExtMadaraSystemController.dll 
and v_repExtMadaraQuadrotorControlPlugin.dll (which have to be in the main folder of V-Rep before 
V-Rep is started to be loaded) to access the Madara client. This assumes that there will be external 
executables for each of the simulated drones, which will communicate with the scene through Madara.
The source code for the plugins is in smash-cmu\Simulations\SimulationFramework.

- Bridge Building Algorithm   
   
    The current simulation attemps to create a line-bridge (which may not be the
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
     - radioRange: the range (radius) of the Wi-Fi radio on the drones,
       required to calculate how many drones are needed for the bridge.  
	 - coverageAlgorithm: can be "random", "snake" or "inside_out", and determines
	   which algorithm will be used for the area coverage.
       
    Moving the locations of the laptop and the people on the scene can also be useful
    to check different behaviors.   

- NOTES

 * This simulation relies on several external .lua files which are present in this folder,
   the same one where the main scene file is. However, V-Rep needs to find these .lua files, 
   and will only look in the current folder or its internal folders. These external .lua files 
   will only be loaded correctly in one of two cases:
   > The simulation is started by double-clicking on the .ttt file, having this extension
   associated with V-Rep (in Windows), or
   > The .lua files are copied over to the main folder of V-Rep, or to its \lua subfolder.
