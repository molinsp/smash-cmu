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

#include "Position.h"

#include <sstream>        // std::ostringstream
#include <iomanip>        // std::setprecision

#ifndef _LOCATION_H_
#define _LOCATION_H_

namespace SMASHSim
{
    struct Location
    {
      SMASH::Utilities::Position latAndLong;    // degrees.
      double altitude;                          // meters.

      Location(const double& lat=0, const double& lon=0, const double& alt=0) :
        latAndLong(lat, lon), altitude(alt) {}

      /** Turns a location into a string.
        * @return a string of the form "lat,lon,alt".
        **/
      std::string toString()
      {
          std::stringstream sstream;
          sstream << std::setprecision(10) << latAndLong.latitude << "," << 
            latAndLong.longitude << "," << altitude;
          return sstream.str();
      }
    };
}

#endif // _LOCATION_H_
