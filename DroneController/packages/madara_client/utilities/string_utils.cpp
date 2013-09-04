/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

#include "string_utils.h"
#include "sstream"

static std::vector<std::string> &stringSplit(const std::string &s, char delim, std::vector<std::string> &elems) 
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> stringSplit(const std::string &s, char delim) 
{
    std::vector<std::string> elems;
    stringSplit(s, delim, elems);
    return elems;
}