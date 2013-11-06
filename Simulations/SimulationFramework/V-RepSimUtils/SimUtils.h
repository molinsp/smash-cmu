/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

/*********************************************************************
* SimUtils.h - Utilities for the SMASH simulations.
*********************************************************************/

#pragma once

#ifndef _SIM_UTILS_H
#define _SIM_UTILS_H

#include "utilities/Position.h"
#include "MadaraQuadrotorControl.h"
#include <string>

// Class that contains various sim-related functions used by various plugins.
namespace SMASHSim
{
  class SimUtils
  {
  public:
    // Gets the reference point for the coordinate translation, from the 
    // scene parameters.
    static SMASH::Utilities::Position getReferencePoint();

    // Gets the name of a drone object given its id.
    static std::string getDroneName(int id);

    // Returns the position in degrees.
    static Location getObjectPositionInDegrees(std::string objectName);
  };
}

#endif