/*******************************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 ******************************************************************************/

/*******************************************************************************
 * RandomAreaCoverage.cpp - Declares the structures and methods for the
 * algorithm to perform a random area coverage search.
 *
 * Drone selects a random side it is not currently on, then selects a random
 * point on that side and moves to it. Repeat until told to quit
 ******************************************************************************/

#include "RandomAreaCoverage.h"

#include "utilities/CommonMadaraVariables.h"

using namespace SMASH::AreaCoverage;
using namespace SMASH::Utilities;

#include <iostream>
using std::cerr;
using std::endl;

// Constructors
RandomAreaCoverage::RandomAreaCoverage(int seed) : AreaCoverage()
{
    // seed the random number generator
    if(seed == -1)
        srand((unsigned)time(NULL));
    else
        srand(seed);
}

// Destructor
RandomAreaCoverage::~RandomAreaCoverage() {}

// Query if algorithm has reached final target
// @return  true if final target has been reached, false otherwise
bool RandomAreaCoverage::isTargetingFinalWaypoint() 
{ 
	return false; 
}

// Initialize the area for the drone
Region* RandomAreaCoverage::initialize(const Region& grid, int deviceIdx,
  int numDrones)
{
    // For this algorithm, all drones will be working on the complete search area.
    printf("Initializing random area coverage algorithm.\n");
    m_cellToSearch = new Region(grid);
    //       m_cellToSearch = calculateCellToSearch(deviceIdx, grid, numDrones);
    return m_cellToSearch;
}

// Calculates the next location to move to, assuming we have reached our
// current target.
Position RandomAreaCoverage::getNextTargetLocation()
{
  enum side_t { NORTH, EAST, SOUTH, WEST, NUM_SIDES };

  // region information
  const double maxLon = m_cellToSearch->southEast.longitude;
  const double minLon = m_cellToSearch->northWest.longitude;
  const double minLat = m_cellToSearch->southEast.latitude;
  const double maxLat = m_cellToSearch->northWest.latitude;

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
    if(m_targetLocation.latitude == minLat)
      side = SOUTH;
    else if(m_targetLocation.latitude == maxLat)
      side = NORTH;
    else if(m_targetLocation.longitude == minLon)
      side = WEST;
    else // if(m_targetLocation.longitude == maxLon)
      side = EAST;

    // get a noncurrent side
    side = (side_t)((side + 1 + (rand() % (NUM_SIDES - 1))) % NUM_SIDES);
  }

  // select a location on the side
  double lat, lon;
  switch(side)
  {
    case EAST:
      lon = frand(minLon, maxLon);
      lat = maxLat;
      break;
    case NORTH:
      lon = maxLon;
      lat = frand(minLat, maxLat);
      break;
    case SOUTH:
      lon = frand(minLon, maxLon);
      lat = minLat;
      break;
    case WEST:
      lon = minLon;
      lat = frand(minLat, maxLat);
      break;
    default:
      cerr << "ERROR: side not in domain" << endl;
      exit(-1);
  }

  // update target
  m_targetLocation.longitude = lon;
  m_targetLocation.latitude = lat;

  return m_targetLocation;
}

// Returns a random double between lower and upper
double RandomAreaCoverage::frand(const double& lower, const double& upper)
{
  double norm = ((double)rand()) / ((double)RAND_MAX);
  return (norm * (upper - lower)) + lower;
}

/**
 * Determines the next area coverage that should be used
 * This should probably never be called
 */
AreaCoverage* RandomAreaCoverage::getNextCoverage()
{
    AreaCoverage* retVal = new RandomAreaCoverage(rand());
    return retVal;
}
