The SMASH project is composed of several distributed components. Some of the components are located on the drones (the **[Drone Controller](UnderstandingTheDroneController.md)** and its dependencies), and other components are located on mobile devices (such as the **AndroidSystemController**).

## [Drone Controller](UnderstandingTheDroneController.md) ##

The following diagram shows the main components that are installed on each drone.

![http://smash-cmu.googlecode.com/git/Architecture/DroneArchitecture.png](http://smash-cmu.googlecode.com/git/Architecture/DroneArchitecture.png)

  * **[Drone Controller](UnderstandingTheDroneController.md)**: this is the main entry point of the system inside each drone. It is in charge of handling all the logic necessary for the drone to follow instructions and react autonomously to the environment.
  * **[MADARA KaRLE](http://code.google.com/p/madara/)**: This knowledge engine supplies a distributed knowledge reasoning environment used by [Drone Controller](UnderstandingTheDroneController.md) to define its behavior and share information.
  * **ACE**: this is the network library used by the system to communicate between the different devices.
  * **Drone-RK 2 API**: this API is part of the Drone-RK project, developed by Carnegie Mellon. It gives access to different features of the drone, including its sensors, movement, etc.
  * **Parrot AR.Drone 2 Flight Controller**: this is a proprietary component provided by the Parrot company, which is in charge of actually allowing the drone to fly by controlling its motors.
  * **Drone-RK 2 RK Extensions**: this are resource kernel extensions, recompiled into the Linux kernel, which provide real-time scheduling features. They are also part of the Drone-RK project.

## System Controller ##
The System Controller is a component of the SMASH distributed system that is in charge of setting up common network parameters, sending commands to the Drone Controllers, and potentially monitoring the status of these drones as well. The SystemControllerSpecs page has details on the specifications for a System Controller.

Currently, the main System Controller is the AndroidSystemController. There are also some simpler CommandLineSystemControllers for test purposes (in the [CommandLineSystemController](https://code.google.com/p/smash-cmu/source/browse/#git%2FCommandLineSystemController) folder), and another V-Rep Plugin that integrates with a V-Rep simulation (see below for simulation details).

## Communication Between Components ##
All of the communication between the distributed components is done through Madara Transports associated to Madara Knowledge Bases. The diagram below shows a simple illustration of how each component sees the main Knowledge Base. Though in reality each component has a different copy of the Knowledge Base in their own host, the abstraction the components shold work with is that it is one central Knowledge Base.

![http://smash-cmu.googlecode.com/git/Architecture/SMASH-Dynamic.png](http://smash-cmu.googlecode.com/git/Architecture/SMASH-Dynamic.png)

It is important to indicate the setup used when running the system in simulation mode, without real drone hardware. This setup can be seen in the diagram below.

![http://smash-cmu.googlecode.com/git/Architecture/Simulation-Dynamic.png](http://smash-cmu.googlecode.com/git/Architecture/Simulation-Dynamic.png)

The main differences with the non-simulated communication are:
  * Several DroneControllers with simulated harware can be run on a single computer. For this reason, a standard Multicast Transport is used to keep the Knowledge Base consistent, instead of the hardware-dependent Drone-RK Transport.
  * For the hardware simulation purposes, a totally seprate Knowledge Base is used to communicate between the DroneControllers and the simulation of the hardware being performed by V-Rep. This Knowledge Base is totally independent on the SMASH one, and has different values in it.
  * There is also the possibility of using a simplified SystemController which is integrated in V-Rep to send the commands that correspond to a SystemController. However, this can also be ignored in favor of a real SystemController which is able to connect to the SMASH Knowledge Base through the transport used in simulation mode (Multicast, as mentioned above).