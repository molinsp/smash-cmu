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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Finds the boundaries of the cell the given id will be covering.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Region SimpleAreaCoverage::findCellBoundaries(int deviceId, Region grid, int numDrones)
{
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

    // Calculate my line and column to find my cell, based on my id.
    int deviceLine = deviceId % amountOfLines;
    int deviceColumn = (int) std::floor((double) (deviceId / amountOfColumns));

    // Calculate the starting position based on the cell (line and column) and the cells's size.
    Position deviceCellTopRightCorner;
    deviceCellTopRightCorner.x = grid.topLeftCorner.x + (deviceLine*cellSizeX);
    deviceCellTopRightCorner.y = grid.topLeftCorner.y + (deviceColumn*cellSizeY);

    // Calculate the ending position based on the starting one and the cell's size.
    Position deviceCellBottomLeftCorner;
    deviceCellBottomLeftCorner.x = deviceCellTopRightCorner.x + cellSizeX;
    deviceCellBottomLeftCorner.y = deviceCellTopRightCorner.y + cellSizeY;

    // Return the cell region.
    Region deviceCell(deviceCellTopRightCorner, deviceCellBottomLeftCorner);
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