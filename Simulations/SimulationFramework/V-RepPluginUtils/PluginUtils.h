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

namespace VREP
{
  // Library class that contains several useful VRep-API wrapper functions.
  class PluginUtils
  {
  public:
    // Get simulation params setup on the main object, for different types.
    static int getIntParam(std::string paramName);
    static double getDoubleParam(std::string paramName);
    static std::string getStringParam(std::string paramName);

    // Get the text of a button pressed on this step, if any.
    static std::string getButtonPressedText(std::string uiName);

    // Get the position of an object.
    static void getObjectPosition(std::string objectName, float position[3]);

    // Move an object to the given position.
    static void setObjectPosition(std::string objectName, double x, double y, 
      double z);

    // Wrapper to send status messages with std::strings instead of char*.
    static void addStatusbarMessage(std::string message);

    // Teleport a dynamic object.
    static void teleportDynamicObject(std::string objectName, double x, double y, double z);
  };
}

#endif
