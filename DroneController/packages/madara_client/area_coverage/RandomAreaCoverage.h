/*******************************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 ******************************************************************************/

/*******************************************************************************
 * RandomAreCoverage.h - Declares the structures and methods for the algorithm
 * to perform a random area coverage search.
 *
 * Drone selects a random side it is not currently on, then selects a random
 * point on that side and moves to it. Repeat until told to quit
 ******************************************************************************/

#ifndef _RANDOM_AREA_COVERAGE_H
#define _RANDOM_AREA_COVERAGE_H

#include "utilities/Position.h"
#include "AreaCoverage.h"

using namespace SMASH::Utilities;

namespace SMASH { namespace AreaCoverage {
	/**
	* @brief Represents a snaking area coverage algorithm.
	**/
	class RandomAreaCoverage : public SMASH::AreaCoverage::AreaCoverage
	{
	public:
		/**
		* Constructors
		*/
		RandomAreaCoverage(int seed = -1);

		/**
		* Destructor
		*/
		virtual ~RandomAreaCoverage();

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
		/**
		* Returns a random float between lower and upper
		*/
		double frand(const double& lower, const double& upper);
	}; // class RandomAreaCoverage

}} // namespace SMASH::AreaCoverage

#endif // _RANDOM_AREA_COVERAGE_H_
