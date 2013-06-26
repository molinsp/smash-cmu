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
#include "utilities/Position.h"

// Class that has information about drones that has to be updated periodically.
struct DroneStatus
{
    int id;
    SMASH::Utilities::Position position;
    bool flying;
};

// Class that stores a movement command which will be simulated in V-Rep.
struct MovementCommand
{
    std::string command;                        // The actual command.
    SMASH::Utilities::Position position;        // The position, if any, associated to the command.
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
    void terminate();

    // General methods.
    void updateNetworkStatus(double controllerPosx, double controllerPosy, std::vector<DroneStatus> droneStatusList);
    MovementCommand* getNewMovementCommand(int droneId);
    void stopDrone(int droneId);

    // Bridge methods.
    void setupBridgeRequest(int bridgeId, SMASH::Utilities::Position sourceTopLeft, SMASH::Utilities::Position sourceBottomRight, 
                                          SMASH::Utilities::Position sinkTopLeft, SMASH::Utilities::Position sinkBottomRight);
    bool MadaraController::isBridging(int droneId);

    // Area coverage methods.
    void setNewSearchArea(int searchAreaId, SMASH::Utilities::Region areaBoundaries);
    void requestAreaCoverage(int droneId, int searchAreaId);
};

#endif
