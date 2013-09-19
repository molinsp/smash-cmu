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

#include "SimplePluginInterface.h"
#include <string>

// SystemController plugin that implements the ISimlpePlugin interface.
namespace SMASH
{
	class SystemControllerPlugin : public ISimplePlugin
	{
	public:
        void initialize(int suffix);
        void cleanup();
        void executeStep();
        std::string getId();
	};
}


#endif