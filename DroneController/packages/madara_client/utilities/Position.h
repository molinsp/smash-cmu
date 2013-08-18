/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * Position.h - Declares the structures for a Position and a Region.
 *********************************************************************/

#ifndef _POSITION_H
#define _POSITION_H

#include <iostream>     // std::dec
#include <sstream>      // std::ostringstream
#include <iomanip>        // std::setprecision

// Macro to convert from int to std::string.
#define NUM_TO_STR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << std::setprecision(10) << x ) ).str()

namespace SMASH
{
    namespace Utilities
    {
        /**
         * @brief Represents a two dimensional position, ignoring height.
         **/
        class Position
        {
        public:
            // TODO: fix other code so this is no longer necessary
            double x;
            double& longitude;
            double y;
            double& latitude;

            /**
             * Blank default constructor.
             **/
            Position() : longitude(x), latitude(y) {}

            /** Constructor from data.
             * @param   newX    The value for the X coordinate.
             * @param   newY    The value for the Y coordinate.
             **/
            Position(double newX, double newY) : longitude(x), latitude(y)
            {
                x = newX;
                y = newY;
            }

            Position(const Position& copy) : longitude(x), latitude(y)
            {
                x = copy.x;
                y = copy.y;
            }

            const Position& operator=(const Position& right)
            {
                x = right.x;
                y = right.y;
                return *this;
            }

            /** Turns a position into a string.
             * @return a string of the form "x,y".
             **/
            std::string toString()
            {
                return NUM_TO_STR(latitude) + std::string(",") + NUM_TO_STR(longitude);
            }
        };

        /**
         * @brief Represents a rectangular region defined by its top left and bottom right corners
         **/
        class Region
        {
        public:
            /**
             * Mark the corners of the Region
             */
            enum Corner { SOUTH_WEST, SOUTH_EAST, NORTH_WEST, NORTH_EAST };

            // TODO: fix other code so this is no longer needed
            Position topLeftCorner;
            Position& northWest;
            Position bottomRightCorner;
            Position& southEast;

            /**
             * Blank default constructor.
             **/
            Region() : southEast(bottomRightCorner), northWest(topLeftCorner) {}

            /** Constructor from data.
             * @param   newTopLeftCorner        The Position for the top left corner.
             * @param   newBottomRightCorner    The Position for the bottom right corner.
             **/
            Region(Position newTopLeftCorner, Position newBottomRightCorner) :
                southEast(bottomRightCorner), northWest(topLeftCorner)
            {
                topLeftCorner = newTopLeftCorner;
                bottomRightCorner = newBottomRightCorner;
            }

            Region(const Region& copy) : southEast(bottomRightCorner), northWest(topLeftCorner)
            {
                topLeftCorner = copy.topLeftCorner;
                bottomRightCorner = copy.bottomRightCorner;
            }

            const Region& operator=(const Region& copy)
            {
                topLeftCorner = copy.topLeftCorner;
                bottomRightCorner = copy.bottomRightCorner;
                return *this;
            }

            bool contains(const Position& test)
            {
                return ((test.x >= topLeftCorner.x) &&
                        (test.x <= bottomRightCorner.x) &&
                        (test.y >= bottomRightCorner.y) &&
                        (test.y <= topLeftCorner.y));
            }
        };
    }
}

#endif
