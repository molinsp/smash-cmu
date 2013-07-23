/*******************************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
******************************************************************************/

/*******************************************************************************
* SnakeAreCoverage.h - Declares the structures and methods for the algorithm
* to perform a snaking area coverage search.
******************************************************************************/

#ifndef _SNAKE_AREA_COVERAGE_H
#define _SNAKE_AREA_COVERAGE_H

#include <vector>
#include "utilities/Position.h"
#include "AreaCoverage.h"

using namespace SMASH::Utilities;

namespace SMASH { namespace AreaCoverage {
	/**
	* @brief Represents a snaking area coverage algorithm.
	**/
	class SnakeAreaCoverage : public SMASH::AreaCoverage::AreaCoverage
	{
	public:
		/**
		* Constructors
        * @param   width    width of column offset
		*/
		SnakeAreaCoverage(const double& width = 0.000005);

		/**
		* Destructor
		*/
		virtual ~SnakeAreaCoverage();

		/**
		* Initialize the area for the drone
		* @param   deviceIdx The position of the device in the list of current
		*                    devices, used to know where to place it.
		* @param   grid      A Region which will be covered by a certain number of
		*                    devices.
		* @param   numDrones The amount of devices covering the grid.
		*
		* @return  The region that this device will be covering.
		*/
		virtual Region* initialize(const Region& grid, int deviceIdx = 0, int numDrones = 1);

		/** 
		* Calculates the next location to move to, assuming we have reached our
		* current target.
		*
		* @return  The position this device should go to next to follow the
		*          search pattern.
		*/
		virtual SMASH::Utilities::Position getNextTargetLocation();

		/**
		* Query if algorithm has reached final target
		* @return  true if final target has been reached, false otherwise
		*/
		virtual bool isTargetingFinalWaypoint();


	protected:
		// The width of each column the device will move over when searching the area.
		// It will roughly have the same width as the device.
		const double m_searchColumnWidth;

		// Flag to check whether we are moving on the X or Y axis (to know whether we
		// have to turn or not).
		bool m_movingOnYAxis;
	}; // class SnakeAreaCoverage

}} // namespace SMASH::AreaCoverage

#endif // _SNAKE_AREA_COVERAGE_H_
