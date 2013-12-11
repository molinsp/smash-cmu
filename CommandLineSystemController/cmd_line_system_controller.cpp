/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
 
#include <string>
#include <vector>
#include <iostream>

#include "ace/Signal.h"

#include "Position.h"
#include "MadaraSystemController.h"

// The global object that will be controlling communications through 
MadaraController* g_madaraController;

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

    // General parameters.
    cerr << "  [-l] MADARA log level" << endl;
    cerr << "  [-d] numDrones" << endl;
    cerr << "  [-p] platform (vrep, ar_drone_2)" << endl;
    cerr << "  [-hm] min height" << endl;
    cerr << "  [-hd] height diff" << endl;
    cerr << "  [-r] comm range" << endl;

    // Search request parameters.
    cerr << "  [-n] northern latitude" << endl;
    cerr << "  [-s] southern latitude" << endl;
    cerr << "  [-e] eastern longitude" << endl;
    cerr << "  [-w] western longitude" << endl;
    cerr << "  [-c] coverage type" << endl;
    cerr << "  [-st] search stride" << endl;

    // Bridge request parameters.
    cerr << "  [-bplat] person latitude" << endl;
    cerr << "  [-bplon] person southern longitude" << endl;
    cerr << "  [-bslat] sink latitude" << endl;
    cerr << "  [-bslon] sink longitude" << endl;

    exit(-1);
}

// Loads all available input parameters.
void handleArgs(int argc, char** argv, int& id, int& numDrones, std::string& platform,
    double& nLat, double& wLong, double& sLat, double& eLong,
    int& logLevel, std::string& coverage_type, double& stride, double& minHeight, double& heightDiff,
    double& commRange, double& personLat, double& personLon, double& sinkLat, double& sinkLon)
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
        else if(arg == "-p" && i + 1 < argc)
            platform = argv[++i];
        else if(arg == "-c" && i + 1 < argc)
            coverage_type = argv[++i];
        else if(arg == "-st" && i + 1 < argc)
            sscanf(argv[++i], "%lf", &stride);
        else if(arg == "-hm" && i + 1 < argc)
            sscanf(argv[++i], "%lf", &minHeight);
        else if(arg == "-hd" && i + 1 < argc)
            sscanf(argv[++i], "%lf", &heightDiff);
        else if(arg == "-r" && i + 1 < argc)
            sscanf(argv[++i], "%lf", &commRange);

        else if(arg == "-bplat" && i + 1 < argc)
            sscanf(argv[++i], "%lf", &personLat);
        else if(arg == "-bplon" && i + 1 < argc)
            sscanf(argv[++i], "%lf", &personLon);
        else if(arg == "-bslat" && i + 1 < argc)
            sscanf(argv[++i], "%lf", &sinkLat);
        else if(arg == "-bslon" && i + 1 < argc)
            sscanf(argv[++i], "%lf", &sinkLon);
        else
            programSummary(argv[0]);
    }
}

// Sets a search region and sends a coverage request.
void setAreaCoverageRequest(int& numDrones, double& nLat, double& wLong, double& sLat, double& eLong, 
                            std::string& coverage_type, int& waitForOthers, double& stride, std::string& human_type)
{
    // Set area to cover.
    int searchAreaId = 0 ;
    SMASH::Utilities::Position northWest;
    northWest.latitude = nLat;
    northWest.longitude = wLong;
    SMASH::Utilities::Position southEast;
    southEast.latitude = sLat;
    southEast.longitude = eLong;
    SMASH::Utilities::Region searchRegion(northWest, southEast);
    g_madaraController->setNewSearchArea(searchAreaId, searchRegion);

    // Request the area coverage.
    std::vector<int> droneIds;
    for(int i=0; i<numDrones; i++)
    {
        droneIds.push_back(i);
    }
    g_madaraController->requestAreaCoverage(droneIds, searchAreaId, coverage_type, waitForOthers, stride, human_type);
}

