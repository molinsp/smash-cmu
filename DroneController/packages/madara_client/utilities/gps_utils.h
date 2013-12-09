/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

/*********************************************************************
* gps_utils.h - Declares GPS-related util functions.
*********************************************************************/

#ifndef _GPS_UTILS_H
#define _GPS_UTILS_H

#include "Position.h"
#include "CartesianPosition.h"
#include <utility>

// To translate between degrees and meters.
// Assumes latitude +40 degrees.
#define DEGREES_PER_METER 0.000009062 

// For Windows compatibility.
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Conversion from degrees to radians.
#define DEG_TO_RAD(x) ((x)*M_PI/180.0)

namespace SMASH
{
  namespace Utilities
  {
    // Calculates the distance in meters between two sets of coordinates.
    double gps_coordinates_distance(double lat1, double long1, double lat2, 
      double long2);

    // Calculates the latitude and longitude of a point given as a distance 
    // in meters in x and y from a reference point.
    SMASH::Utilities::Position getLatAndLong(double x, double y, 
      SMASH::Utilities::Position referencePoint);

    // Turns coordinates into cartesian values.
    SMASH::Utilities::CartesianPosition getCartesianCoordinates(
      SMASH::Utilities::Position coordinates, 
      SMASH::Utilities::Position referencePoint);
  }
}

#endif
