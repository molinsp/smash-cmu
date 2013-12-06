/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * kb_setup.h - Defines an interface to setup a knowledge base.
 *********************************************************************/

#ifndef _KB_SETUP_H
#define _KB_SETUP_H

#include "madara/knowledge_engine/Knowledge_Base.h"

// Sets up a knowledge base with a transport that is appropriate for this 
// platform.
void setup_knowledge_base(Madara::Knowledge_Engine::Knowledge_Base* knowledge, 
  std::vector<Madara::Transport::Base*> transports, int id, bool enableLog);

#endif
