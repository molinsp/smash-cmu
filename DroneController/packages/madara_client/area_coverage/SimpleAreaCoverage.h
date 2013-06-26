/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * SimpleAreCoverage.h - Declares the structures and methods for the algorithm
 * to performa simple area coverage search.
 *********************************************************************/

#ifndef _SIMPLE_AREA_COVERAGE_H
#define _SIMPLE_AREA_COVERAGE_H

#include <vector>
#include "Position.h"

namespace SMASH
{
    namespace AreaCoverage
    {
        /**
         * @brief Represents a simple area coverage algorithm.
         **/
        class SimpleAreaCoverage
        {
        public:
	        /** 
                * Calculates the grid that will be used for area coverage, and returns the boundaries of the cell for the given device to cover.
                * @param   deviceIdx            The position of the device in the list of current devices, used to know where to place it.
                * @param   grid                 A Region which will be covered by a certain number of devices.
                * @param   numDrones            The amount of devices covering the grid.
                *
                * @return  The region that this device will be covering.
                **/
	        SMASH::Utilities::Region SimpleAreaCoverage::calculateCellToSearch(int deviceIdx, SMASH::Utilities::Region grid, int numDrones);

	        /** 
                * Calculates the next location to move to, assuming we have reached our current target.
                *
                * @return  The position this device should go to next to follow the search pattern.
                **/
            SMASH::Utilities::Position SimpleAreaCoverage::getNextTargetLocation();

        private:
            // The width of each column the device will move over when searching the area. It will roughly have the same width as the device.
            static const double SEARCH_COLUMN_WIDTH;

            // Flag to indicate if we started searching or not.
            bool m_searchStarted;

            // Stores the coordinates of the area we are searching.
            SMASH::Utilities::Region m_cellToSearch;

            // Stores the location we are currently using as our target (i.e., where we are moving towards).
            SMASH::Utilities::Position m_targetLocation;

            // Flag to check whether we are moving on the X or Y axis (to know whether we have to turn or not).
            bool m_movingOnYAxis;

            // Finds the middle (closest to each other) divisors of a number.
            std::vector<int> findMiddleDivisors(int numberToEvaluate);
        };
    }
}

#endif
