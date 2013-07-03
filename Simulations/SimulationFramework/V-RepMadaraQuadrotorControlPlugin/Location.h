/**
 * Location.h
 * Anton Dukeman
 *
 * Simple struct that stores lat/long/alt
 */

#ifndef _LOCATION_H_
#define _LOCATION_H_

struct Location
{
  double m_lat;
  double m_long;
  double m_alt; // meters

  Location(const double& lat = 0, const double& lon = 0, const double& alt = 0) :
    m_lat(lat), m_long(lon), m_alt(alt) {}
};

#endif // _LOCATION_H_
