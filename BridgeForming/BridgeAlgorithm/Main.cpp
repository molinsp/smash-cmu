/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
 
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/utility/Log_Macros.h"
#include "ace/Signal.h"
#include "ace/High_Res_Timer.h"
#include "ace/OS_NS_Thread.h"

//#include "DroneActions.h"
#include "BridgeAlgorithm.h"
#include "MadaraBridgeManager.h"

//Inturupt handling
volatile bool g_terminated = false;
extern "C" void terminate (int)
{
  g_terminated = true;
}

void handle_arguments (int argc, char ** argv);

using namespace std;

// By default we identify ourselves by the hostname set in our OS.
string g_host ("");

// By default, we use the multicast port 239.255.0.1.:4150
const string DEFAULT_MULTICAST_ADDRESS ("239.255.0.1:4150");

// Used for updating various transport settings
Madara::Transport::Settings g_settings;

Madara::Knowledge_Record::Integer g_id;


int main (int argc, char** argv)
{
    // Set the use of Ctrl+C to terminate.
    ACE_Sig_Action sa ((ACE_SignalHandler) terminate, SIGINT);
    
    // Define the transport.
    g_settings.hosts_.resize (1);
    g_settings.hosts_[0] = DEFAULT_MULTICAST_ADDRESS;
    g_settings.type = Madara::Transport::MULTICAST;

    // Handle arguments, if any (include recieving an external ID).
    handle_arguments (argc, argv);    
    
    // Create the knowledge base.
    Madara::Knowledge_Engine::Knowledge_Base knowledge (g_host, g_settings);
    
    // Set our id.
    knowledge.set (".id", (Madara::Knowledge_Record::Integer) g_id);
    
	// Setup everything related to the drones.
	//setupDroidActions(knowledge);
	//Madara::Knowledge_Engine::Compiled_Expression mainLogic = getMainLogic();

	Madara::Knowledge_Engine::Eval_Settings eval_settings;
	eval_settings.pre_print_statement =
		"Mobile drones: {.available_drones}/{controller.max_drones}\n"
		"Current drone {.id} position: {drone{.id}.pos.x},{drone{.id}.pos.y}\n"
		"Drone {.id} is bridging: {drone{.id}.bridging}\n"
		"Target pos for {.id}: {drone{.id}.target_pos.x},{drone{.id}.target_pos.y}\n\n"
		;

	// Setup bridge building.
	MadaraBridgeManager::getInstance().initialize(knowledge);
	std::string buildingMainLogicCall = MadaraBridgeManager::getInstance().getMainLogicMadaraCall();

	// Setup a simple test since we are not inside actual drones.
	MadaraBridgeManager::getInstance().setupBridgeTest();    

    // Until the user presses ctrl+c in this terminal, check for input.
    while (!g_terminated)
    {
        knowledge.evaluate (buildingMainLogicCall, eval_settings);
        ACE_OS::sleep (1);

		double myX = knowledge.get("drone{.id}.pos.x").to_double();

		double sourceX = knowledge.get("drone{.id}.target_position.x").to_double();
		double sourceY = knowledge.get("drone{.id}.target_position.y").to_double();
    }

    knowledge.print_knowledge ();

    return 0;
}

// Parses command line arguments
void handle_arguments (int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    string arg1 (argv[i]);

    if (arg1 == "-m" || arg1 == "--multicast")
    {
        if (i + 1 < argc)
            g_settings.hosts_[0] = argv[i + 1];

        ++i;
    }
    else if (arg1 == "-o" || arg1 == "--host")
    {
        if (i + 1 < argc)
            g_host = argv[i + 1];

        ++i;
    }
    else if (arg1 == "-d" || arg1 == "--domain")
    {
        if (i + 1 < argc)
            g_settings.domains = argv[i + 1];

        ++i;
    }
    else if (arg1 == "-i" || arg1 == "--id")
    {
        if (i + 1 < argc)
        {
            stringstream buffer (argv[i + 1]);
            buffer >> g_id;
        }

        ++i;
    }
    else if (arg1 == "-l" || arg1 == "--level")
    {
        if (i + 1 < argc)
        {
            stringstream buffer (argv[i + 1]);
            buffer >> MADARA_debug_level;
        }

        ++i;
    }
    else
    {
        MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG,
            "\nProgram summary for %s:\n\n" \
            "  Run a drone client that responds to a terminal. If running more\n" \
            "  than one drone, make sure to set their ids to different values.\n\n" \
            " [-o|--host hostname]     the hostname of this process (def:localhost)\n" \
            " [-m|--multicast ip:port] the multicast ip to send and listen to\n" \
            " [-d|--domain domain]     the knowledge domain to send and listen to\n" \
            " [-i|--id id]             the id of this agent (should be non-negative)\n" \
            " [-l|--level level]       the logger level (0+, higher is higher detail)\n" \
            "\n",
            argv[0]));
        exit (0);
    }
  }
}
