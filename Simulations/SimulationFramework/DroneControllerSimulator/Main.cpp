/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
 
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/transport/multicast/Multicast_Transport.h"
#include "madara/utility/Log_Macros.h"
#include "ace/Signal.h"
#include "ace/High_Res_Timer.h"
#include "ace/OS_NS_Thread.h"

#include "utilities/CommonMadaraVariables.h"
#include "platforms/platform.h"

#include "main_functions.h"

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

// The id of the drone.
int g_id;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Enntry point.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    g_settings.id = g_id;
    
    // Create the knowledge base.
    Madara::Knowledge_Engine::Knowledge_Base knowledge (g_host, g_settings);
    
    //knowledge.log_to_file(string("madaralog" + NUM_TO_STR(g_id) + ".txt").c_str(), false);
    //knowledge.evaluate("#log_level(10)");

	// Initialize the platform.
	init_platform();

	// Startup the drone.
	bool success = initializeDroneController(g_id, knowledge);
	if(!success)
	{
		return 1;
	}

    // Visual settings to show console output.
	Madara::Knowledge_Engine::Eval_Settings eval_settings;
	eval_settings.pre_print_statement =
        "Drone {" MV_MY_ID "}\n"
		"Total:\t\t{" MV_TOTAL_DEVICES "}\n"
		"Position:\t{" MV_DEVICE_LAT("{.id}") "},{" MV_DEVICE_LON("{.id}") "}\n"
		"Mobile:\t\t{" MV_MOBILE("{.id}") "}\n"
		"Bridge ID:\t{" MV_BRIDGE_ID("{.id}") "}\n"
        "Move to Br:\t{.bridge.moving_to_bridge}\n"
        "Search alg:\t{" MV_AREA_COVERAGE_REQUESTED("{.id}") "}\n"
		"Target pos:\t{" MV_MOVEMENT_TARGET_LAT "},{" MV_MOVEMENT_TARGET_LON "}\n"
        "Search end:\t{.area_coverage.cell.bottom_right.location.latitude},{.area_coverage.cell.bottom_right.location.longitude}\n\n"
		"Command:\t{" MV_MOVEMENT_REQUESTED "}: {" MV_MOVEMENT_TARGET_LAT "},{" MV_MOVEMENT_TARGET_LON "}\n"
		;

    // Until the user presses ctrl+c in this terminal, check for input.
	Madara::Knowledge_Engine::Compiled_Expression mainExpression = main_get_main_expression();
    while (!g_terminated)
    {
        knowledge.evaluate (mainExpression, eval_settings);
        ACE_OS::sleep (1);
    }

    knowledge.print_knowledge ();

	cleanup_platform();

	cleanupDroneController(knowledge);

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
