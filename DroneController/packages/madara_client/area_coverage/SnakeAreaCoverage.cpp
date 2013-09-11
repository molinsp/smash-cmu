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

// Constructors
SnakeAreaCoverage::SnakeAreaCoverage(const Region::Corner& start, const double& width) :
    AreaCoverage(), m_startCorner(start), m_searchColumnWidth(width) {}

// Destructor
SnakeAreaCoverage::~SnakeAreaCoverage() {}

// Initialize the area for the drone
Region* SnakeAreaCoverage::initialize(const Region& grid, int deviceIdx, int numDrones)
{
    printf("Initializing snake area coverage algorithm.\n");
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
        m_targetLocation = m_cellToSearch->northWest;
        m_movingNorthSouth = false; // start by moving north-south
    }
    else
    {
        // Check if we were moving North-South or not.
        if(m_movingNorthSouth)
        {
            // If we were moving North-South, that means that we just finished searching in the
            // current search column.
            // We set our next target to the east, to the start of the next column.
            // After we reach this target, we will be ready to move down or up the search column.
            m_targetLocation.longitude += m_searchColumnWidth;
            m_movingNorthSouth = false;
        }
        else
        {
            // If we are in here, we just moved to the beginning of a search column.

            // Check if we are at the end of a column in the north or south of the area.
            if(m_targetLocation.latitude == m_cellToSearch->southEast.latitude)
            {
                // Since we are on the bottom, set our next target to the north
                m_targetLocation.latitude = m_cellToSearch->northWest.latitude;
            }
            else
            {
                // Since we are on the top, set our next target to the south
                m_targetLocation.latitude = m_cellToSearch->southEast.latitude;
            }

            // Indicate that now we are going to be moving North-South
            m_movingNorthSouth = true;
        }
    }

    // We updated it internally, but we also return our next target it so it can be used by the movement controller.
    printf("Target location: Lat = %.10f, Long = %.10f\n", m_targetLocation.latitude, m_targetLocation.longitude);
    return m_targetLocation;
}

// Query if algorithm has reached final target
// @return  true if final target has been reached, false otherwise
bool SnakeAreaCoverage::isTargetingFinalWaypoint()
{
    double accuracy = (m_searchColumnWidth / 2); 
    bool reachedBottom = fabs(m_targetLocation.longitude - m_cellToSearch->southEast.longitude) < accuracy;
    bool reachedRight =  fabs(m_targetLocation.latitude - m_cellToSearch->southEast.latitude) < accuracy;
    return reachedBottom && reachedRight;
}


// Determines the next area coverage that should be used
AreaCoverage* SnakeAreaCoverage::getNextCoverage()
{
    return new SnakeAreaCoverage((Region::Corner)((m_startCorner + 2) % 4), m_searchColumnWidth);
}
