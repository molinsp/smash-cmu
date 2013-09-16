/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * BridgeAlgorithm.cpp - Defines the methods for the algorithm
 * that creates a bridge and decides which drone should move where.
 *********************************************************************/

#include <cmath>
#include <list>
#include <vector>
#include <map>
#include "BridgeAlgorithm.h"
#include "utilities/gps_utils.h"

#include <iostream>
#include <fstream>
#include <sstream>

// Used to enable a simple log to debug algorithm problems.
#define BRIDGE_ALGORITHM_DEBUG 1

using namespace std;
using namespace SMASH::Bridge;
using namespace SMASH::Utilities;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Compares two distance tuples by the distance field. It is not a class method, as a simple function is required.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool compareByDistance(DistanceTuple first, DistanceTuple second)
{
	return first.distance < second.distance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Creates a bridge given the source, sink, and available drone locations. 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Position* BridgeAlgorithm::getPositionInBridge(int myId, double commRange, Position sourcePosition, Position sinkPosition, 
                                               map<int, Position> availableDronePositions)
{
#if BRIDGE_ALGORITHM_DEBUG
    std::ofstream outputFile;
    outputFile.open(std::string("algorithm" + NUM_TO_STR(myId) + ".txt").c_str());
#endif

	Position* myNewPosition = NULL;

    // Calculate the length of the bridge in meters, to compare it to the comm range we got. Note that it is assuming the positions are in degrees.
	//double bridgeLength = sqrt(pow(sourcePosition.x - sinkPosition.x, 2) + pow(sourcePosition.y - sinkPosition.y, 2));
    double bridgeLength = gps_coordinates_distance(sourcePosition.latitude, sourcePosition.longitude, sinkPosition.latitude, sinkPosition.longitude);

	// Calculate how many drones we need for the bridge.
	int numberOfRelays = (int) ceil(bridgeLength/commRange + 1);    // +1 to add a relay in each end point.

#if BRIDGE_ALGORITHM_DEBUG
    outputFile << "Source: (" << std::setprecision(10) << sourcePosition.x << ", " << sourcePosition.y  << "), sink: (" << sinkPosition.x << ", " << sinkPosition.y << ")" << endl;
	outputFile << "Num Relays " << numberOfRelays << endl;
#endif

    // If no relays are required, return NULL.
    if(numberOfRelays < 2)
    {
        return myNewPosition;
    }

	// Calculate the locations of each relay drone.
	vector<Position> relayList(numberOfRelays);
	for(int i=0; i<numberOfRelays; i++)
	{
		Position relayPosition;
		//relayPosition.x = sinkPosition.x + (sourcePosition.x - sinkPosition.x)/(numberOfRelays + 1)*(i+1);
		//relayPosition.y = sinkPosition.y + (sourcePosition.y - sinkPosition.y)/(numberOfRelays + 1)*(i+1);
        relayPosition.x = sinkPosition.x + (sourcePosition.x - sinkPosition.x)/(numberOfRelays - 1)*(i);
		relayPosition.y = sinkPosition.y + (sourcePosition.y - sinkPosition.y)/(numberOfRelays - 1)*(i);
		relayList[i] = relayPosition;

#if BRIDGE_ALGORITHM_DEBUG
        outputFile << "Relay " << i << ", pos " << relayPosition.x << ", " << relayPosition.y << ")" << endl;
#endif
	}

	// List to mark when a drone has been assigned.
	map<int, bool> assignedDrones;

	// Calculate the distances between each drone and each relay location.
	list<DistanceTuple> distanceTuples;
	typedef std::map<int, Position>::iterator it_type;
	for(it_type iterator = availableDronePositions.begin(); iterator != availableDronePositions.end(); iterator++) 
	{
		// Get info for current drone ID and position, and mark this drone as not assigned for now.
		int currentDroneId = iterator->first;
		Position currentDronePos = iterator->second;
		assignedDrones[currentDroneId] = false;

		for(int j=0; j<numberOfRelays; j++)
		{
			// Calculate the distance betwee drone i and location j.
			Position currentRelayPos = relayList[j];
			double distanceToLocation = sqrt(pow(currentDronePos.x - currentRelayPos.x,2) + pow(currentDronePos.y - currentRelayPos.y,2));

			// Store the distance and the information about the drone and location in a list to be sorted later.
			DistanceTuple currentDistanceTuple;
			currentDistanceTuple.droneId = currentDroneId;
			currentDistanceTuple.relayLocationId = j;
			currentDistanceTuple.distance = distanceToLocation;
			distanceTuples.push_back(currentDistanceTuple);

#if BRIDGE_ALGORITHM_DEBUG
            outputFile << "Drone " << currentDroneId << ", to loc " << j << ", distance " << distanceToLocation << "" << endl;
#endif
		}
	}

	// Sort the list with all the distances.
	distanceTuples.sort(compareByDistance);

	// List to store the drone assigned to each location.
	vector<int> droneAssignedToLocation(numberOfRelays);
	for(int i=0; i<numberOfRelays; i++)
	{
		droneAssignedToLocation[i] = -1;
	}

	// Find the best drone-location pairs, with the smallest distances.
	list<DistanceTuple>::iterator currentTuple;
	for(currentTuple=distanceTuples.begin(); currentTuple!=distanceTuples.end(); ++currentTuple)
	{
		// Check if this drone-location pair is useful, if both drone and location have not been assigned yet.
		bool droneAndRelayLocationNotAssigned = !assignedDrones[currentTuple->droneId] && (droneAssignedToLocation[currentTuple->relayLocationId]==-1);
		if(droneAndRelayLocationNotAssigned)
		{
			// Mark this drone as assigned, and store the drone assigned to that location.
			assignedDrones[currentTuple->droneId] = true;
			droneAssignedToLocation[currentTuple->relayLocationId] = currentTuple->droneId;

#if BRIDGE_ALGORITHM_DEBUG
            outputFile << "Loc " << currentTuple->relayLocationId << ", drone " << currentTuple->droneId << std::endl;
#endif

			// Check if this is me to stop search right away?
			if(currentTuple->droneId == myId)
			{
				myNewPosition = new Position();
				myNewPosition->x = relayList[currentTuple->relayLocationId].x;
				myNewPosition->y = relayList[currentTuple->relayLocationId].y;

				// break;
			}
		}
	}

#if BRIDGE_ALGORITHM_DEBUG
    outputFile.close();
#endif

	// This will be null if I didn't end up inside the bridge.
	return myNewPosition;
}