/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

/**
 * Location.h
 * Anton Dukeman
 *
 * Simple struct that stores lat/long/alt
 */

#include "utilities/Position.h"

#ifndef _LOCATION_H_
#define _LOCATION_H_

namespace SMASHSim
{
    struct Location
    {
      SMASH::Utilities::Position latAndLong;    // degrees.
      double altitude;                          // meters.

      Location(const double& lat = 0, const double& lon = 0, const double& alt = 0) :
        latAndLong(lat, lon), altitude(alt) {}
    };
}

#endif // _LOCATION_H_
