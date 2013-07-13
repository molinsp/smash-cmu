/*******************************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 ******************************************************************************/

/*******************************************************************************
 * InsideOutAreaCoverage.h - Declares the structures and methods for the
 * algorithm to perform an inside out area search
 ******************************************************************************/

#include "InsideOutAreaCoverage.h"

#include <iostream>
using std::cerr;
using std::endl;

#include "utilities/Position.h"
#include "AreaCoverage.h"

using namespace SMASH::Utilities;
using namespace SMASH::AreaCoverage;

// Constructor
InsideOutAreaCoverage::InsideOutAreaCoverage(const Utilities::Region& region,
  float delta, direction_t heading, bool clockwise) : AreaCoverage(region),
  m_delta(delta), m_clockwise(clockwise), m_heading(heading), m_iteration(2)
{
  cerr << "delta:   " << m_delta << " " << delta << endl;
  cerr << "heading: " << m_heading << " " << heading << endl;
  cerr << "clock:   " << m_clockwise << " " << clockwise << endl;
  //m_delta = delta; m_clockwise = clockwise; m_heading = heading;
}

// Destructor
InsideOutAreaCoverage::~InsideOutAreaCoverage() {}

// Initialize the area for the drone
Region InsideOutAreaCoverage::initialize(int deviceIdx, const Region& grid,
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

  return m_searchRegion = calculateCellToSearch(deviceIdx, grid, numDrones);
}

// Calculates the next location to move to, assuming we have reached our
// current target.
Position InsideOutAreaCoverage::getNextTargetLocation()
{
  if(!m_started) // start in center of the region
  {
    m_targetLocation.x = (m_searchRegion.bottomRightCorner.x +
                          m_searchRegion.topLeftCorner.x) / 2;
    m_targetLocation.y = (m_searchRegion.bottomRightCorner.y +
                          m_searchRegion.topLeftCorner.y) / 2;
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
bool InsideOutAreaCoverage::hasReachedFinalTarget()
{
  return m_searchRegion.contains(m_targetLocation);
}
