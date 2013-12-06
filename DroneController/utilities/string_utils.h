/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

/*********************************************************************
 * string_utils.h - Declares string-related util functions.
 *********************************************************************/

#include <string>
#include <vector>

#ifndef _STRING_UTILS_H
#define _STRING_UTILS_H

// Macro to convert from int to std::string.
#define NUM_TO_STR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << std::setprecision(10) << x ) ).str()

// Macro to check if a string ends on a certain string.
#define STRING_ENDS_WITH(str, end) (str.length() >= end.length() ? (0 == str.compare (str.length() - end.length(), end.length(), end)) : false)

namespace SMASH
{
	namespace Utilities
	{
        // Splits a string.
        std::vector<std::string> stringSplit(const std::string &s, char delim);
    }
}

#endif