void setBridgeRequest(double& personLat, double& personLon, double& sinkLat, double& sinkLon)
{
    // Set bridge request.
    int bridgeId = 0;
    SMASH::Utilities::Position sourcePos;
    sourcePos.latitude = personLat;
    sourcePos.longitude = personLon;
    SMASH::Utilities::Position sinkPos;
    sinkPos.latitude = sinkLat;
    sinkPos.longitude = sinkLon;
    SMASH::Utilities::Region startRegion(sourcePos, sourcePos);
    SMASH::Utilities::Region endRegion(sinkPos, sinkPos);
    g_madaraController->setupBridgeRequest(bridgeId, startRegion, endRegion);
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
    std::string platform = "";

    // Default parameters.
    double stride = 0.00001;
    double minHeight = 1.5;
    double heightDiff = 0.8;
    double commRange = 4;

    // Search parameters.
    double nLat = 0;
    double wLong = 0;
    double sLat = 0;
    double eLong = 0;
    std::string coverage_type = "random";
    std::string human_type = "basic";
    int waitForOthers = 0;

    // Bridge parameters.
    double personLat = 0;
    double personLon = 0;
    double sinkLat = 0;
    double sinkLon = 0;

    // Load arguments and show them.
    cout << "Parse args..." << endl;
    handleArgs(argc, argv, id, numDrones, platform, nLat, wLong, sLat, eLong, 
      local_debug_level, coverage_type, stride, minHeight, heightDiff, commRange,
      personLat, personLon, sinkLat, sinkLon);
    cout << "  id:           " << id << endl;
    cout << "  numDrones:    " << numDrones << endl;
    cout << "  platform :    " << platform << endl;
    cout << "  northern lat: " << nLat << endl;
    cout << "  southern lat: " << sLat << endl;
    cout << "  western lon:  " << wLong << endl;
    cout << "  eastern lon:  " << eLong << endl;
    cout << "  debug level:  " << local_debug_level << endl;
    cout << "  coverage type:" << coverage_type << endl;
    cout << "  stride:       " << stride << endl;
    cout << "  min height:   " << minHeight << endl;
    cout << "  height diff:  " << heightDiff << endl;
    cout << "  commRange:    " << commRange << endl;

    cout << "  person lat: " << personLat << endl;
    cout << "  person lon: " << personLon << endl;
    cout << "  sink lat:  " << sinkLat << endl;
    cout << "  sink lon:  " << sinkLon << endl;

    // Set the debug level.
    bool enableLogging = false;
    if(local_debug_level != -1)
    {
        MADARA_debug_level = local_debug_level;
        enableLogging = true;
    }

    // Setup the knowledge base and basic parameters.
    cout << "Init Knowlege Base..." << endl;
    g_madaraController = new MadaraController(id, platform);

    // Disseminating basic parameters, mandatory and optional ones.
    // NOTE: coverage tracking is disabled here by default withe the 0,0 passed
    // as final arguments. This could be added as a command line parameter.
    printf("\nSetting up basic parameters...\n");
    g_madaraController->updateGeneralParameters(numDrones, commRange, minHeight, heightDiff, 0, 0, 0);

    // Also send takeoff command.
    printf("\nSending takeoff command, and waiting for drones to take off...\n");
    g_madaraController->sendTakeoffCommand();
    int inBetweenTime = 10;
    ACE_OS::sleep (inBetweenTime);

    // Set area coverage, if requested.
    if(nLat != 0 && wLong != 0 && sLat != 0 && eLong != 0)
    {
        printf("\nSending area coverage request.\n");
        setAreaCoverageRequest(numDrones, nLat, wLong, sLat, eLong, coverage_type, waitForOthers, stride, human_type);

        printf("\nWaiting for area coverage request to kick in....\n");
        ACE_OS::sleep (inBetweenTime);
    }

    // Set bridge buidling, if requested.
    if(personLat != 0 && personLon != 0 && sinkLat != 0 && sinkLon != 0)
    {
        printf("\nSending bridge request.\n");
        setBridgeRequest(personLat, personLon, sinkLat, sinkLon);
    }
    
    printf("\nCommands sent, entering loop to show status of knowledge base.\n");
    while(!g_terminated)
    {
        g_madaraController->printKnowledge();
        ACE_OS::sleep (1);
    }

    // Simply delete the knowledge base when the program terminates.
    delete g_madaraController;
    return 0;
}
