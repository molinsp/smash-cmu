/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
#include "VRepKnowledgeBaseUtils.h"

#ifdef WIN_VREP
  // Only include the custom transport in Windows, as it is not necessary in Linux.
  #include "Windows_Multicast_Transport.h"
#endif

static Madara::Knowledge_Engine::Knowledge_Base* setupVRepKnowledgeBase(Madara::Transport::Settings transportSettings, std::string logName);

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sets up a Madara knowledge base appropriate for simulation in VRep.
////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Engine::Knowledge_Base* setupVRepKnowledgeBase(int transportId, std::string logName, 
                                                                std::string host)
{
    // Define the transport settings.
    Madara::Transport::Settings transportSettings;
    transportSettings.hosts_.resize(1);
    transportSettings.hosts_[0] = host;
    transportSettings.id = transportId;

    // Setup the knowledge base.
    return setupVRepKnowledgeBase(transportSettings, logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sets up a Madara knowledge base appropriate for simulation in VRep.
////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Engine::Knowledge_Base* setupVRepKnowledgeBase(int transportId, std::string logName, 
                                                                std::string host, std::string domain, int queueLength)
{
    // Define the transport settings.
    Madara::Transport::Settings transportSettings;
    transportSettings.hosts_.resize(1);
    transportSettings.hosts_[0] = host;
    transportSettings.id = transportId;
    transportSettings.domains = domain;
    transportSettings.queue_length = queueLength;

    // Setup the knowledge base.
    return setupVRepKnowledgeBase(transportSettings, logName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sets up a Madara knowledge base appropriate for simulation in VRep.
////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Engine::Knowledge_Base* setupVRepKnowledgeBase(Madara::Transport::Settings transportSettings, std::string logName)
{
    // Setup the actual transport.
#ifndef WIN_VREP
    // In Linux, or Windows outside of V-Rep, we can use the default Mulitcast transport.
    transportSettings.type = Madara::Transport::MULTICAST;
#endif
    
    // Delay launching the transport so that we can activate it when we want to, and setup logging.
    transportSettings.delay_launch = true;
    Madara::Knowledge_Engine::Knowledge_Base::log_level (10);
    Madara::Knowledge_Engine::Knowledge_Base::log_to_file(logName.c_str(), true);

    // Create the knowledge base.
    Madara::Knowledge_Engine::Knowledge_Base* knowledge = new Madara::Knowledge_Engine::Knowledge_Base("", transportSettings);
    Madara::Knowledge_Record::set_precision(10);
    knowledge->print ("Past the Knowledge_Base creation.\n");

#ifdef WIN_VREP
    // In Windows with V-Rep we need a custom transport to avoid crashes due to incompatibilities between Win V-Rep and ACE.
    knowledge->attach_transport(new Windows_Multicast_Transport (knowledge->get_id (),
                                  knowledge->get_context (), transportSettings, true));
#else
    // Everywhere else we just activate the default Multicast transport.
    m_knowledge->activate_transport ();
#endif

    return knowledge;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Closes down a knowledge base.
////////////////////////////////////////////////////////////////////////////////////////////////////////
void terminateVRepKnowledgeBase(Madara::Knowledge_Engine::Knowledge_Base* knowledge)
{
    if(knowledge != NULL)
    {
        MADARA_DEBUG (MADARA_LOG_MAJOR_EVENT, (LM_DEBUG, 
          DLINFO "terminateSimKnowledgeBase:" \
          "Terminating Madara knowledge base.\n"));

        //m_knowledge->close_transport();
        knowledge->clear();
        delete knowledge;
    }
}
