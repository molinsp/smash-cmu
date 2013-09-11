/*******************************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
******************************************************************************/

/*******************************************************************************
* AreaCoverage.cpp - Base class for area coverage algorithms
******************************************************************************/

#include "AreaCoverage.h"
#include "RandomAreaCoverage.h"
#include "SnakeAreaCoverage.h"
#include "InsideOutAreaCoverage.h"

#include "utilities/CommonMadaraVariables.h"

#include <vector>
using std::vector;
#include <cmath>
using std::floor;

using namespace SMASH::AreaCoverage;
using namespace SMASH::Utilities;

// Constructors
AreaCoverage::AreaCoverage() : m_started(false),
    m_cellToSearch(NULL) 
{
}

// Destructor
AreaCoverage::~AreaCoverage() 
{
	delete m_cellToSearch;
}

// Returns the area being searched by this coverage
const Region* AreaCoverage::getSearchRegion()
{
    return m_cellToSearch;
}

// Resets the area coverage in a "pleasing" way
AreaCoverage* AreaCoverage::continueCoverage(AreaCoverage* coverage, const string& next)
{
    AreaCoverage* retVal;
    if(next == MO_AREA_COVERAGE_RANDOM)
        retVal = new RandomAreaCoverage();
    else if(next == MO_AREA_COVERAGE_SNAKE)
        retVal = new SnakeAreaCoverage();
    else if(next == MO_AREA_COVERAGE_INSIDEOUT)
        retVal = new InsideOutAreaCoverage();
    else // default to repeat current coverage beginning from current location
        retVal = coverage->getNextCoverage();
    return retVal;
}

// Calculates the grid that will be used for area coverage, and returns the
// boundaries of the cell for the given device to cover.
Region* AreaCoverage::calculateCellToSearch(int deviceIdx, const Region& grid,
    int numDrones)
{
    // If there is only one drone, return the whole search area, no need to do calculations.
    if(numDrones <= 1)
    {
        return new Region(grid);
    }

    // Just for debugging purposes, print the grid we are searching in.
    string out = "Given grid: NW = " + grid.northWest.toString() + " SE = ";
    out += grid.southEast.toString();
    printf("%s\n", out.c_str());

    // Get the two divisors of the number of drones that are more similar to one
    // another, to be used as the size of the grid in number of drones. This will
    // allow us to generate a rectangular-shaped grid which is as square-like as
    // possible.
    vector<int> divisors = findMiddleDivisors(numDrones);
    int amountOfLines = divisors[0];
    int amountOfColumns = divisors[1];
    printf("Amounts of lines and columns for %d drones: %d, %d \n", numDrones, amountOfLines, amountOfColumns);

    // Calculate the size of each cell by diviging the real size of the grid in
    // the amount of drones that will cover the grid, for both sides. Each drone
    // will end up covering a rectangular cell of this size.
    double cellSizeX = (grid.bottomRightCorner.x - grid.topLeftCorner.x) /
        amountOfLines;
    double cellSizeY = (grid.topLeftCorner.y - grid.bottomRightCorner.y) /
        amountOfColumns;
    printf("Cell size deltaLat: %.10f deltaLong: %.10f \n", cellSizeY, cellSizeX);

    // Calculate my line and column to find my cell, based on my idx.
    int deviceLine = (int) floor( ((double) deviceIdx) / ((double) amountOfColumns));
    int deviceColumn = deviceIdx % amountOfColumns;
    printf("My idx, line and column: %d, %d, %d \n", deviceIdx, deviceLine, deviceColumn);

    // Calculate the starting position based on the cell (line and column) and the
    // cells's size.
    Position deviceCellTopLeftCorner;
    deviceCellTopLeftCorner.x = grid.topLeftCorner.x + (deviceLine*cellSizeX);
    deviceCellTopLeftCorner.y = grid.topLeftCorner.y - (deviceColumn*cellSizeY);
    printf("TopLeft lat: %.10f long:%.10f \n", deviceCellTopLeftCorner.y, deviceCellTopLeftCorner.x);

    // Calculate the ending position based on the starting one and the cell's size
    SMASH::Utilities::Position deviceCellBottomRightCorner;
    deviceCellBottomRightCorner.x = deviceCellTopLeftCorner.x + cellSizeX;
    deviceCellBottomRightCorner.y = deviceCellTopLeftCorner.y - cellSizeY;
    printf("BottomRight lat: %.10f long: %.10f \n", deviceCellBottomRightCorner.y, deviceCellBottomRightCorner.x);

    // Reset the search; indicate that we have not started searching or moving on
    // any axis yet.
    m_started = false;

    // Return the cell region (also storing it locally for further reference).
    Region* deviceCell = new Region(deviceCellTopLeftCorner, deviceCellBottomRightCorner);
    
    m_cellToSearch = deviceCell;
    return deviceCell;
}


// Returns a vector with the two middle divisors of a number (the ones closest
// to one another).
vector<int> AreaCoverage::findMiddleDivisors(int numberToEvaluate)
{
    // Vector where the divisors will be stored.
    vector<int> closestDivisors(2);

    // Iterate over numbers from 2 up to (potentially) 1 less than the number
    // itself, to find further divisors.
    int highestDivisorFound = numberToEvaluate;
    for(int currentDivisorCandidate = 1;
        currentDivisorCandidate < highestDivisorFound; currentDivisorCandidate++)
    {
        // If the modulo operation returns zero, it means that
        // currentFactorCandidate is a divisor.
        if((numberToEvaluate % currentDivisorCandidate) == 0)
        {
            // Get the divisor that multiplied by currentDivisorCandidate makes the
            // number to factorize.
            int pairedDivisor = (int) (numberToEvaluate / currentDivisorCandidate);

            // Make the two new divisors as the closest ones so far. Once we reach the
            // "middle" of the divisors found, the loop will end and we will have the
            // closest ones in the vector.
            closestDivisors[0] = currentDivisorCandidate;
            closestDivisors[1] = pairedDivisor;

            // Since the currentDivisorCandidate is increasing, the pairedDivisor
            // found is the highest one we will be able to get. We can trim down the
            // loop limit to that divisor, since there won't be any higher divisors
            // after it.
            highestDivisorFound = pairedDivisor;
        }
    }

    return closestDivisors;
}
