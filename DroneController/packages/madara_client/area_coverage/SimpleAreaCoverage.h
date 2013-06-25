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
                * @param   deviceId             The ID of the device that will cover a specific area.
                * @param   grid                 A Region which will be covered by a certain number of devices.
                * @param   numDrones            The amount of devices covering the grid.
                *
                * @return  The position this drone should go to, or NULL if this drone was not selected for the bridge.
                **/
	        SMASH::Utilities::Region SimpleAreaCoverage::findCellBoundaries(int deviceId, SMASH::Utilities::Region grid, int numDrones);

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
