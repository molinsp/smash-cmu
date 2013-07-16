/*******************************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
******************************************************************************/

/*******************************************************************************
* AreaCoverage.h - Base class for area coverage algorithms
******************************************************************************/

#ifndef _AREA_COVERAGE_H_
#define _AREA_COVERAGE_H_

#include "utilities/utilities_module.h"
#include "utilities/Position.h"

#include "madara/knowledge_engine/Knowledge_Base.h"

#include <vector>
using std::vector;

using namespace SMASH::Utilities;

namespace SMASH { namespace AreaCoverage {
	/**
	* @brief Represents a base class for area coverage algorithms
	**/
	class AreaCoverage
	{
	public:
		/**
		* Constructor.
		*/
		AreaCoverage();

		/**
		* Destructor.
		*/
		virtual ~AreaCoverage();

		/** 
		* Calculates the next location to move to, assuming we have reached our
		* current target.
		*
		* @return  The position this device should go to next to follow the
		*          search pattern.
		*/
		virtual Position getNextTargetLocation() = 0;

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
		virtual Region* initialize(int deviceIdx, const Region& grid, int numDrones) = 0;

		/**
		* Query if algorithm has reached final target
		* @return  true if final target has been reached, false otherwise
		*/
		virtual bool isTargetingFinalWaypoint() = 0;

	protected:
		/**
		* Flag to indicate if we started searching or not.
		*/
		bool m_started;

		/**
		* Stores the coordinates of the area we are searching.
		*/
		Region* m_cellToSearch;

		/**
		* Stores the location we are currently using as our target (i.e., where we
		* are moving towards)
		*/
		Position m_targetLocation;

		/** 
		* Calculates the grid that will be used for area coverage, and returns the
		* boundaries of the cell for the given device to cover.
		* @param   deviceIdx The position of the device in the list of current
		*                    devices, used to know where to place it.
		* @param   grid      A Region which will be covered by a certain number of
		*                    devices.
		* @param   numDrones The amount of devices covering the grid.
		*
		* @return  The region that this device will be covering.
		**/
		Region* calculateCellToSearch(int deviceIdx, const Region& grid,
			int numDrones);

		/**
		* Returns a vector with the two middle divisors of a number (the ones closest
		* to one another).
		*/
		vector<int> findMiddleDivisors(int numberToEvaluate);
	}; // class AreaCoverage
}} // namespace SMASH::AreaCoverage

#endif // _AREA_COVERAGE_H_
