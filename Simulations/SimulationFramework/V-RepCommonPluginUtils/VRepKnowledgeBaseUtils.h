/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
#include "madara/knowledge_engine/Knowledge_Base.h"
#include <string.h>

Madara::Knowledge_Engine::Knowledge_Base* setupVRepKnowledgeBase(int transportId, std::string logName, 
                                                                std::string host);
Madara::Knowledge_Engine::Knowledge_Base* setupVRepKnowledgeBase(int transportId, std::string logName, 
                                                                std::string host, std::string domain, int queueLength);
void terminateVRepKnowledgeBase(Madara::Knowledge_Engine::Knowledge_Base* knowledge);
