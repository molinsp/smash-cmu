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

#include <sstream>

#define NUM_TO_STR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << std::setprecision(10) << x ) ).str()

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
    exit(-1);
}

void handleArgs(int argc, char** argv, int& id, int& numDrones,
    double& nLat, double& wLong, double& sLat, double& eLong,
    int& logLevel)
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
        else
            programSummary(argv[0]);
    }
}

int main (int argc, char** argv)
{
    int local_debug_level = 0;
    int id = 0;
    int numDrones = 0;
    double nLat = 0;
    double wLong = 0;
    double sLat = 0;
    double eLong = 0;

    // Handle args
    cout << "Parse args..." << endl;
    handleArgs(argc, argv, id, numDrones, nLat, wLong, sLat, eLong, local_debug_level);
    cout << "  id:           " << id << endl;
    cout << "  numDrones:    " << numDrones << endl;
    cout << "  northern lat: " << nLat << endl;
    cout << "  southern lat: " << sLat << endl;
    cout << "  western lat:  " << wLong << endl;
    cout << "  eastern lat:  " << eLong << endl;
    cout << "  debug level:  " << local_debug_level << endl;
        
    cout << "Init Knowlege Base..." << endl;
    Madara::Knowledge_Engine::Knowledge_Base* knowledge = comm_setup_knowledge_base(id, true);

    knowledge->set(".id", Madara::Knowledge_Record::Integer(id));

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
    knowledge->set(MV_TOTAL_SEARCH_AREAS, Madara::Knowledge_Record::Integer(1));

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


    printf("\nExiting...\n");

    knowledge->print_knowledge();

    delete knowledge;

    return 0;
}
