/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
 
#include <string>
using std::string;
#include <vector>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <stdlib.h>

#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/utility/Log_Macros.h"
#include "ace/Signal.h"
#include "ace/High_Res_Timer.h"
#include "ace/OS_NS_Thread.h"

#include "platforms/platform.h"

#include "movement/movement_module.h"
#include "sensors/sensors_module.h"
#include "utilities/utilities_module.h"
#include "area_coverage/area_coverage_module.h"

#include "main_functions.h"

#include <sstream>
using std::stringstream;

#include "transport/DroneRK_Transport.h"

//Inturupt handling
volatile bool terminated = false;
extern "C" void terminate (int)
{
  terminated = true;
}

int main (int argc, char** argv)
{
    
    ACE_Sig_Action sa ((ACE_SignalHandler) terminate, SIGINT);
    
    int local_debug_level = 0;
    int id = 0;
    if (argc > 1)
        id = atoi(argv[1]);
    else
    {
        printf("You must supply an ID as the first arguement\n");
        return 0;
    }
    if (argc > 2)
        local_debug_level = atoi(argv[2]);
        
    //We call platform init early incase it has to fork()
    Madara::Knowledge_Engine::Knowledge_Base* knowledge;
    if (!init_platform())
    {
        printf("Failed to initialize the platform\n");
        return 1;
    }
    //ACE_OS::sleep(5);

    MADARA_debug_level = local_debug_level;

    // should move this to init_platform
    Madara::Transport::Settings settings;
    settings.hosts_.resize (1);
    settings.hosts_[0] = "192.168.1.255:15000";
    settings.type = Madara::Transport::BROADCAST;
    //settings.type = Madara::Transport::NO_TRANSPORT;
    settings.id = id;
    settings.queue_length = 1024; //Smaller queue len to preserve memory
    char host[30];
    sprintf(host, "drone%d", id);
    knowledge = new Madara::Knowledge_Engine::Knowledge_Base(host, settings);
    //stringstream out;
    //out << settings.id;
    //knowledge->attach_transport(new DroneRK_Transport(out.str(),
        //knowledge->get_context(), settings, true, 500));
 
    //First thing we do is set our ID, this needs to be changed to actually set it
    //Set our ID
    knowledge->set(".id", Madara::Knowledge_Record::Integer(id));

    SMASH::Movement::initialize(*knowledge);
    SMASH::Sensors::initialize(*knowledge);
    SMASH::Utilities::initialize(*knowledge);
    SMASH::AreaCoverage::initialize(*knowledge);
    //SMASH::Bridge::initialize(knowledge);

    //Compile the main logic
    main_compile_expressions(*knowledge);

    Madara::Knowledge_Engine::Eval_Settings eval_settings;

    // TODO: move this into control loop
    knowledge->evaluate("takeoff();");

    Madara::Knowledge_Engine::Compiled_Expression mainExpression = main_get_main_expression();
    while (!terminated)
    {
        knowledge->evaluate (mainExpression, eval_settings);
        knowledge->print_knowledge();
        ACE_OS::sleep (1);
    }

    printf("\nExiting...\n");

    cleanup_platform();

    delete knowledge;

    return 0;
}
