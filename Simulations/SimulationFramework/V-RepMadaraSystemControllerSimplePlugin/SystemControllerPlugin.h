/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * SystemControllerPlugin.h - Specifies a SystemController plugin for VRep.
 *********************************************************************/

#pragma once

#ifndef _SYSTEM_CONTROLLER_PLUGIN_H
#define _SYSTEM_CONTROLLER_PLUGIN_H

#include "ISimplePlugin.h"
#include "MadaraSystemController.h"
#include <string>

// SystemController plugin that implements the ISimlpePlugin interface.
namespace VREP
{
	class SystemControllerPlugin : public VREP::ISimplePlugin
	{
	public:
        // Interface implementation.
        void initialize(int suffix);
        void cleanup();
        void executeStep();
        std::string getId();
    private:
        // The controller used to manage the Madara stuff.
        MadaraController* m_madaraController;

        // The id of the next search request id to send.
        int m_searchRequestId;

        // The id of the next bridge request id to send.
        int m_bridgeRequestId;

        // Checks if there is a new command from the UI and executes it.
        void handleNewCommand();

        void sendSearchRequest();
        int setupSearchArea();
        void sendSearchRequestToDrones(int numDrones, int areaId);
        void sendBridgeRequestForLastPersonFound();

        SMASH::Utilities::Position getObjectPositionInDegrees(std::string objectName);
        SMASH::Utilities::Position getReferencePoint();
	};
}


#endif