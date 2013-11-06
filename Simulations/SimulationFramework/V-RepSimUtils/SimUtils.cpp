/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * SimUtils.cpp - Utilities for the SMASH simulations.
 *********************************************************************/

#include "SimUtils.h"
#include "PluginUtils.h"
#include "utilities/gps_utils.h"
#include "utilities/string_utils.h"
#include <string>

using namespace SMASHSim;

/////////////////////////////////////////////////////////////////////////////////////////////    
// Gets the reference point for the coordinate translation, from the scene parameters.
/////////////////////////////////////////////////////////////////////////////////////////////
SMASH::Utilities::Position SimUtils::getReferencePoint()
{
    double refLat = VREP::PluginUtils::getDoubleParam("referenceLat");
    double refLong = VREP::PluginUtils::getDoubleParam("referenceLong");
    SMASH::Utilities::Position refPoint;
    refPoint.latitude = refLat;
    refPoint.longitude = refLong;

    return refPoint;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the name of the drone.
///////////////////////////////////////////////////////////////////////////////
std::string SimUtils::getDroneName(int id)
{
    // For all drones but the first one (id 0), we have to add the suffix, 
    // which starts at 0 (id-1).
    std::string droneObjectName = "Quadricopter#";
    if(id != 0)
    {
        droneObjectName = droneObjectName + NUM_TO_STR(id-1);
    }

    return droneObjectName;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Returns the position in degrees.
///////////////////////////////////////////////////////////////////////////////////////////////
Location SimUtils::getObjectPositionInDegrees(std::string objectName)
{
    // Get the handle of the object.
    int objectHandle = simGetObjectHandle(objectName.c_str());

    // Get the cartesian position first, and turn it into lat and long.
    SMASH::Utilities::Position refPoint = SimUtils::getReferencePoint();
    float vrepPosition[3];
    simGetObjectPosition(objectHandle, -1, vrepPosition);
    SMASH::Utilities::Position latAndLong = SMASH::Utilities::getLatAndLong(vrepPosition[0], vrepPosition[1], refPoint);

    // Put into a status info object and return it.
    Location positionInfo;
    positionInfo.latAndLong.latitude = latAndLong.latitude;
    positionInfo.latAndLong.longitude = latAndLong.longitude;
    positionInfo.altitude = vrepPosition[2];
    return positionInfo;
}

