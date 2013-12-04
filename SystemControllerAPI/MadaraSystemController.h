/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * MadaraSystemController.h - Class that simulates a System Controller.
 *********************************************************************/

#pragma once

#ifndef _MADARA_SYSTEM_CONTROLLER_H
#define _MADARA_SYSTEM_CONTROLLER_H

#include "madara/knowledge_engine/Knowledge_Base.h"
#include "Position.h"

#include <vector>
#include <string>

using std::string;

// Class that simulates the Madara controller of the system (though it also acts as a bridge between
// the drone information given by the simualted drones and the Madara knowledge base).
class MadaraController
{
private:
    // The controller's id.
    int m_id;

    // Whether human detection should be on or off: 1 is on, 0 is off.
    int m_humanDetection;

    // The actual knowledge base.
    Madara::Knowledge_Engine::Knowledge_Base* m_knowledge;

    // A counter for the regions created.
    int m_regionId;
public:
    MadaraController(int id, Madara::Transport::Types transportType);
    ~MadaraController();

    // Commands to take off and land the swarm.
    void sendTakeoffCommand();
    void sendLandCommand();

    // Sets general parameters in Madara variables.
    void updateGeneralParameters(const int& numberOfDrones, const double& commRange, const double& minAltitude, 
                                 const double& heightDiff, const int& coverageTrackingEnabled, const int& coverageTrackingFileEnabled);

    // Bridge methods.
    void setupBridgeRequest(int bridgeId, SMASH::Utilities::Region startRegion, 
                                          SMASH::Utilities::Region endRegion);

    // Area coverage methods.
    void setNewSearchArea(int searchAreaId, SMASH::Utilities::Region& areaBoundaries);
    void requestAreaCoverage(std::vector<int> droneIds, int searchAreaId, string searchAlgorithm, int wait, double lineWidth, 
                             std::string humanDetectionAlgorithm);

    // Information getter methods.
    std::vector<SMASH::Utilities::Position> getCurrentLocations();
    std::vector<SMASH::Utilities::Position> getCurrentThermals();

    void printKnowledge();
};

#endif
