/*******************************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 ******************************************************************************/

/*******************************************************************************
 * InsideOutAreaCoverage.cpp - Declares the structures and methods for the
 * algorithm to perform an inside out area search
 ******************************************************************************/

#include "InsideOutAreaCoverage.h"

#include "utilities/Position.h"
#include "utilities/CommonMadaraVariables.h"

#include <iostream>
using std::cerr;
using std::endl;

using namespace SMASH::Utilities;
using namespace SMASH::AreaCoverage;

// Constructor
InsideOutAreaCoverage::InsideOutAreaCoverage(float delta, direction_t heading,
	bool clockwise) : AreaCoverage(), m_delta(delta), 	m_iteration(2),m_clockwise(clockwise),
 m_heading(heading) {}

// Destructor
InsideOutAreaCoverage::~InsideOutAreaCoverage() {}

// Initialize the area for the drone
Region* InsideOutAreaCoverage::initialize(const Region& grid, int deviceIdx, 
  int numDrones)
{
  // trick to get initial heading correct
  switch(m_heading)
  {
    case NORTH:
      m_heading = m_clockwise ? WEST : EAST;
      break;
    case EAST:
      m_heading = m_clockwise ? NORTH : SOUTH;
      break;
    case SOUTH:
      m_heading = m_clockwise ? EAST : WEST;
      break;
    case WEST:
      m_heading = m_clockwise ? SOUTH : NORTH;
      break;
    default:
      cerr << "Invalid direction" << endl;
      exit(-1);
  }

	// find search region
	if(numDrones == 1)
		m_cellToSearch = new Region(grid);
	else
		m_cellToSearch = calculateCellToSearch(deviceIdx, grid, numDrones);
	return m_cellToSearch;
}

// Calculates the next location to move to, assuming we have reached our
// current target.
Position InsideOutAreaCoverage::getNextTargetLocation()
{
  if(!m_started) // start in center of the region
  {
    m_targetLocation.x = (m_cellToSearch->bottomRightCorner.x +
                          m_cellToSearch->topLeftCorner.x) / 2;
    m_targetLocation.y = (m_cellToSearch->bottomRightCorner.y +
                          m_cellToSearch->topLeftCorner.y) / 2;
    m_started = true;
  }
  else
  {
    // update direction
    switch(m_heading)
    {
      case NORTH:
        m_heading = m_clockwise ? EAST : WEST;
        break;
      case EAST:
        m_heading = m_clockwise ? SOUTH : NORTH;
        break;
      case SOUTH:
        m_heading = m_clockwise ? WEST : EAST;
        break;
      case WEST:
        m_heading = m_clockwise ? NORTH : SOUTH;
        break;
      default:
        cerr << "Invalid direction" << endl;
        exit(-1);
    }

    // find new target
    switch(m_heading)
    {
      case NORTH:
        m_targetLocation.x += (m_iteration / 2) * m_delta;
        break;
      case EAST:
        m_targetLocation.y += (m_iteration / 2) * m_delta;
        break;
      case SOUTH:
        m_targetLocation.x -= (m_iteration / 2) * m_delta;
        break;
      case WEST:
        m_targetLocation.y -= (m_iteration / 2) * m_delta;
        break;
      default:
        cerr << "Invalid direction" << endl;
        exit(-1);
    }

    // go to next iteration
    ++m_iteration;
  }

  return m_targetLocation;
}


// Query if algorithm has reached final target
// @return  false, default to algorithm never finishes
bool InsideOutAreaCoverage::isTargetingFinalWaypoint()
{
  return !m_cellToSearch->contains(m_targetLocation);
}

// Determines the next area coverage that should be used
// TODO: Fix this to do OutsideInCoverage
AreaCoverage* InsideOutAreaCoverage::getNextCoverage()
{
    return new InsideOutAreaCoverage(m_delta, m_heading, m_clockwise);
}
