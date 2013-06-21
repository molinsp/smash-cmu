/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * BridgeAlgorithm.h - Declares the structures and methods for the algorithm
 * that creates a bridge and decides which drone should move where.
 *********************************************************************/

#ifndef _BRIDGE_ALGORITHM_H
#define _BRIDGE_ALGORITHM_H

#include <map>
#include "Position.h"

/**
 * @brief Represents the information of the distance between a certain drone and a particular location.
 **/
namespace SMASH
{
    namespace Bridge
    {
        struct DistanceTuple
        {
            // The id of the drone.
	        int droneId;

            // The id of one of the locations where drones should go to to form the bridge.
	        int relayLocationId;

            // The distance between the drone identified by droneId, and the location relayLocationId.
	        double distance;
        };

        /**
         * @brief Represents a simple bridge algorithm.
         **/
        class BridgeAlgorithm
        {
        public:
	        /** 
             * Calculates the drones more appropriate for a bridge, and returns the position for the indicated drone to go to, if it should participate in the bridge.
             * @param   myId                      The ID of the drone that we want to find a new position to, if any, to move to in the new bridge.
             * @param   commRange                 The range of the radio, used to calculate how many drones are required to form a bridge where each drone is within range of the next.
             * @param   sourcePosition            The position of the source of information (a stopped drone that found a person).
             * @param   sinkPosition              The position of the sink (the device that will be getting information from the people found).
             * @param   availableDronePositions   A map with the positions of each drone, where they key corresponds to the drone's id.
             *
             * @return  The position this drone should go to, or NULL if this drone was not selected for the bridge.
             **/
	        SMASH::Utilities::Position* getPositionInBridge(int myId, 
                                                            double commRange, 
                                                            SMASH::Utilities::Position sourcePosition, 
                                                            SMASH::Utilities::Position sinkPosition, 
                                                            std::map<int, SMASH::Utilities::Position> availableDronePositions);
        };
    }
}

#endif
