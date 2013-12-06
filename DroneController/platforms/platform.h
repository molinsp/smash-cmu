/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#ifndef _PLATFORM_H
#define _PLATFORM_H

#include "madara/knowledge_engine/Knowledge_Base.h"
#include <vector>

// Initializes whatever is required for the platform to be ready to work.
bool platform_init();

// Return the transport(s) for this platform.
std::vector<Madara::Transport::Base*> platform_get_transports(int id, 
  Madara::Knowledge_Engine::Knowledge_Base* kb);

// Cleans up any resources that are particular to this platform.
bool platform_cleanup();

#endif
