/*******************************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 ******************************************************************************/

/*******************************************************************************
 * RandomAreCoverage.cpp - Declares the structures and methods for the algorithm
 * to perform a snaking area coverage search.
 ******************************************************************************/

#include "RandomAreaCoverage.h"

using namespace SMASH::AreaCoverage;
using namespace SMASH::Utilities;

#include <iostream>
using std::cerr;
using std::endl;

// Constructors
RandomAreaCoverage::RandomAreaCoverage(const SMASH::Utilities::Region& region, 
  int seed) : AreaCoverage(region)
{
  // seed the random number generator
  //if(seed == -1)
    srand((unsigned)time(NULL));
  //else
    //srand(seed);
}

// Destructor
RandomAreaCoverage::~RandomAreaCoverage() {}

// Calculates the next location to move to, assuming we have reached our
// current target.
Position RandomAreaCoverage::getNextTargetLocation()
{
  enum side_t { NORTH = 0, EAST, SOUTH, WEST, NUM_SIDES };

  // region information
  const double maxLon = m_searchRegion.bottomRightCorner.y;
  const double minLon = m_searchRegion.topLeftCorner.y;
  const double minLat = m_searchRegion.bottomRightCorner.x;
  const double maxLat = m_searchRegion.topLeftCorner.x;

  // select a side to go to
  side_t side;
  if(!m_started) // select a random edge
  {
    side = (side_t)(rand() % NUM_SIDES);
    m_started = true;
  }
  else // select a random different edge than we are on
  {
    // what side are we on now
    if(m_targetLocation.x == minLat)
      side = SOUTH;
    else if(m_targetLocation.x == maxLat)
      side = NORTH;
    else if(m_targetLocation.y == minLon)
      side = WEST;
    else // m_targetLocation.y == maxLon)
      side = EAST;

    // get a noncurrent side
    side = (side_t)((side + (rand() % (NUM_SIDES - 1))) % NUM_SIDES);
  }

  // select a location on the side
  double lat, lon;
  switch(side)
  {
    case NORTH:
      cerr << "moving to north" << endl;
      lon = frand(minLon, maxLon);
      lat = maxLat;
      break;
    case EAST:
      cerr << "moving to east" << endl;
      lon = maxLon;
      lat = frand(minLat, maxLat);
      break;
    case SOUTH:
      cerr << "moving to south" << endl;
      lon = frand(minLon, maxLon);
      lat = minLat;
      break;
    case WEST: // case WEST
      cerr << "moving to west" << endl;
      lon = minLon;
      lat = frand(minLat, maxLat);
      break;
    default:
      cerr << "ERROR: side not in domain" << endl;
      exit(-1);
  }

  cerr << "bottom right: <" << minLat << "," << maxLon << ">" << endl;
  cerr << "top left    : <" << maxLat << "," << minLon << ">" << endl;
  cerr << "moving to   : <" << lat << "," << lon << ">" << endl;
  // update target
  m_targetLocation.x = lat;
  m_targetLocation.y = lon;

  return m_targetLocation;
}

// Returns a random double between lower and upper
double RandomAreaCoverage::frand(const double& lower, const double& upper)
{
  double norm = ((double)rand()) / ((double)RAND_MAX);
  return (norm * (upper - lower)) + lower;
}
