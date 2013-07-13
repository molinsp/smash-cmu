/*******************************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 ******************************************************************************/

/*******************************************************************************
 * SnakeAreaCoverage.cpp - Defines the methods for the algorithm to perform a
 * snaking area coverage search.
 ******************************************************************************/

#include "SnakeAreaCoverage.h"
#include <cmath>

using namespace SMASH::AreaCoverage;
using namespace SMASH::Utilities;

// We set the search column width to roughly the width of the device, in meters.
const double SnakeAreaCoverage::SEARCH_COLUMN_WIDTH = 0.5;

// Constructors
SnakeAreaCoverage::SnakeAreaCoverage(const SMASH::Utilities::Region& region) :
  AreaCoverage(region) {}

// Destructor
SnakeAreaCoverage::~SnakeAreaCoverage() {}

// Initialize the area for the drone
Region SnakeAreaCoverage::initialize(int deviceIdx, const Region& grid, int numDrones)
{
  return calculateCellToSearch(deviceIdx, grid, numDrones);
}

// Called when we reach the next target location, updates and returns the next
// target location.
Position SnakeAreaCoverage::getNextTargetLocation()
{
    // If we are being called for the first time to start a search, return the
    // top left corner of the area as the next target.
    if(!m_started)
    {
        m_started = true;
        m_targetLocation = m_searchRegion.topLeftCorner;
    }
    else
    {
        // Check if we were moving on the Y axis or not.
        if(m_movingOnYAxis)
        {
            // If we were moving on the Y axis, that means that we just finished searching in the current search column.
            // We set our next target to the right, to the start of the next column.
            // After we reach this target, we will be ready to move down or up the search column.
            m_targetLocation.x -= SEARCH_COLUMN_WIDTH;
            m_movingOnYAxis = false;
        }
        else
        {
            // If we are in here, we just moved to the beginning of a search column (either on the top or the bottom).

            // Check if we are at the end of a column on the top or the bottom of the area.
            bool bottomReached = m_targetLocation.y == m_searchRegion.bottomRightCorner.y;
            if(bottomReached)
            {
                // Since we are on the bottom, set our next target to the top.
                m_targetLocation.y = m_searchRegion.topLeftCorner.y;
            }
            else
            {
                // Since we are on the top, set our next target to the bottom.
                m_targetLocation.y = m_searchRegion.bottomRightCorner.y;
            }

            // Indicate that now we are going to be moving on the Y axis.
            m_movingOnYAxis = true;
        }
    }

    // We updated it internally, but we also return our next target it so it can be used by the movement controller.
    return m_targetLocation;
}

// Query if algorithm has reached final target
// @return  true if final target has been reached, false otherwise
bool SnakeAreaCoverage::hasReachedFinalTarget()
{
  return ((m_targetLocation.x == m_searchRegion.bottomRightCorner.x) &&
          (m_targetLocation.y == m_searchRegion.bottomRightCorner.y));
}
