/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * comm_sim.h - Sets up a communication platform thorugh a Knowledge Base.
 * for hw simulation.
 *********************************************************************/

#ifndef _COMM_SIM_H
#define _COMM_SIM_H

#include "madara/knowledge_engine/Knowledge_Base.h"

// Sets up a knowledge base with a transport that is appropriate for HW simulation.
Madara::Knowledge_Engine::Knowledge_Base* sim_setup_knowledge_base(int id, bool enableLog);

// Cleans up a sim knowledge base.
void sim_cleanup_knowledge_base(Madara::Knowledge_Engine::Knowledge_Base* knowledge);

#endif
