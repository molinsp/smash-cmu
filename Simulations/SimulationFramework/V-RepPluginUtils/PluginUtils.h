/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * PluginUtils.h - Util functions for creating V-Rep plugins.
 *********************************************************************/

#pragma once

#include "v_repLib.h"
#include <string>

#ifndef _PLUGIN_UTILS_H
#define _PLUGIN_UTILS_H

// Library class that contains several useful VRep-API wrapper functions.
namespace VREP
{
    class PluginUtils
    {
    public:
        static double getDoubleParam(std::string paramName);
        static std::string getStringParam(std::string paramName);
        static std::string getButtonPressedText(std::string uiName);
    };
}

#endif