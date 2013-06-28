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
  float m_lat;
  float m_long;
  float m_alt; // meters

  Location(const float& lat = 0, const float& lon = 0, const float& alt = 0) :
    m_lat(lat), m_long(lon), m_alt(alt) {}
};

#endif // _LOCATION_H_
