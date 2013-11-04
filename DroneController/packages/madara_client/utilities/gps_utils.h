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

namespace SMASH
{
	namespace Utilities
	{
        // Calculates the distance in meters between two sets of coordinates.
        double gps_coordinates_distance(double lat1, double long1, double lat2, double long2);

        // Calculates the latitude and longitude of a point given as a distance in meters in x and y from a reference point.
        SMASH::Utilities::Position getLatAndLong(int x, int y, SMASH::Utilities::Position referencePoint);
	}
}

#endif
