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
  // region information
  const double maxLon = m_cellToSearch->southEast.longitude;
  const double minLon = m_cellToSearch->northWest.longitude;
  const double minLat = m_cellToSearch->southEast.latitude;
  const double maxLat = m_cellToSearch->northWest.latitude;

  // select a side to go to
  if(!m_started) // select a random edge
  {
    m_currentTargetSide = (side_t)(rand() % NUM_SIDES);
    m_started = true;
  }
  else // select a random different edge than we are on
  {
    // get a noncurrent side based on the last side we picked
    m_currentTargetSide = (side_t)((m_currentTargetSide + 1 + (rand() % (NUM_SIDES - 1))) % NUM_SIDES);
  }

  // select a location on the side
  double lat, lon;
  switch(m_currentTargetSide)
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
      // If for some weird reason we end up here, we just pick any heading.
      cerr << "ERROR: side not in domain" << endl;
      lon = frand(minLon, maxLon);
      lat = maxLat;
  }

  // update target
  m_targetLocation.longitude = lon;
  m_targetLocation.latitude = lat;

  return m_targetLocation;
}

// Returns a random double between lower and upper
double RandomAreaCoverage::frand(const double& lower, const double& upper)
{
    // Get a double number between 0 and 1.
    double position_in_range = ((double)rand()) / ((double)RAND_MAX);

    if (lower < upper)
        return (position_in_range * (upper - lower)) + lower;
    else
        return (position_in_range * (lower - upper)) + upper;
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
