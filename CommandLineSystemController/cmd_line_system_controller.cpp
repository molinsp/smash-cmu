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
#include "utilities/string_utils.h"
#include "platforms/comm/comm.h"
#include "ace/Signal.h"

#include <sstream>

Madara::Knowledge_Engine::Knowledge_Base* knowledge;

// Interupt handling.
volatile bool g_terminated = false;
extern "C" void terminate (int)
{
    g_terminated = true;
}

// Shows a summary of the parameters available for this program.
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
    cerr << "  [-hm] min height" << endl;
    cerr << "  [-hd] height diff" << endl;
    cerr << "  [-r] comm range" << endl;
    exit(-1);
}

// Loads all available input parameters.
void handleArgs(int argc, char** argv, int& id, int& numDrones,
    double& nLat, double& wLong, double& sLat, double& eLong,
    int& logLevel, std::string& coverage_type, double& stride, double& minHeight, double& heightDiff,
    double& commRange)
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
        else if(arg == "-hm" && i + 1 < argc)
            sscanf(argv[++i], "%lf", &minHeight);
        else if(arg == "-hd" && i + 1 < argc)
            sscanf(argv[++i], "%lf", &heightDiff);
        else if(arg == "-r" && i + 1 < argc)
            sscanf(argv[++i], "%lf", &commRange);
        else
            programSummary(argv[0]);
    }
}

// Sets a search region and sends a coverage request.
void setAreaCoverageRequest(int& numDrones, double& nLat, double& wLong, double& sLat, double& eLong, std::string& coverage_type, double& stride)
{
    printf("\nInitializing search area...\n");

    // Setup search area.
    int rectangleType = 0;
    SMASH::Utilities::Position nwCorner(wLong, nLat);
    SMASH::Utilities::Position seCorner(eLong, sLat);
    SMASH::Utilities::Region areaBoundaries(nwCorner, seCorner);
    string sourceRegionIdString = NUM_TO_STR(0);

    string topLeftLocation = areaBoundaries.northWest.toString();
    string botRightLocation = areaBoundaries.southEast.toString();

    knowledge->set(MV_REGION_TYPE(sourceRegionIdString),
        (Madara::Knowledge_Record::Integer) rectangleType,
        Madara::Knowledge_Engine::Eval_Settings(true));
    knowledge->set(MV_REGION_TOPLEFT_LOC(sourceRegionIdString), topLeftLocation,
        Madara::Knowledge_Engine::Eval_Settings(true));
    knowledge->set(MV_REGION_BOTRIGHT_LOC(sourceRegionIdString), botRightLocation,
        Madara::Knowledge_Engine::Eval_Settings(true));
    knowledge->set(MV_TOTAL_SEARCH_AREAS,
        Madara::Knowledge_Record::Integer(1));

    if(stride != 0)
    {
        knowledge->set(MV_AREA_COVERAGE_LINE_WIDTH, stride);
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
}

// Main entry point.
int main (int argc, char** argv)
{
    // Set the use of Ctrl+C to terminate.
    ACE_Sig_Action sa ((ACE_SignalHandler) terminate, SIGINT);

    // Variables to hold input parameters.
    int local_debug_level = -1;
    int id = 0;
    int numDrones = 0;
    double nLat = 0;
    double wLong = 0;
    double sLat = 0;
    double eLong = 0;
    std::string coverage_type = "random";
    double stride = 0;
    double minHeight = 0;
    double heightDiff = 0;
    double commRange = 0;

    // Load arguments and show them.
    cout << "Parse args..." << endl;
    handleArgs(argc, argv, id, numDrones, nLat, wLong, sLat, eLong, local_debug_level, coverage_type, stride, minHeight, heightDiff, commRange);
    cout << "  id:           " << id << endl;
    cout << "  numDrones:    " << numDrones << endl;
    cout << "  northern lat: " << nLat << endl;
    cout << "  southern lat: " << sLat << endl;
    cout << "  western lat:  " << wLong << endl;
    cout << "  eastern lat:  " << eLong << endl;
    cout << "  debug level:  " << local_debug_level << endl;
    cout << "  coverage type:" << coverage_type << endl;
    cout << "  stride:       " << stride << endl;
    cout << "  min height:   " << minHeight << endl;
    cout << "  height diff:  " << heightDiff << endl;
    cout << "  commRange:    " << commRange << endl;

    // Set the debug level.
    bool enableLogging = false;
    if(local_debug_level != -1)
    {
        MADARA_debug_level = local_debug_level;
        enableLogging = true;
    }
    
    // Setup the knowledge base.
    cout << "Init Knowlege Base..." << endl;
    knowledge = comm_setup_knowledge_base(id, enableLogging);

    // Setup basic parameters, mandatory and optional ones.
    printf("\nSetting up basic parameters...\n");
    knowledge->set(".id", Madara::Knowledge_Record::Integer(id),
            Madara::Knowledge_Engine::Eval_Settings(true));
    if(numDrones != 0)
    {
        knowledge->set(MV_TOTAL_DEVICES, Madara::Knowledge_Record::Integer(numDrones),
            Madara::Knowledge_Engine::Eval_Settings(true));
    }
    if(minHeight != 0)
    {
        knowledge->set(MV_MIN_ALTITUDE, minHeight,
            Madara::Knowledge_Engine::Eval_Settings(true));
    }
    if(heightDiff != 0)
    {
        knowledge->set(MV_AREA_COVERAGE_HEIGHT_DIFF, heightDiff,
            Madara::Knowledge_Engine::Eval_Settings(true));
    }
    if(commRange != 0)
    {
        knowledge->set(MV_COMM_RANGE, commRange,
            Madara::Knowledge_Engine::Eval_Settings(true));
    }
    knowledge->apply_modified();

    // Send takeoff command and wait for a bit so the drones take off.
    printf("\nSending takeoff command, and waiting for drones to take off...\n");
    int takeoffWaitTime = 3;
    knowledge->set(MV_SWARM_MOVE_REQUESTED, MO_TAKEOFF_CMD);
    ACE_OS::sleep (takeoffWaitTime);

    // Set area coverage.
    setAreaCoverageRequest(numDrones, nLat, wLong, sLat, eLong, coverage_type, stride);
    
    printf("\nCommands sent, entering loop to show status of knowledge base.\n");
    while(!g_terminated)
    {
        knowledge->print_knowledge();
        ACE_OS::sleep (1);
    }

    // Simply delete the knowledge base when the program terminates.
    delete knowledge;
    return 0;
}
