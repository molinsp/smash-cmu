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

Right now, the simulation works in the following way: by default, drones are
covering the search area dividing it in quadrants and zig-zagging in each
quadrant. When a drone finds a person, it will stop searching and stay in that 
location. All drones will be notified, and the system will calculate how many, 
and which drones, will have to move to create a bridge between the static drone 
and the laptop (sink). In reality this would be triggerd by the laptop user, 
not automatically.

The current simulation attemps to create a line-bridge (which may not be the
optimal way of creating a bridge) that minimizes the distances that the drones
have to move, in order to reduce the amount of battery they spend to do this.
The drones not required for the bridge will continue going over their search 
patterns. One of its assumptions is that there are going to be enough drones 
to create a bridge.

The method it uses is brute force, calculating all distances from all drones
to all locations, sorting them, and using the smaller distances to chose which
drone to send to each location. The complexity should O(n*m), where n is the
number of drones, and m the number of drones needed for the bridge, so it can be
O(n^2) in the worst case.

Main simulation parameters:
 - numberOfDrones: the number of drones in the simulation
 - numberOfPeople: the number of people in the simulation
 - radioRange: the range (radius) of the Wi-Fi radio on the drones,
   required to calculate how many drones are needed for the bridge.
   
   
