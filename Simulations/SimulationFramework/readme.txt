######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################

This folder contains the code to simulate drones using the actual code
they will have and and plugins to simulate parts of it in V-Rep. The projects are:

 - DroneControllerSimulator
 
     This program acts as a simulated drone, containing all current code and logic for the drones
     except for the hardware layer. Instead of the actual hardware layer, it uses a V-Rep layer
     which gets information from the "sensors" (location) from V-Rep, and sends movement commands
     to V-Rep to, to move the simulated drones on a V-Rep scene. It uses the actual algorithms
     for Area Coverage and Bridge building that will be included on the drone. Since there is 
     an interface for the hardware layer, the use of V-Rep instead is (almost) transparent. 
     This is supposed to be used with the V-RepMadaraClientPlugin installed in the V-Rep main 
     executable folder, as they communicate with each other through Madara. It is assumed that a 
     V-Rep scene will act as the System Controller, as well as the sensors and actuators for each drone.
     
     The communication with V-Rep could be done through TCP or other methods, but it is actually
     handled through Madara as well, using some particular prefixes to get sensor information
     or send movement commands to V-Rep through Madara.
 
     Most of the code is actually located in the DroneController/packages/madara_client 
     folder or the repository, and is only referenced from this project.
     
     The project itself contains a simple main program that is very similar to the main loop
     in the DroneController project. The main differences are some simplifications for simulation
     purposes (and compatibility issues), and a small hack to enable the V-Rep layer to access
     Madara through the same Knowledge Base used to communicate between the drones.
     
     To execute, add the "-i" option, followed by an integer number starting from 0. 
     This indicates the id that this pseudo-drone will have. Running this in multiple
     consoles with different ids allows to simluate multiple pseudo-drones.
     
 - V-RepBridgeAlgorithmPlugin
 
    A simple plugin that calls the C++ version of the bridge algorithm directly. It does not use 
    Madara, and allows the V-Rep simulation to use the actual implementation of the algorithm locally.
    It defines only one custom Lua function that can be used by a simulation to get the position a
    drone should go to to form a bridge, if it is part of it.
    
    Notes:
    * This project has a custom property that has to be set to indicate where the V-Rep folder with
    its main executable is. It can be accessed through the Property Manager, opening the VRepEnvironment
    Property Page. Once there, in the User Macros section, the value of VRepPath has to be set to the
    absolute value on your system where the main executable of V-Rep is.
    * The property indicate above is used post-compilation to automatically move the generated DLL to 
    that folder, since VRep needs all plugins to be inside its executable folder.
    
 - V-RepMadaraClientPlugin

    A plugin that acts as the Controller of the network, as well as disseminating the positioning 
    information of the drones obtained from V-Rep, and receiving commands to move the drones to certain locations. 
    This is done through Madara to communicate with simulated pseudo-drones running with the DroneControllerSimulator 
    project described above. It defines several Lua functions that can be used by a simulation to setup 
    and cleanup the Madara interface, as well as send and receive the state of different variables in the Knowledge Base, 
    including area coverage and bridge requests, and functions to handle sensor and movement requests from the drones.
    
    Notes:
    * This project has a custom property that has to be set to indicate where the V-Rep folder with
    its main executable is. It can be accessed through the Property Manager, opening the VRepEnvironment
    Property Page. Once there, in the User Macros section, the value of VRepPath has to be set to the
    absolute value on your system where the main executable of V-Rep is.    
    * The property indicate above is used post-compilation to automatically move the generated DLL to 
    that folder, since VRep needs all plugins to be inside its executable folder.    

 - V-RepMadaraQuadrotorControlPlugin

    This plugin allows quadrotors in vrep to respond to go to commands issued
    from Madara. Functionality for land and takeoff commands will be added
    later.

##############################
# Linux notes
##############################

 * Makefile will recursively build projects
 * ensure REPO_ROOT is set correctly in Makefile
 * V-REP plugins are built in their respective directories, add symlinks or move
   these to VREP install directory to use
 * vrep needs to find libACE and libMADARA found when loading
   libv_repExtMadaraQuadrotorControlPlugin; symlink or copy them to vrep
   install directory
 * vrep needs to find LaptopController.lua, QuadrictoperTargetController.lua,
   and Utils.lua; symlink or copy them to vrep install directory
