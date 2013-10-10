/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

#include "gps_utils.h"
#include <math.h>

// For Windows compatibility.
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// Conversion from degrees to radians.
#define DEG_TO_RAD(x) (x)*M_PI/180.0

// Haversin function.
#define HAVERSIN(x) pow(sin((x)/2), 2)

// The radius of the Earth in meters.
static const double EARTH_RADIUS = 6371000;

static const double DEGREES_IN_CIRCUMFERENCE = 360;
static const double EARTH_POLES_PERIMETER = 40008000;
static const double EARTH_EQUATORIAL_PERIMETER = 40075160;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Calculate the distance between two coordinate pairs.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
double SMASH::Utilities::gps_coordinates_distance (double lat1, double long1, double lat2, double long2)
{
    // Turn the latitudes into radians.
    lat1 =  DEG_TO_RAD(lat1);
    lat2 =  DEG_TO_RAD(lat2);

    // Get the difference between our two points then convert the difference into radians.
    double lat_diff = DEG_TO_RAD(lat2 - lat1);
    double long_diff = DEG_TO_RAD(long2 - long1);

    // Calculate the distance in meters via trigonometry, using the haversine formula.
    double pointsHaversin = HAVERSIN(lat_diff) + (cos(lat1) * cos(lat2) * HAVERSIN(long_diff));
    double c = 2 * atan2(sqrt(pointsHaversin), sqrt(1 - pointsHaversin));
    double distanceInMeters = EARTH_RADIUS * c;

    // Return the distance.
    return distanceInMeters;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Calculates the latitude and longitude of a point given as a distance in meters in x and y from a reference point.
/////////////////////////////////////////////////////////////////////////////////////////////    
SMASH::Utilities::Position SMASH::Utilities::getLatAndLong(int x, int y, SMASH::Utilities::Position referencePoint)
{
    // Get the deltas from reference point.
    double perimeterAtRefLatitude =  EARTH_EQUATORIAL_PERIMETER * cos(DEG_TO_RAD(referencePoint.latitude));
    double deltaLongitude = x * DEGREES_IN_CIRCUMFERENCE / perimeterAtRefLatitude;
    double deltaLatitude  = y * DEGREES_IN_CIRCUMFERENCE / EARTH_POLES_PERIMETER;
    
    // Get the lat and long.
    double latitude = deltaLatitude + referencePoint.latitude;
    double longitude = deltaLongitude + referencePoint.longitude;
    
    // Turn this into a Position and return it.
    SMASH::Utilities::Position result;
    result.latitude = latitude;
    result.longitude = longitude;
    return result;
}
