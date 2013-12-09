 /*******************************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 ******************************************************************************/

/*******************************************************************************
 * PriorityAreaCoverage.h - Declares the structures and methods for the
 * algorithm to perform a prioritized area search
 ******************************************************************************/

#ifndef _PRIORITY_AREA_COVERAGE_H_
#define _PRIORITY_AREA_COVERAGE_H_

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime> 
#include <string>
#include <unistd.h>
#include <algorithm>
#include <sstream>
#include <cmath>
#include <queue>
#include <set>
#include <utility>
#include "utilities/Position.h"
#include "AreaCoverage.h"

using namespace SMASH::Utilities;

namespace SMASH { namespace AreaCoverage {
/**
 * @brief Represents a snaking area coverage algorithm.
 **/
class PriorityAreaCoverage : public SMASH::AreaCoverage::AreaCoverage
{
public:
    /**
     * Use fourway directions
     */
    enum direction_t { NORTH, SOUTH, EAST, WEST };
  
    /**
     * Constructors
     */
    PriorityAreaCoverage(Madara::Knowledge_Engine::Variables &variables, std::vector<Region> regions, Region searchRegion, double delta = 0.5);
  
    /**
     * Destructor
     */
    virtual ~PriorityAreaCoverage();
  
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


    virtual void init(std::vector<std::vector<int> > &matrix);
    
    virtual void print(std::vector<std::vector<int> > matrix, int x, int y);

    virtual std::pair<int, int> decreasingOrder(std::vector<std::vector<int> > matrix, int x, int y);

    virtual bool inBounds(int x, int y);

    double xToLat(int x);

    double yToLong(int y);

    void solveMatrix(std::vector<std::vector<int> > &matrix, int numDrones, int myId);

    int sumMatrix(std::vector<std::vector<int> > &matrix);

    std::pair<int, int> nextMove(std::vector<std::vector<int> > &matrix, int &x, int &y);

    double distance(int x1, int y1, int x2, int y2);

    std::pair<int, int> stupidSearch(std::vector<std::vector<int> > matrix, int x, int y);

int valueOfDirection(std::vector<std::vector<int> > &matrix, int &x, int &y, int direction);
void move(std::vector<std::vector<int> > &matrix, int &x, int &y, int direction);
int highestNeighbor(std::vector<std::vector<int> > matrix, int x, int y);
int mostValuableDirection(std::vector<std::vector<int> > matrix, int x, int y);
double valueOfDirection(std::vector<std::vector<int> > matrix, int startX, int startY, int directionX, int directionY);

protected:
    // distance between two waypoints 
    int m_iteration;
  
    // Offset from previous row/column
    double m_delta;

    int m_width;

    int m_height;

    double m_id;

    double m_turn;

    std::vector<std::vector<int> > m_matrix;

    Madara::Knowledge_Engine::Variables m_variables;

    // The total region
    Region m_searchRegion;

    // The priorized areas within the search region.
    std::vector<Region> m_regions;
  
    // current heading
    direction_t m_heading;

}; // class PriorityAreaCoverage
}} // namespace SMASH::AreaCoverage

#endif // _PRIORITY_AREA_COVERAGE_H_

