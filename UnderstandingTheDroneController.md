# Introduction #

The SMASH project strives to create autonomous swarms of general-purpose vehicles and devices which work with a human operator to accomplish mission objectives in a real-time networked system. On this wiki page, we describe the C++ controller that interfaces with the [MADARA](http://madara.googlecode.com) middleware to accomplish distributed, decentralized control of a swarm.

# Overview #

<img src='http://smash-cmu.googlecode.com/files/ControlLoop.png' alt='Image overview of Drone Controller'>

<h1>Phase 1: Sense</h1>

Each cycle of the control loop starts with sensing the environment around the agent. Information is gathered from the network (MADARA facilitated), thermals, GPS, acoustics, ultrasound, LiDAR, or whatever sensors are available and digested into a format that can be processed.<br>
<br>
<h1>Phase 2: Process</h1>

The results of sensing the environment is then processed by the agent to inform it of its current state. The mission, objectives, and role of the agent are applied to the results of the sensing, and action or inaction is primed.<br>
<br>
<h1>Phase 3: Act</h1>

The results of the processing of the environmental state is applied to the abilities and intent of the agent to act appropriately in the environment, in support of whatever objectives the swarm or agent are involved in.<br>
<br>
<h1>Location in Repository</h1>

The Drone Controller's entry point is located in the <a href='https://code.google.com/p/smash-cmu/source/browse/#git%2FDroneController'>DroneController</a> directory, which is available directly in the git checkout trunk. Inside of this directory is a <a href='https://code.google.com/p/smash-cmu/source/browse/DroneController/README'>README</a> file, which will guide you through the process of installing and compiling the SMASH middleware and applications.<br>
<br>
The directory for the actual logic of the three phases is located in the <a href='https://code.google.com/p/smash-cmu/source/browse/#git%2FDroneController%2Fpackages%2Fmadara_client'>madara_client</a> directory.<br>
<br>
<h1>Static Architecture</h1>
The following diagram shows a high-level decomposition of the internal structure of the DroneController application.<br>
<br>
<img src='http://smash-cmu.googlecode.com/git/Architecture/DroneController-Static.png' />

Important things to consider from the diagram:<br>
<ul><li>Modules do not communicate with each other directly, but use the SMASH Knowledge Base as a mediator or common repository. Specific variables can be set and read in order to pass information between modules.<br>
</li><li>Only the Movement and Sensors modules have direct interaction with the sensors (Sense phase) and actuators (Act phase).<br>
</li><li>The Platform interface allows the system to easily add new platforms (for example, for different drone models), without affecting the rest of the application. In practice, the DroneController is compiled for a particular target platform by compiling the common modules as well as the platform-specific modules.