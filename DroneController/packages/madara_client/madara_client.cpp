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

// Note that this only includes the "interface", or function declarations, not the implementation.
// The implementation to be used is included in the compilation depending on a flag.
#include "platforms/platform.h"

#include "drone_controller.h"

#include <sstream>
using std::stringstream;

// Interupt handling.
volatile bool g_terminated = false;
extern "C" void terminate (int)
{
    g_terminated = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Enntry point.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main (int argc, char** argv)
{
    // Set the use of Ctrl+C to terminate.
    ACE_Sig_Action sa ((ACE_SignalHandler) terminate, SIGINT);
    
    // Check command-line parameters.
    // The first parameter should be an id.
    int id = 0;
    if (argc > 1)
        id = atoi(argv[1]);
    else
    {
        printf("You must supply an ID as the first argument\n");
        return 0;
    }

    // The second optional parameters is the debug level for logging.
    int local_debug_level = 0;
    if (argc > 2)
        local_debug_level = atoi(argv[2]);
        
    // We call platform init early in case it has to fork().
    if (!platform_init())
    {
        printf("Failed to initialize the platform\n");
        return 1;
    }

    // Set the debug level.
    MADARA_debug_level = local_debug_level;

    // Create the knowledge base.
    Madara::Knowledge_Engine::Knowledge_Base* knowledge = platform_setup_knowledge_base(id);
 
	// Setup the drone controller.
    SMASH::DroneController controller;
    controller.initialize(id, knowledge);

	// Main loop.
    Madara::Knowledge_Engine::Compiled_Expression mainExpression = controller.get_main_expression();
    Madara::Knowledge_Engine::Eval_Settings eval_settings;
    while (!g_terminated)
    {
        knowledge->evaluate (mainExpression, eval_settings);
        knowledge->print_knowledge();
        ACE_OS::sleep (1);
    }

    printf("\nExiting...\n");
    knowledge->print_knowledge ();

    controller.cleanup(knowledge);

	platform_cleanup();

    delete knowledge;

    return 0;
}
