/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * QuadcopterPlatformPlugin.h - Specifies a QuadcopterPlatformPlugin plugin for VRep.
 *********************************************************************/

#pragma once

#ifndef _QUADCOPTER_PLATFORM_PLUGIN_H
#define _QUADCOPTER_PLATFORM_PLUGIN_H

#include "ISimplePlugin.h"
#include "MadaraQuadrotorControl.h"
#include <string>
#include <vector>

// QuadcopterPlatformPlugin plugin that implements the ISimplePlugin interface.
namespace SMASHSim
{
    class QuadcopterPlatformPlugin : public VREP::ISimplePlugin
    {
    public:
        // Interface implementation.
        void initialize(int suffix);
        void cleanup(int suffix);
        void executeStep(int suffix);
        std::string getId();
    private:
        // The controller used to manage the Madara stuff.
        MadaraQuadrotorControl* m_madaraController;

        // Simulate sensing and movement steps.
        void simulateSensors(int droneId);
        void simulateMovement(int droneId);

        // Simulate specific sensors.
        void updateDronePosition(int droneId);
        void updateThermals(int droneId);
    };
}

#endif
