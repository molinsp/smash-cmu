SMASH is a project at CMU led by Dr. James Edmondson that focuses on extending a human operator's reach by using a swarm of drones and sensors. This Google Code project will house the open source components that build on top of Android's SDK, the [MADARA Google Code project](http://madara.googlecode.com) for distributed knowledge and reasoning, the [V-REP simulator](http://coppeliarobotics.com), and various other open architectures and platforms.


---


To help guide you through the contents of this repository, please consider first visiting the following Wiki pages:

[SMASH Architecture](SmashArchitecture.md) | [The Drone Controller](UnderstandingTheDroneController.md) | [Interacting with SMASH via MADARA Variables](DroneControllerMadaraVariables.md)


---


Features of this project will include (<font color='green'>supported</font> | <font color='blue'>in progress</font> | <font color='red'>planned</font>):

  * <font color='blue'>Algorithms for solving the Area Coverage Problem with mobile robots</font>
    * <font color='green'>Algorithms for basic area coverage</font>
      * <font color='green'>Randomized vectors</font> (<font color='green'>V-REP simulation</font> | <font color='green'>implementation</font>)
      * <font color='green'>Cell decomposition</font> (<font color='green'>V-REP simulation</font> | <font color='green'>implementation</font>)
    * <font color='green'>Algorithms for prioritized area coverage</font>
      * <font color='red'>Randomized vectors</font> (<font color='red'>V-REP simulation</font> | <font color='red'>implementation</font>)
      * <font color='green'>Cell decomposition</font> (<font color='green'>V-REP simulation</font> | <font color='green'>implementation</font>)
  * <font color='blue'>Algorithms for wireless sensor networking</font>
    * <font color='blue'>Algorithms for basic mobile WSN</font> (<font color='green'>V-REP simulation</font> | <font color='green'>implementation</font>)
    * <font color='green'>Algorithm for bridge forming in mobile WSN</font> (<font color='green'>V-REP simulation</font> | <font color='green'>implementation</font>)
    * <font color='red'>Algorithms for prioritized area mobile WSN</font> (<font color='red'>V-REP simulation</font> | <font color='red'>implementation</font>)
    * <font color='red'>Algorithms for battery-optimized mobile WSN</font> (<font color='red'>V-REP simulation</font> | <font color='red'>implementation</font>)
  * <font color='green'>Integration with the <a href='http://madara.googlecode.com'>MADARA</a> project (distributed knowledge and reasoning)</font>
    * <font color='green'>Java port of MADARA</font> (enables Android usage)
    * <font color='green'>Android interface for basic MADARA commands</font>
    * <font color='green'>C++ interfacing for drone command and control</font>



The content of this website and open source project falls under the following [License](https://code.google.com/p/smash-cmu/wiki/License) and [Terms of Use](http://www.sei.cmu.edu/legal/index.cfm).