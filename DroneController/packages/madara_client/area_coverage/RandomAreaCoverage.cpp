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

enum side_t { NORTH, EAST, SOUTH, WEST};
side_t side = NORTH; // initiate <side> where my target is.
#define NUM_SIDES 4 // rectangle

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

//TODO
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
    printf("Initializing random area coverage algorithm.\n");
    if(numDrones == 1)
        m_cellToSearch = new Region(grid);
    else
        m_cellToSearch = calculateCellToSearch(deviceIdx, grid, numDrones);
    return m_cellToSearch;
}

// Select a new side to go to. req.: try hard NOT choose the same side.
side_t get_random_side(side_t curside){
	side_t tmp_side;
  
	for (int i=0;i<1000;i++)
	{ //while(1) is ugly. try 1000 times to get a new target. if you can't, just return side NORTH...
		tmp_side = (side_t)(rand() % NUM_SIDES);	
		if (tmp_side != curside)
    {
      printf("***Chosen side %i\n",tmp_side);	
      return tmp_side;
    }
	}
  printf("***Chosen side %i\n",tmp_side);	
	
	return NORTH;
}

// Calculates the next location to move to, assuming we have reached our
// current target.
// it assigns a new point in the perimeter in a rectangle perfectly oriented NORTH-SOUTH
Position RandomAreaCoverage::getNextTargetLocation()
{
  // region information
  const double maxLon = m_cellToSearch->southEast.longitude;
  const double minLon = m_cellToSearch->northWest.longitude;
  const double minLat = m_cellToSearch->southEast.latitude;
  const double maxLat = m_cellToSearch->northWest.latitude;
 
	side = get_random_side(side); //returns a random side: N, S, W or E.

  // select a location on the side
  double tlat = 0, tlon = 0;
  switch(side)
  {
    case NORTH:
      tlon = frand(minLon, maxLon);
      tlat = maxLat;
      break;
    case EAST:
      tlon = maxLon;
      tlat = frand(minLat, maxLat);
      break;
    case SOUTH:
      tlon = frand(minLon, maxLon);
      tlat = minLat;
      break;
    case WEST:
      tlon = minLon;
      tlat = frand(minLat, maxLat);
      break;
    default:
      cerr << "ERROR: side not in domain" << endl;
      exit(-1);
  }

  // update target
  m_targetLocation.longitude = tlon;
  m_targetLocation.latitude = tlat;

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
