/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
 
#ifndef _UTILITIES_MODULE_H
#define _UTILITIES_MODULE_H

#include <stdlib.h>
#include "madara/knowledge_engine/Knowledge_Base.h"


#define STRING_ENDS_WITH(str, end) (str.length() >= end.length() ? (0 == str.compare (str.length() - end.length(), end.length(), end)) : false)


namespace SMASH
{
	namespace Utilities
	{
		void initialize(Madara::Knowledge_Engine::Knowledge_Base&);
	}
}

#endif
