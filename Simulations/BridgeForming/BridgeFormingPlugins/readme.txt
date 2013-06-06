######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################

This folder contains the code for the simple Bridge Algorithm, as well as testers
for that code, and plugins to use it in V-Rep. The projects are:

 - BridgeDroneSimulator
 
     References the actual Bride Algorithm code and an object to handle access to the 
     algorithm through Madara logic which coordinates with the rest of the distributed 
     system. This code is actually located in the DroneController/packages/madara_client 
     folder or the repository.
     
     Only contains a simple main program that runs a loop to test out the bridge 
     functionality, assuming the V-Rep simulator does the rest (act as a controller and 
     disseminate position information). This is supposed to be used with the V-RepMadaraClientPlugin
     installed in the V-Rep main executable folder, as they communicate with each other
     through Madara.
     
     To execute, add the "-i" option, followed by an integer number starting from 0. 
     This indicates the id that this pseudo-drone will have. Running this in multiple
     consoles with different ids allows to simluate multiple pseudo-drones.
     
     The main program can also be run independently of V-Rep with the "-t" option to 
     populate the knowledge base with a predefined test state of drone positions and
     a bridge request, to see the interaction of multiple drones in different consoles
     and the results of the bridge algorithm.
     
 - V-RepBridgeAlgorithmPlugin
 
    A simple plugin that calls the C++ version of the algorithm directly. It does not use 
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
    information of the drones obtained from V-Rep. This is done through Madara to communicate 
    with simulated pseudo-drones running with the BridgeDroneSimulator project described above.
    It defines several Lua functions that can be used by a simulation to setup and cleanup the Madara
    interface, as well as send and receive the state of different variables in the Knowledge Base, 
    including bridge requests and bridge target positions for a drone.
    
    Notes:
    * This project has a custom property that has to be set to indicate where the V-Rep folder with
    its main executable is. It can be accessed through the Property Manager, opening the VRepEnvironment
    Property Page. Once there, in the User Macros section, the value of VRepPath has to be set to the
    absolute value on your system where the main executable of V-Rep is.    
    * The property indicate above is used post-compilation to automatically move the generated DLL to 
    that folder, since VRep needs all plugins to be inside its executable folder.    