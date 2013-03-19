######################################################################
# Usage of this software requires acceptance of the SMASH-CMU License,
# which can be found at the following URL:
#
# https://code.google.com/p/smash-cmu/wiki/License
######################################################################

This folder contains a V-Rep simulation scene for bridge-forming. This is 
a proposed secondary mode of operation for the drones, in which the drones
will try to create a "bridge" of wireless connections so that some data stream
(such as video) can be sent from a drone that found something of interest (source)
to a person that wants to see it (sink).

Right now, the simulation works in the following way: when a drone finds a 
person, it will stop searching and stay in that location. All drones will
be notified, and the system will calculate how many, and which drones, 
will have to move to create a bridge between the static drone and the laptop
(sink). In reality this would be triggerd by the laptop user, not automatically.

The current simulation attemps to create a line-bridge (which may not be the
optimal way of creating a bridge) that minimizes the distances that the drones
have to move, in order to reduce the amount of battery they spend to do this.

Main simulation parameters:
 - numberOfDrones: the number of drones in the simulation
 - numberOfPeople: the number of people in the simulation
 - radioRange: the range (radius) of the Wi-Fi radio on the drones,
   required to calculate how many drones are needed for the bridge.
   
   
