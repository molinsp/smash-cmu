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

// Represents a two dimensional position, ignoring height.
class Position
{
public:
	double x;
	double y;

	// Blank default constructor.
	Position() {}

	// Constructor from data.
	Position(double newX, double newY)
	{
		x = newX;
		y= newY;
	}
};

// Represents the information of the distance between a certain drone and a particular location.
struct DistanceTuple
{
	int droneId;			// The id of the drone.
	int relayLocationId;	// The id of one of the locations where drones should go to to form the bridge.
	double distance;		// The distance between the drone identified by droneId, and the location relayLocationId.
};

// Represents a drone that can be part of a bridge.
class BridgeAlgorithm
{
public:
	Position* getPositionInBridge(int myId, double commRange, Position sourcePosition, Position sinkPosition, std::map<int, Position> availableDronePositions);
};

#endif
