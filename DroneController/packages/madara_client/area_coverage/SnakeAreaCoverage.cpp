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

#include "utilities/CommonMadaraVariables.h"

#include <cmath>

using namespace SMASH::AreaCoverage;
using namespace SMASH::Utilities;

// We set the search column width to roughly the width of the device, in meters.
const double SnakeAreaCoverage::SEARCH_COLUMN_WIDTH = 0.000005;

// Constructors
SnakeAreaCoverage::SnakeAreaCoverage() :
AreaCoverage() {}

// Destructor
SnakeAreaCoverage::~SnakeAreaCoverage() {}

// Initialize the area for the drone
Region* SnakeAreaCoverage::initialize(const Region& grid, int deviceIdx, int numDrones)
{
	if(numDrones == 1)
		m_cellToSearch = new Region(grid);
	else
		m_cellToSearch = calculateCellToSearch(deviceIdx, grid, numDrones);
	return m_cellToSearch;
}

// Called when we reach the next target location, updates and returns the next
// target location.
Position SnakeAreaCoverage::getNextTargetLocation()
{
    if(m_cellToSearch == NULL)
    {
        // We should improve error handling here somehow.
        printf("Error! Attempting to get next target when cell to search has not been set.\n");
        return Position(0,0);
    }

    // If we are being called for the first time to start a search, return the
    // top left corner of the area as the next target.
    if(!m_started)
    {
        m_started = true;
        m_targetLocation = m_cellToSearch->topLeftCorner;
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
            bool bottomReached = m_targetLocation.y == m_cellToSearch->bottomRightCorner.y;
            if(bottomReached)
            {
                // Since we are on the bottom, set our next target to the top.
                m_targetLocation.y = m_cellToSearch->topLeftCorner.y;
            }
            else
            {
                // Since we are on the top, set our next target to the bottom.
                m_targetLocation.y = m_cellToSearch->bottomRightCorner.y;
            }

            // Indicate that now we are going to be moving on the Y axis.
            m_movingOnYAxis = true;
        }
    }

    // We updated it internally, but we also return our next target it so it can be used by the movement controller.
    printf("Target location: %.10f, %.10f\n", m_targetLocation.x, m_targetLocation.y);
    return m_targetLocation;
}

// Query if algorithm has reached final target
// @return  true if final target has been reached, false otherwise
bool SnakeAreaCoverage::isTargetingFinalWaypoint()
{
  return (fabs(m_targetLocation.x - m_cellToSearch->bottomRightCorner.x) <
                    (SEARCH_COLUMN_WIDTH / 2)) &&
         (fabs(m_targetLocation.y - m_cellToSearch->bottomRightCorner.y) <
                    (SEARCH_COLUMN_WIDTH / 2));
}
