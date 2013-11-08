/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * CartesianPosition.h - Declares the structures for a Cartesian Position.
 *********************************************************************/

#ifndef _CARTESIAN_POSITION_H
#define _CARTESIAN_POSITION_H

#include <sstream>      // std::ostringstream
#include <iomanip>      // std::setprecision

namespace SMASH
{
    namespace Utilities
    {
        /**
         * @brief Represents a three dimensional cartesian position.
         **/
        class CartesianPosition
        {
        public:
            double x;
            double y;
            double z;

            /**
             * Blank default constructor.
             **/
            CartesianPosition() {}

            /** Constructor from data.
             * @param   x    The value for the x coordinate.
             * @param   y    The value for the y coordinate.
             * @param   z    The value for the z coordinate.
             **/
            CartesianPosition(double newX, double newY, double newZ=0) 
            {
                x = newX;
                y = newY;
                z = newZ;
            }

            CartesianPosition(const CartesianPosition& copy)
            {
                x = copy.x;
                y = copy.y;
                z = copy.z;
            }

            const CartesianPosition& operator=(const CartesianPosition& right)
            {
                x = right.x;
                y = right.y;
                z = right.z;
                return *this;
            }

            /** Turns a position into a string.
             * @return a string of the form "x,y,z".
             **/
            std::string toString()
            {
                std::stringstream sstream;
                sstream << std::setprecision(10) << x << "," << y << "," << z;
                return sstream.str();
            }
        };
    }
}

#endif
