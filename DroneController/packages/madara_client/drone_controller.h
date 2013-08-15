/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * drone_controller.h - Declares functions for the main module.
 *********************************************************************/

#ifndef _DRONE_CONTROLLER_H
#define _DRONE_CONTROLLER_H

#include "madara/knowledge_engine/Knowledge_Base.h"

#include "area_coverage/area_coverage_module.h"
#include "bridge/bridge_module.h"
#include "movement/movement_module.h"
#include "sensors/sensors_module.h"
#include "utilities/utilities_module.h"

namespace SMASH
{
    // Class that holds the main setup and logic for the DroneController.
    class DroneController
    {
    public:
        // Initialize and cleanup the controller.
        bool initialize(int droneId, Madara::Knowledge_Engine::Knowledge_Base* knowledge);
        void cleanup(Madara::Knowledge_Engine::Knowledge_Base* knowledge);

        // Returns the main logic expression.
        Madara::Knowledge_Engine::Compiled_Expression get_main_expression();

        // Gets a string with a summary of the state of the drone.
        std::string getStatusSummaryExpression();
    private:
        // The modules.
        SMASH::AreaCoverage::AreaCoverageModule m_areaCoverageModule;
        SMASH::Bridge::BridgeModule m_bridgeModule;
        SMASH::Movement::MovementModule m_movementModule;
        SMASH::Sensors::SensorsModule m_sensorsModule;
        SMASH::Utilities::UtilitiesModule m_utilitiesModule;

        // Module handling.
        void initializeModules(Madara::Knowledge_Engine::Knowledge_Base& knowledge);
        void cleanupModules(Madara::Knowledge_Engine::Knowledge_Base& knowledge);

        // Drone initialization.
        void initializeDrone(int droneId, Madara::Knowledge_Engine::Knowledge_Base& knowledge);

        // Compile its own expressions.
        void compileExpressions (Madara::Knowledge_Engine::Knowledge_Base & knowledge);
    };
}

#endif
