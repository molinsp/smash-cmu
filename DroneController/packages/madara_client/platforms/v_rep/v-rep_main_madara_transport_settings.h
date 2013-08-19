/********************************************************************* 
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * v-rep_main_madara_transportings.h - Defines connection information to the
 * main transport used to communicate bewteen simulated drones and system
 * controllers.
 *********************************************************************/

#ifndef _VREP_MAIN_MADARA_TRANSPORT_SETTINGS_H
#define _VREP_MAIN_MADARA_TRANSPORT_SETTINGS_H

// Defines the IP through which devices on the SMASH network (drones and system controller) will communicate with each other
// will being run on a simulated platform.
#define MAIN_MULTICAST_ADDRESS "239.255.0.1:4150"

#endif
