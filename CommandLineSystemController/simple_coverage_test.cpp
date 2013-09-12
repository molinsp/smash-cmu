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
using std::cerr;
using std::cout;
using std::endl;
#include <sstream>
#include <assert.h>
#include <stdlib.h>

#include "madara/knowledge_engine/Knowledge_Base.h"
#include "madara/utility/Log_Macros.h"
#include "utilities/CommonMadaraVariables.h"
#include "utilities/Position.h"
#include "platforms/comm/comm.h"
#include "ace/Signal.h"

#include <sstream>

std::string coverage_type = "random";

#define NUM_TO_STR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << std::setprecision(10) << x ) ).str()

// Interupt handling.
volatile bool g_terminated = false;
extern "C" void terminate (int)
{
    g_terminated = true;
}

void programSummary(char* arg)
{
    cerr << arg << endl;
    cerr << "  [-i] id" << endl;
    cerr << "  [-d] numDrones" << endl;
    cerr << "  [-n] northern latitude" << endl;
    cerr << "  [-s] southern latitude" << endl;
    cerr << "  [-e] eastern longitude" << endl;
    cerr << "  [-w] western longitude" << endl;
    cerr << "  [-l] MADARA log level" << endl;
    cerr << "  [-t] coverage type" << endl;
    cerr << "  [-st] search stride" << endl;
    exit(-1);
}

void handleArgs(int argc, char** argv, int& id, int& numDrones,
    double& nLat, double& wLong, double& sLat, double& eLong,
    int& logLevel, double& stride)
{
    for(int i = 1; i < argc; ++i)
    {
        string arg(argv[i]);

        if(arg == "-i" && i + 1 < argc)
            sscanf(argv[++i], "%d", &id);
        else if(arg == "-d" && i + 1 < argc)
            sscanf(argv[++i], "%d", &numDrones);
        else if(arg == "-n" && i + 1 < argc)
            sscanf(argv[++i], "%lf", &nLat);
        else if(arg == "-s" && i + 1 < argc)
            sscanf(argv[++i], "%lf", &sLat);
        else if(arg == "-e" && i + 1 < argc)
            sscanf(argv[++i], "%lf", &eLong);
        else if(arg == "-w" && i + 1 < argc)
            sscanf(argv[++i], "%lf", &wLong);
        else if(arg == "-l" && i + 1 < argc)
            sscanf(argv[++i], "%d", &logLevel);
        else if(arg == "-t" && i + 1 < argc)
            coverage_type = argv[++i];
        else if(arg == "-st" && i + 1 < argc)
            sscanf(argv[++i], "%lf", &stride);
        else
            programSummary(argv[0]);
    }
}

int main (int argc, char** argv)
{
    // Set the use of Ctrl+C to terminate.
    ACE_Sig_Action sa ((ACE_SignalHandler) terminate, SIGINT);

    int local_debug_level = -1;
    int id = 0;
    int numDrones = 0;
    double nLat = 0;
    double wLong = 0;
    double sLat = 0;
    double eLong = 0;
    double stride = 0;

    // Handle args
    cout << "Parse args..." << endl;
    handleArgs(argc, argv, id, numDrones, nLat, wLong, sLat, eLong, local_debug_level, stride);
    cout << "  id:           " << id << endl;
    cout << "  numDrones:    " << numDrones << endl;
    cout << "  northern lat: " << nLat << endl;
    cout << "  southern lat: " << sLat << endl;
    cout << "  western lat:  " << wLong << endl;
    cout << "  eastern lat:  " << eLong << endl;
    cout << "  debug level:  " << local_debug_level << endl;
    cout << "  coverage type:  " << coverage_type << endl;
    cout << "  stride:  " << stride << endl;

    // Set the debug level.
    bool enableLogging = false;
    if(local_debug_level != -1)
    {
        MADARA_debug_level = local_debug_level;
        enableLogging = true;
    }
        
    cout << "Init Knowlege Base..." << endl;
    Madara::Knowledge_Engine::Knowledge_Base* knowledge = comm_setup_knowledge_base(id, enableLogging);

    knowledge->set(".id", Madara::Knowledge_Record::Integer(id));

    // Send takeoff command and wait for a bit so the drones take off.
    printf("\nSending takeoff command...\n");
    int takeoffWaitTime = 3;
    knowledge->set(MV_SWARM_MOVE_REQUESTED, MO_TAKEOFF_CMD);
    ACE_OS::sleep (takeoffWaitTime);

    printf("\nInitializing search area...\n");

    // setup search area
    int rectangleType = 0;
    SMASH::Utilities::Position nwCorner(wLong, nLat);
    SMASH::Utilities::Position seCorner(eLong, sLat);
    SMASH::Utilities::Region areaBoundaries(nwCorner, seCorner);
    string sourceRegionIdString = NUM_TO_STR(0);
    string topLeftLocation = areaBoundaries.topLeftCorner.toString();
    string botRightLocation = areaBoundaries.bottomRightCorner.toString();
    knowledge->set(MV_REGION_TYPE(sourceRegionIdString),
        (Madara::Knowledge_Record::Integer) rectangleType,
        Madara::Knowledge_Engine::Eval_Settings(true));
    knowledge->set(MV_REGION_TOPLEFT_LOC(sourceRegionIdString), topLeftLocation,
        Madara::Knowledge_Engine::Eval_Settings(true));
    knowledge->set(MV_REGION_BOTRIGHT_LOC(sourceRegionIdString), botRightLocation,
        Madara::Knowledge_Engine::Eval_Settings(true));
    knowledge->set(MV_TOTAL_DEVICES, Madara::Knowledge_Record::Integer(numDrones),
        Madara::Knowledge_Engine::Eval_Settings(true));
    knowledge->set(MV_MIN_ALTITUDE, 2.0,
        Madara::Knowledge_Engine::Eval_Settings(true));
    knowledge->set(MV_TOTAL_SEARCH_AREAS,
        Madara::Knowledge_Record::Integer(1));

    if(stride != 0)
    {
        knowledge->set("area_coverage.line_width", stride);
    }

    printf("\nSet drones as mobile...\n");
    for(int i = 0; i < numDrones; ++i)
    {
        knowledge->set(MV_MOBILE(NUM_TO_STR(i)), 1.0,
            Madara::Knowledge_Engine::Eval_Settings(true));
        knowledge->set(MV_BUSY(NUM_TO_STR(i)), 0.0,
            Madara::Knowledge_Engine::Eval_Settings(true));
    }

    knowledge->apply_modified();

    printf("\nAssigning search area...\n");
    for(int i = 0; i < numDrones; ++i)
    {
        string droneIdString = NUM_TO_STR(i);
        knowledge->set(MV_ASSIGNED_SEARCH_AREA(droneIdString), Madara::Knowledge_Record::Integer(0), Madara::Knowledge_Engine::Eval_Settings(true));
        knowledge->set(MV_AREA_COVERAGE_REQUESTED(droneIdString), "random", Madara::Knowledge_Engine::Eval_Settings(true));
        knowledge->set(MV_HUMAN_DETECTION_REQUESTED(droneIdString), MO_HUMAN_DETECTION_BASIC, Madara::Knowledge_Engine::Eval_Settings(true));
    }

    knowledge->apply_modified();
    
    printf("\nCommands sent...\n");

    knowledge->print_knowledge();

    while(!g_terminated)
    {
        knowledge->print_knowledge();
        ACE_OS::sleep (1);
    }

    delete knowledge;

    return 0;
}
