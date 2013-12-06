/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#ifndef _PLATFORM_H
#define _PLATFORM_H

#include "madara/knowledge_engine/Knowledge_Base.h"

// Initializes whatever is required for the platform to be ready to work.
bool platform_init();

// Sets up a knowledge base with a transport that is appropriate for this platform.
Madara::Knowledge_Engine::Knowledge_Base* platform_setup_knowledge_base(int id, bool enableLogging);

// Cleans up any resources that are particular to this platform.
bool platform_cleanup();

#endif
