/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * MadaraBridgeManager.h - Declares
 *********************************************************************/

#pragma once

#ifndef _MADARA_CONTROLLER_H
#define _MADARA_CONTROLLER_H

#include "madara/knowledge_engine/Knowledge_Base.h"
#include <vector>
#include <string>

// Class that has information about drones that has to be updated periodically.
struct DroneStatus
{
    int id;
    double posx;
    double posy;
    bool flying;
};

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
		y = newY;
	}
};

// Class that simulates the Madara controller of the system (though it also acts as a bridge between
// the drone information given by the simualted drones and the Madara knowledge base).
class MadaraController
{
private:
    // By default we identify ourselves by the hostname set in our OS.
    std::string m_host;

    // Used for updating various transport settings
    Madara::Transport::Settings m_transportSettings;

    // The controller's id.
    int m_id;

    // The communications range for the network.
    double m_commRange;

    // The actual knowledge base.
    Madara::Knowledge_Engine::Knowledge_Base* m_knowledge;

    // A counter for the regions created.
    int m_regionId;

    void updateMyStatus(double posx, double posy);
    void updateDroneStatus(std::vector<DroneStatus> droneStatus);
public:
    MadaraController(int id, double commRange);
    ~MadaraController();
    MadaraController(Madara::Knowledge_Engine::Knowledge_Base* knowledge, int id, double commRange);
    void setupBridgeRequest(int bridgeId, Position sourceTopLeft, Position sourceBottomRight, Position sinkTopLeft, Position sinkBottomRight);
    void updateNetworkStatus(double controllerPosx, double controllerPosy, std::vector<DroneStatus> droneStatusList);
    Position* getBridgePosition(int droneId);
    void stopDrone(int droneId);
    void terminate();
};

#endif
