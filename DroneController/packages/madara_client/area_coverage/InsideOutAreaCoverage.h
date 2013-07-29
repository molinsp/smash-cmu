/*******************************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 ******************************************************************************/

/*******************************************************************************
 * InsideOutAreaCoverage.h - Declares the structures and methods for the
 * algorithm to perform an inside out area search
 ******************************************************************************/

#ifndef _INSIDE_OUT_AREA_COVERAGE_H_
#define _INSIDE_OUT_AREA_COVERAGE_H_

#include "utilities/Position.h"
#include "AreaCoverage.h"

using namespace SMASH::Utilities;

namespace SMASH { namespace AreaCoverage {
/**
 * @brief Represents a snaking area coverage algorithm.
 **/
class InsideOutAreaCoverage : public SMASH::AreaCoverage::AreaCoverage
{
public:
    /**
     * Use fourway directions
     */
    enum direction_t { NORTH, SOUTH, EAST, WEST };
  
    /**
     * Constructors
     */
    InsideOutAreaCoverage(float delta = 0.5, direction_t heading = EAST, bool clockwise = false);
  
    /**
     * Destructor
     */
    virtual ~InsideOutAreaCoverage();
  
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
     * @return  false, default to algorithm never finishes
     */
    virtual bool isTargetingFinalWaypoint();
  
    /**
     * Determines the next area coverage that should be used
     */
    virtual AreaCoverage* getNextCoverage();

protected:
    // distance between two waypoints
    int m_iteration;
  
    // Offset from previous row/column
    float m_delta;
  
    // rotation style
    bool m_clockwise;
  
    // Determine distance multiplier for traveling in specified direction
    // For when area is not square
    double m_nsMultiplier;
    double m_ewMultiplier;

    // current heading
    direction_t m_heading;
}; // class InsideOutAreaCoverage
}} // namespace SMASH::AreaCoverage

#endif // _INSIDE_OUT_AREA_COVERAGE_H_

