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

#include "area_coverage/area_coverage_module.h"
#include "bridge/bridge_module.h"
#include "movement/movement_module.h"
#include "sensors/sensors_module.h"
#include "utilities/CommonMadaraVariables.h"
#include "utilities/utilities_module.h"
#include "platforms/platform.h"

#define NUM_TASKS 	3
#define MAIN_LOGIC 	0
#define PROCESS_STATE   1
#define PROCESS_STATE_MOVEMENT_COMMANDS   2
static Madara::Knowledge_Engine::Compiled_Expression expressions [NUM_TASKS];

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

// NOTE: Hack to pass the knowledge base to the V-Rep simulation platform.
extern Madara::Knowledge_Engine::Knowledge_Base* m_sim_knowledge;

// Flag to indicate if we want to run an internal test configuration.
bool g_setupTest;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Extra defined function just to force local update settings on global movement variables.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record process_state_movement_commands (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
  return variables.evaluate(expressions[PROCESS_STATE_MOVEMENT_COMMANDS],
    Madara::Knowledge_Engine::Eval_Settings(false, true));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Setup of pre-compiled expressions.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void compile_expressions (Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{
	expressions[PROCESS_STATE_MOVEMENT_COMMANDS] = knowledge.compile
	(
		knowledge.expand_statement
		(
			".movement_command=0;"
			"swarm.movement_command  || device.{.id}.movement_command =>"
			"("
				"(( swarm.movement_command => "
				"("
					".movement_command = swarm.movement_command;"
					"copy_vector('swarm.movement_command.*', '.movement_command.');"
				"))"
				"||"
				"( device.{.id}.movement_command => "
				"(" 
					".movement_command = device.{.id}.movement_command;"
					"copy_vector('device.{.id}.movement_command.*', '.movement_command.');"
				")));"
				"swarm.movement_command = 0; device.{.id}.movement_command = 0;"
			")"
		)
	);
	knowledge.define_function("process_state_movement_commands", process_state_movement_commands);

	expressions[PROCESS_STATE] = knowledge.compile
	(
		knowledge.expand_statement
		(
            "("
                MV_BUSY("{" MV_MY_ID "}") "=" MV_BUSY("{" MV_MY_ID "}") ";"
                MV_MOBILE("{" MV_MY_ID "}") "=" MV_MOBILE("{" MV_MY_ID "}") ";"
            ");"

			"device.{.id}.location=.location;"
			"inflate_coords(.location, '.location');"
			"inflate_coord_array_to_local('device.*');"
			"inflate_coord_array_to_local('region.*');"
			"process_state_movement_commands();"
		)
	);
	
	knowledge.define_function("process_state", expressions[PROCESS_STATE]);
	
	std::string areaMainLogicCall = SMASH::AreaCoverage::get_core_function();
    std::string bridgeMainLogicCall = SMASH::Bridge::get_core_function();
	expressions[MAIN_LOGIC] = knowledge.compile
	(
		"read_sensors ();"
		"process_state ();"
		"(.movement_command"
		"||"
		//"(.needs_bridge => " + bridgeMainLogicCall + " )"
        "(" + bridgeMainLogicCall + " )"
		"||"
		"" + areaMainLogicCall + ");"
		".movement_command => process_movement_commands();"
	);

}

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
    g_setupTest = false;
    handle_arguments (argc, argv);
    g_settings.id = (int) g_id;
    
    // Create the knowledge base.
    Madara::Knowledge_Engine::Knowledge_Base knowledge (g_host, g_settings);
    knowledge.set (".id", (Madara::Knowledge_Record::Integer) g_id);

    // NOTE: Hack to pass the knowledge base to the V-Rep simulation platform.
    m_sim_knowledge = &knowledge;
    
    //knowledge.log_to_file(string("madaralog" + NUM_TO_STR(g_id) + ".txt").c_str(), false);
    //knowledge.evaluate("#log_level(10)");

    // Startup the modules.
    init_platform();
    SMASH::AreaCoverage::initialize(knowledge);
    SMASH::Bridge::initialize(knowledge);
    SMASH::Sensors::initialize(knowledge);
    SMASH::Movement::initialize(knowledge);
    SMASH::Utilities::initialize(knowledge);

	// Setup a simple test since we are not inside actual drones.
    if(g_setupTest)
    {
    	//SMASH::Bridge::setupBridgeTest(knowledge);    
        SMASH::AreaCoverage::setupSearchTest(knowledge);
    }

    // Indicate we start moving.
    knowledge.set(MV_MOBILE("{" MV_MY_ID "}"), 1.0, Madara::Knowledge_Engine::Eval_Settings(true));

    compile_expressions(knowledge);

    // Visual settings to show console output.
	Madara::Knowledge_Engine::Eval_Settings eval_settings;
	eval_settings.pre_print_statement =
        "Drone {" MV_MY_ID "}\n"
		"Total:\t\t{" MV_TOTAL_DEVICES "}\n"
		"Position:\t{" MV_DEVICE_LAT("{.id}") "},{" MV_DEVICE_LON("{.id}") "}\n"
		"Mobile:\t\t{" MV_MOBILE("{.id}") "}\n"
		"Bridging:\t{" MV_BUSY("{.id}") ".bridging}\n"
		"Target pos:\t{" MV_MOVEMENT_TARGET_LAT "},{" MV_MOVEMENT_TARGET_LON "}\n"
        "Search end:\t{.area_coverage.cell.bottom_right.location.latitude},{.area_coverage.cell.bottom_right.location.longitude}\n\n"
		;

    // Until the user presses ctrl+c in this terminal, check for input.
    while (!g_terminated)
    {
        knowledge.evaluate (expressions[MAIN_LOGIC], eval_settings);
        ACE_OS::sleep (1);
    }

    knowledge.print_knowledge ();

    knowledge.close_transport();
    knowledge.clear();

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
    else if (arg1 == "-t" || arg1 == "--test")
    {
        g_setupTest = true;
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
            " [-t|--test]              setup a test configuration of drones\n" \
            "\n",
            argv[0]));
        exit (0);
    }
  }
}
