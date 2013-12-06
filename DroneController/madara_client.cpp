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
// Instructions on how the program works.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void programSummary(char* arg)
{
    cerr << arg << endl;
    cerr << "  [-i] id" << endl;
    cerr << "  [-l] MADARA log level" << endl;
    exit(-1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Argument handling.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void handleArgs(int argc, char** argv, int& id, int& logLevel)
{
    for(int i = 1; i < argc; ++i)
    {
        string arg(argv[i]);

        if(arg == "-i" && i + 1 < argc)
        {
            sscanf(argv[++i], "%d", &id);
        }
        else if(arg == "-l" && i + 1 < argc)
        {
            sscanf(argv[++i], "%d", &logLevel);
        }
        else
        {
            programSummary(argv[0]);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Enntry point.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main (int argc, char** argv)
{
    // Set the use of Ctrl+C to terminate.
    ACE_Sig_Action sa ((ACE_SignalHandler) terminate, SIGINT);
    
    // Check command-line parameters.
    int id = 0;
    int local_debug_level = -1;
    cout << "Parsing args..." << endl;
    handleArgs(argc, argv, id, local_debug_level);
    cout << "  id:           " << id << endl;
    cout << "  debug level:  " << local_debug_level << endl;
        
    // We call platform init early in case it has to fork().
    if (!platform_init())
    {
        printf("Failed to initialize the platform\n");
        return 1;
    }

    // Set the debug level.
    bool enableLogging = false;
    if(local_debug_level != -1)
    {
        MADARA_debug_level = local_debug_level;
        enableLogging = true;
    }

    // Create the knowledge base.
    Madara::Knowledge_Engine::Knowledge_Base* knowledge = platform_setup_knowledge_base(id, enableLogging);
 
	// Setup the drone controller.
    SMASH::DroneController controller;
    controller.initialize(id, knowledge);

	// Main loop.
    Madara::Knowledge_Engine::Compiled_Expression mainExpression = controller.get_main_expression();
    Madara::Knowledge_Engine::Eval_Settings eval_settings;
    eval_settings.pre_print_statement = controller.getStatusSummaryExpression();
    while (!g_terminated)
    {
        knowledge->evaluate (mainExpression, eval_settings);
        //printf("=====================================\n");
        //knowledge->print_knowledge();
        //printf("=====================================\n");
        ACE_OS::sleep (1);
        printf("\n");
    }

    printf("\nExiting...\n");
    knowledge->print_knowledge ();

    controller.cleanup(knowledge);

	platform_cleanup();

    delete knowledge;

    return 0;
}
