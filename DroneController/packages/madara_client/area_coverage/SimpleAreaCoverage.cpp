/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * SimpleAreaCoverage.cpp - Defines the methods for the algorithm
 *to performa simple area coverage search.
 *********************************************************************/

#include "SimpleAreaCoverage.h"
#include <cmath>

using namespace SMASH::AreaCoverage;
using namespace SMASH::Utilities;

// We set the search column width to roughly the width of the device.
const double SimpleAreaCoverage::SEARCH_COLUMN_WIDTH = 0.5;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Finds the boundaries of the cell the given id will be covering.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Region SimpleAreaCoverage::calculateCellToSearch(int deviceIdx, Region grid, int numDrones)
{
    // This should never happen.
    if(numDrones == 0)
    {
        // This means something is really bad... we don't have a way to notify of errors, so we just return an empty region.
        return Region();
    }

	// Get the two divisors of the number of drones that are more similar to one another, 
    // to be used as the size of the grid in number of drones. This will allow us to generate 
    // a rectangular-shaped grid which is as square-like as possible.
    std::vector<int> divisors = findMiddleDivisors(numDrones);
    int amountOfLines = divisors[0];
    int amountOfColumns = divisors[1];

	// Calculate the size of each cell by diviging the real size of the grid in the amount of drones that will cover
    // the grid, for both sides. Each drone will end up covering a rectangular cell of this size.
    double cellSizeX = (grid.bottomRightCorner.x - grid.topLeftCorner.x) / amountOfLines;
	double cellSizeY = (grid.bottomRightCorner.y - grid.topLeftCorner.y) / amountOfColumns;

    // Calculate my line and column to find my cell, based on my idx.
    int deviceLine = deviceIdx % amountOfLines;
    int deviceColumn = (int) std::floor((double) (deviceIdx / amountOfColumns));

    // Calculate the starting position based on the cell (line and column) and the cells's size.
    Position deviceCellTopLeftCorner;
    deviceCellTopLeftCorner.x = grid.topLeftCorner.x + (deviceLine*cellSizeX);
    deviceCellTopLeftCorner.y = grid.topLeftCorner.y + (deviceColumn*cellSizeY);

    // Calculate the ending position based on the starting one and the cell's size.
    Position deviceCellBottomRightCorner;
    deviceCellBottomRightCorner.x = deviceCellTopLeftCorner.x + cellSizeX;
    deviceCellBottomRightCorner.y = deviceCellTopLeftCorner.y + cellSizeY;

    // Reset the search; indicate that we have not started searching or moving on any axis yet.
    m_searchStarted = false;
    m_movingOnYAxis = false;

    // Return the cell region (also storing it locally for further reference).
    Region deviceCell(deviceCellTopLeftCorner, deviceCellBottomRightCorner);
    m_cellToSearch = deviceCell;
    return deviceCell;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Returns a vector with the two middle divisors of a number (the ones closest to one another).
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<int> SimpleAreaCoverage::findMiddleDivisors(int numberToEvaluate)
{
    // Vector where the divisors will be stored.
	std::vector<int> closestDivisors(2);

    // Iterate over numbers from 2 up to (potentially) 1 less than the number itself, to find further divisors.
	int highestDivisorFound = numberToEvaluate;
	for(int currentDivisorCandidate = 1; currentDivisorCandidate < highestDivisorFound; currentDivisorCandidate++)
    {
        // If the modulo operation returns zero, it means that currentFactorCandidate is a divisor.
		if((numberToEvaluate % currentDivisorCandidate) == 0)
        {
			// Get the divisor that multiplied by currentDivisorCandidate makes the number to factorize.
			int pairedDivisor = (int) (numberToEvaluate / currentDivisorCandidate);

            // Make the two new divisors as the closest ones so far. Once we reach the "middle" of the divisors found,
            // the loop will end and we will have the closest ones in the vector.
			closestDivisors[0] = currentDivisorCandidate;
			closestDivisors[1] = pairedDivisor;

            // Since the currentDivisorCandidate is increasing, the pairedDivisor found is the highest one we will be able to get.
            // We can trim down the loop limit to that divisor, since there won't be any higher divisors after it.
            highestDivisorFound = pairedDivisor;
        }
    }

    return closestDivisors;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Called when we reach the next target location, updates and returns the next target location.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Position SimpleAreaCoverage::getNextTargetLocation()
{
    // If we are being called for the first time to start a search, return the top left corner of the area as the next target.
    if(!m_searchStarted)
    {
        m_searchStarted = true;
        m_targetLocation = m_cellToSearch.topLeftCorner;
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
            bool bottomReached = m_targetLocation.y == m_cellToSearch.bottomRightCorner.y;
            if(bottomReached)
            {
                // Since we are on the bottom, set our next target to the top.
                m_targetLocation.y = m_cellToSearch.topLeftCorner.y;
            }
            else
            {
                // Since we are on the top, set our next target to the bottom.
                m_targetLocation.y = m_cellToSearch.bottomRightCorner.y;
            }

            // Indicate that now we are going to be moving on the Y axis.
            m_movingOnYAxis = true;
        }
    }

    // We updated it internally, but we also return our next target it so it can be used by the movement controller.
    return m_targetLocation;
}