/********************************************************************* 
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

/*********************************************************************
* area_coverage_module.cpp - Defines the manager for area coverage.
*********************************************************************/

#include "area_coverage_module.h"
#include "utilities/CommonMadaraVariables.h"
#include "AreaCoverage.h"
#include "SnakeAreaCoverage.h"
#include "RandomAreaCoverage.h"
#include "InsideOutAreaCoverage.h"

#include <vector>
#include <map>
#include <string>

using namespace SMASH::AreaCoverage;
using namespace SMASH::Utilities;
using std::string;

// Default values for various parameters, loaded only when the module is loaded, and locally. They will be overriden if a System Controller changes them.
#define DEFAULT_SEARCH_LINE_OFFSET_DEGREES      0.0000100   // Margin (in degrees) to use when moving to another column or line of search. Should be similar to the view range of a drone.
#define DEFAULT_ALTITUDE_DIFFERENCE             0.8         // The amount of vertical space (in meters) to leave between drones.
#define DEFAULT_MIN_HEIGHT                      1.5         // The default minimum height (in meters) to use when choosing heights for search coverage.

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Madara Variable Definitions
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions.
#define MF_MAIN_LOGIC                   "area_coverage_doAreaCoverage"              // Function that checks if there is area coverage to be done, and does it.
#define MF_INIT_SEARCH_CELL             "area_coverage_initSearchCell"              // Initializes the cell that we will be searching.
#define MF_ASSIGN_ALT_AND_REQUEST_MOVE  "area_coverage_calcAndMoveToAlt"            // Assigns an altitude and requests movement to that altitude.
#define MF_NEXT_TARGET_REACHED          "area_coverage_checkNextTargetReached"      // Checks if the current target has been reached.
#define MF_FINAL_TARGET_REACHED         "area_coverage_checkFinalTargetReached"     // Checks if the final target has been reached.
#define MF_ALTITUDE_REACHED             "area_coverage_checkAltitudeReached"        // Checks if the assigned altitude has been reached.
#define MF_SET_NEW_TARGET               "area_coverage_setNewTarget"                // Sets the next target.
#define MF_UPDATE_AVAILABLE_DRONES      "area_coverage_updateAvailableDrones"       // Function that checks the amount and positions of drones ready for covering.
#define MF_SET_NEW_COVERAGE             "area_coverage_setNewCoverage"              // Sets the new coverage to use when a coverage reaches its final target
#define MF_ALL_DRONES_READY             "area_coverage_checkAllDronesReady"         // Function that checks if all drones in my area are ready for the next target/waypoint.

// Internal variables.
#define MV_CELL_INITIALIZED             ".area_coverage.cell.initialized"                       // Flag to check if we have initialized our cell in the search area.
#define MV_NEXT_TARGET_LAT              ".area_coverage.target.location.latitude"               // The latitude of the next target location in our search pattern.
#define MV_NEXT_TARGET_LON              ".area_coverage.target.location.longitude"              // The longitude of the next target location in our search pattern.
#define MV_AVAILABLE_DRONES_IN_MY_AREA  ".area_coverage.my_area.devices.available"              // The amount of available drones in my search area.
#define MV_MY_POS_IN_MY_AREA            ".area_coverage.my_area.my_position"                    // The position of the device in the list of available drones in my area.
#define MV_MY_CELL_TOP_LEFT_LAT         ".area_coverage.cell.top_left.location.latitude"        // The latitude of the top left corner of the cell I am searching.
#define MV_MY_CELL_TOP_LEFT_LON         ".area_coverage.cell.top_left.location.longitude"       // The longitude of the top left corner of the cell I am searching.
#define MV_MY_CELL_BOT_RIGHT_LAT        ".area_coverage.cell.bottom_right.location.latitude"    // The latitude of the bottom right corner of the cell I am searching.
#define MV_MY_CELL_BOT_RIGHT_LON        ".area_coverage.cell.bottom_right.location.longitude"   // The longitude of the bottom right corner of the cell I am searching.

#define MV_INITIAL_HEIGHT_REACHED       ".area_coverage.initial_height_reached"                 // Variable to check if the initial height has been reached at least once.
#define MV_FIRST_TARGET_SELECTED        ".area_coverage.first_target_selected"                  // Variable to check if the first target has been selected.

#define MV_READY_DRONES_AMOUNT          ".area_coverage.my_area.devices.waiting"                // The amount of drones in my area in waiting  state.
#define MV_LAST_REACHED_TARGET          ".area_coverage.my_area.current_target"                 // The number of the current target, just goes up as new targets are chosen.
#define MV_WAITING                      ".area_coverage.my_area.waiting"                        // 1 if I am currently waiting, 0 otherwise.

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private variables.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Define the ids for the expressions that will exist to accomplish different aspects of area coverage.
enum AreaCoverageMadaraExpressionId 
{
    // Expression to call function to update the positions of the drones available for coverage.
    ACE_FIND_AVAILABLE_DRONES_POSITIONS,
};

// Map of Madara expressions used in bridge building.
static std::map<AreaCoverageMadaraExpressionId, Madara::Knowledge_Engine::Compiled_Expression> m_expressions;

// Stores information about the area coverage state.
static AreaCoverage* m_coverageAlgorithm;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private function declarations. Static linkage so they are not seen oustide of the module.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void defineFunctions(Madara::Knowledge_Engine::Knowledge_Base &knowledge);
static void compileExpressions(Madara::Knowledge_Engine::Knowledge_Base &knowledge);

static Madara::Knowledge_Record madaraInitSearchCell (Madara::Knowledge_Engine::Function_Arguments &args,
    Madara::Knowledge_Engine::Variables &variables);
static Madara::Knowledge_Record madaraSetNewTarget (Madara::Knowledge_Engine::Function_Arguments &args,
    Madara::Knowledge_Engine::Variables &variables);
static Madara::Knowledge_Record madaraReachedFinalTarget(Madara::Knowledge_Engine::Function_Arguments &args,
    Madara::Knowledge_Engine::Variables &variables);
static Madara::Knowledge_Record madaraSetNewCoverage(Madara::Knowledge_Engine::Function_Arguments &args,
    Madara::Knowledge_Engine::Variables &variables);
static Madara::Knowledge_Record madaraCalculateAndMoveToAltitude (Madara::Knowledge_Engine::Function_Arguments &args,
    Madara::Knowledge_Engine::Variables &variables);
static Madara::Knowledge_Record madaraAltitudeReached (Madara::Knowledge_Engine::Function_Arguments &args,
    Madara::Knowledge_Engine::Variables &variables);
static Region invertRegionIfRequired(const Region& sourceRegion);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Initializer, gets the refence to the knowledge base and compiles expressions.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SMASH::AreaCoverage::AreaCoverageModule::initialize(Madara::Knowledge_Engine::Knowledge_Base &knowledge)
{
    knowledge.print("SMASH::AreaCoverage::initialize...\n");

    // Defines internal and external functions.
    defineFunctions(knowledge);

    // Initialize common search parameters with default values, locally.
    knowledge.set(MV_AREA_COVERAGE_LINE_WIDTH, DEFAULT_SEARCH_LINE_OFFSET_DEGREES,
                  Madara::Knowledge_Engine::Eval_Settings(true, true));
    knowledge.set(MV_AREA_COVERAGE_HEIGHT_DIFF, DEFAULT_ALTITUDE_DIFFERENCE,
                  Madara::Knowledge_Engine::Eval_Settings(true, true));
    knowledge.set(MV_MIN_ALTITUDE, DEFAULT_MIN_HEIGHT,
                  Madara::Knowledge_Engine::Eval_Settings(true, true));
    knowledge.set(MV_SEARCH_WAIT, 0.0,
                  Madara::Knowledge_Engine::Eval_Settings(true, true)); // No wait by default.

    // Registers all default expressions, to have them compiled for faster access.
    compileExpressions(knowledge);
    knowledge.print("leaving SMASH::AreaCoverage::initialize...\n");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Cleanup, cleans up the dynamically allocated search algorithm
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SMASH::AreaCoverage::AreaCoverageModule::cleanup(Madara::Knowledge_Engine::Knowledge_Base &knowledge)
{
    // should only need to delete the coverage algorithm
    delete m_coverageAlgorithm;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gets the main logic to be run. This returns a function call that can be included in another block of logic.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string SMASH::AreaCoverage::AreaCoverageModule::get_core_function()
{
    return MF_MAIN_LOGIC "()";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers functions with Madara.
// ASSUMPTION: Drone IDs are continuous, starting from 0.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void defineFunctions(Madara::Knowledge_Engine::Knowledge_Base &knowledge)
{
    // Function that can be included in main loop of another method to introduce area coverage.
    knowledge.define_function(MF_MAIN_LOGIC, 
        // If there is any string value for the requested area coverage, and we are mobile and not busy, do area coverage.
        "(" MV_AREA_COVERAGE_REQUESTED("{" MV_MY_ID "}") " && " MV_MOBILE("{" MV_MY_ID "}") " && (!" MV_BUSY("{" MV_MY_ID "}") ")" ")" 
        " => "
            "("
                "("
                    // Only do the following if the cell we will be searching in has alerady been set up, and we have reached at least once our assigned height.
                    "(" MV_CELL_INITIALIZED " && (" MV_INITIAL_HEIGHT_REACHED " || " MV_IS_AT_ASSIGNED_ALTITUDE ") )" 
                    " => " 
                        "("
                            // We only need to wait for the assigned altitude to be reached the first time; we don't care here if it changes its altitude later.
                            "(" MV_INITIAL_HEIGHT_REACHED " = 1);"

                            // If wait is enabled, propagate our current target continously once we have reached our target, so in case 
                            // others are waiting and there are packets lost, they will get this eventually.
                            "("
                                "(" MV_SEARCH_WAIT ")"
                                " => "
                                    "(" MV_CURRENT_COVERAGE_TARGET("{" MV_MY_ID "}") " = " MV_LAST_REACHED_TARGET ");"
                            ");"

                            // Check if we have reached our next target.
                            "("
                                "(" MV_FIRST_TARGET_SELECTED " && " MV_REACHED_GPS_TARGET ")"
                                " => "
                                    "("
                                        // Check if we are not waiting yet, to update data and start waiting.  
                                        "(" MV_WAITING " == 0)"
                                        " => "
                                            "("
                                                // Update the last target that has been reached, only before we start waiting.
                                                "(++" MV_LAST_REACHED_TARGET ");"
                                                "(" MV_CURRENT_COVERAGE_TARGET("{" MV_MY_ID "}") " = " MV_LAST_REACHED_TARGET ");"

                                                // Indicate we are now in waiting mode.
                                                "(" MV_WAITING " = 1);"
                                            ");"

                                        // Only look for a new target if we have not reached the last target, and all other drones have reached their current target.
                                        "( !(" MF_FINAL_TARGET_REACHED "()) && ((!" MV_SEARCH_WAIT ") || " MF_ALL_DRONES_READY "()) )"
                                        " => "
                                            "("
                                                "(" MF_SET_NEW_TARGET  "());"
                                                "(" MV_WAITING " = 0);"
                                            ")"
                                    ");"
                            ");"

                            // Check if we are initializing the search; if so, get a new target.
                            "("
                                "(" MV_FIRST_TARGET_SELECTED " == 0)"
                                " => "
                                    "("
                                        // Get a new target, the first one.
                                        "(" MV_FIRST_TARGET_SELECTED " = 1);"
                                        "(" MV_LAST_REACHED_TARGET " = 0);"
                                        "(" MV_CURRENT_COVERAGE_TARGET("{" MV_MY_ID "}") " = " MV_LAST_REACHED_TARGET ");"
                                        "(" MF_SET_NEW_TARGET "());" 
                                        "(" MV_WAITING " = 0);"
                                    ")"
                            ");"
                        ");"
                ");"
                "("
                    // If we haven't defined our cell yet, do it. 
                    "(!" MV_CELL_INITIALIZED ")"
                    " => "
                        "("                            
                            "(" MF_INIT_SEARCH_CELL "() ) " 
                            " => "
                                "("
                                    // Indicate that we have initialized the cell, and request to move to our assigned height.
                                    "(" MV_CELL_INITIALIZED " = 1);"
                                    "(" MV_FIRST_TARGET_SELECTED " = 0);"
                                    "(" MF_ASSIGN_ALT_AND_REQUEST_MOVE "() );"
                                ");"
                        ");"
                ");"
            ");"
    );

    // Function to update the amount and positions of drones available for covering a specific area.
    knowledge.define_function(MF_UPDATE_AVAILABLE_DRONES, 
        // Set available drones to 0 and disregard its return (choose right).
        MV_AVAILABLE_DRONES_IN_MY_AREA " = 0 ;>"

        // Loop over all the drones to find the available ones.
        ".i[0->" MV_TOTAL_DEVICES ")"
        "("
            // A drone is available if it is mobile and it is not busy, and it is assigned to the same area as I am.
            "(" MV_MOBILE("{.i}") " && (!" MV_BUSY("{.i}") ") && (" MV_ASSIGNED_SEARCH_AREA("{.i}") " == " MV_ASSIGNED_SEARCH_AREA("{" MV_MY_ID "}") "))"
            " => "
                "("
                    // If so, increase the amount of available drones, and and store what is my position among the available drones.
                    "((.i == .id) => (" MV_MY_POS_IN_MY_AREA " = .i));"
                    "++" MV_AVAILABLE_DRONES_IN_MY_AREA ";"
                ");"
        ");"
    );

    // Function to check if all drones covering a specific area are waiting for the next target.
    knowledge.define_function(MF_ALL_DRONES_READY, 
        // Update the number of available drones.
        MF_UPDATE_AVAILABLE_DRONES "();>"

        // Set waiting drones to 0 and disregard its return (choose right).
        MV_READY_DRONES_AMOUNT " = 0 ;>"

        // Loop over all the drones to find the how many have reached the target.
        "("
            ".i[0->" MV_TOTAL_DEVICES ")"
            "("
                // A drone is available if it is mobile and it is not busy, and it is assigned to the same area as I am.
                "(" MV_MOBILE("{.i}") " && (!" MV_BUSY("{.i}") ") && (" MV_ASSIGNED_SEARCH_AREA("{.i}") " == " MV_ASSIGNED_SEARCH_AREA("{" MV_MY_ID "}") "))"
                " => "
                    "("
                        // If this drone is available, check if it has reached the same target I was moving towards (or if it already started moving towards the next).
                        "((" MV_CURRENT_COVERAGE_TARGET("{.i}") " >= " MV_LAST_REACHED_TARGET ") && (" MV_LAST_REACHED_TARGET "> 0 ))"
                        " => "
                            // If it has reached the target, update the counter.
                            "(++" MV_READY_DRONES_AMOUNT ";)"
                    ");"
            ");"
        ");>"

        // Return 1 if all expected drones have reached the target, 0 otherwise.
        "(" MV_READY_DRONES_AMOUNT " == " MV_AVAILABLE_DRONES_IN_MY_AREA ");"
    );

    // Checks if the final target of the area coverage strategy has been reached.
    knowledge.define_function(MF_FINAL_TARGET_REACHED, madaraReachedFinalTarget);

    // Sets the new coverage after reaching final target
    knowledge.define_function(MF_SET_NEW_COVERAGE, madaraSetNewCoverage);

    // Function that can be included in main loop of another method to introduce area coverage.
    knowledge.define_function(MF_INIT_SEARCH_CELL, madaraInitSearchCell);

    // Function that can be included in main loop of another method to introduce area coverage.
    knowledge.define_function(MF_ASSIGN_ALT_AND_REQUEST_MOVE, madaraCalculateAndMoveToAltitude);

    // Function that can be included in main loop of another method to introduce area coverage.
    knowledge.define_function(MF_SET_NEW_TARGET, madaraSetNewTarget);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Compiles all expressions to be used by this class.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void compileExpressions(Madara::Knowledge_Engine::Knowledge_Base &knowledge)
{
    // Expression to update the list of available drones, simply calls the predefined function.
    m_expressions[ACE_FIND_AVAILABLE_DRONES_POSITIONS] = knowledge.compile(
        MF_UPDATE_AVAILABLE_DRONES "();"
    );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Selects an area coverage algorithm given the string from the madara variable
 *
 * @param algorithm  string determining which algorithm to select
 * @param variables  access to the knowledge base.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
AreaCoverage* selectAreaCoverageAlgorithm(string algorithm, Madara::Knowledge_Engine::Variables &variables)
{
    AreaCoverage* coverageAlgorithm = NULL;
    if(algorithm == MO_AREA_COVERAGE_RANDOM)
    {
        // Use my index in the search area plus the current time as the seed for the random algorithm.
        int myIndexInArea = (int) variables.get(MV_MY_POS_IN_MY_AREA).to_integer();
        int seed = time(NULL) + myIndexInArea;
        coverageAlgorithm = new RandomAreaCoverage(seed);
    }
    else if(algorithm == MO_AREA_COVERAGE_SNAKE)
    {
        double searchLineOffset = variables.get(MV_AREA_COVERAGE_LINE_WIDTH).to_double();
        coverageAlgorithm = new SnakeAreaCoverage(Region::NORTH_WEST, searchLineOffset);
    }
    else if(algorithm == MO_AREA_COVERAGE_INSIDEOUT)
    {
        double searchLineOffset = variables.get(MV_AREA_COVERAGE_LINE_WIDTH).to_double();
        coverageAlgorithm = new InsideOutAreaCoverage(searchLineOffset);
    }
    else
    {
        string err = "selectAreaCoverageAlgorithm(algo = \"";
        err += algorithm;
        err += "\") failed to find match\n";
        printf("%s", err.c_str());
    }

    return coverageAlgorithm;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
* Method that invocates the functionality of defining our cell to search, which will be called from Madara when required.
* Will be called from an external Madara function.
* @return  Returns true (1) if it can calculate the cell, false (0) if it couldn't for some reason.
**/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record madaraInitSearchCell (Madara::Knowledge_Engine::Function_Arguments &args,
    Madara::Knowledge_Engine::Variables &variables)
{
    // Find all the available drones, called here to ensure atomicity and we have the most up to date data.
    variables.evaluate(m_expressions[ACE_FIND_AVAILABLE_DRONES_POSITIONS],
        Madara::Knowledge_Engine::Knowledge_Update_Settings(true, false));

    // Obtain drone information
    int availableDrones = (int) variables.get(MV_AVAILABLE_DRONES_IN_MY_AREA).to_integer();
    int myIndexInList = (int) variables.get(MV_MY_POS_IN_MY_AREA).to_integer();

    // Obtain the region details where we will be searching.
    std::string myAssignedSearchArea = variables.get(variables.expand_statement(MV_ASSIGNED_SEARCH_AREA("{" MV_MY_ID "}"))).to_string();
    std::string myAssignedSearchRegion = variables.get(MV_SEARCH_AREA_REGION(myAssignedSearchArea)).to_string();
    double nwLat = variables.get(MV_REGION_TOPLEFT_LAT(myAssignedSearchRegion)).to_double();
    double nwLon = variables.get(MV_REGION_TOPLEFT_LON(myAssignedSearchRegion)).to_double();
    double seLat = variables.get(MV_REGION_BOTRIGHT_LAT(myAssignedSearchRegion)).to_double();
    double seLon = variables.get(MV_REGION_BOTRIGHT_LON(myAssignedSearchRegion)).to_double();
    Region searchArea = Region(Position(nwLon, nwLat), Position(seLon, seLat));

    // Swap sides of the region if it was incorrectly setup originally in the Madara variables.
    searchArea = invertRegionIfRequired(searchArea);

    // Calculate the actual cell I will be covering.
    string algo = variables.get(variables.expand_statement(MV_AREA_COVERAGE_REQUESTED("{" MV_MY_ID "}"))).to_string();
    m_coverageAlgorithm = selectAreaCoverageAlgorithm(algo, variables);
    if(m_coverageAlgorithm != NULL)
    {
        // Calculate the cell I will be working on.
        Region* myCell = m_coverageAlgorithm->initialize(searchArea, myIndexInList, availableDrones);

        if(myCell != NULL)
        {
            // Store this cell in Madara.
            variables.set(MV_MY_CELL_TOP_LEFT_LAT, (myCell->northWest.latitude));
            variables.set(MV_MY_CELL_TOP_LEFT_LON, (myCell->northWest.longitude));
            variables.set(MV_MY_CELL_BOT_RIGHT_LAT, (myCell->southEast.latitude));
            variables.set(MV_MY_CELL_BOT_RIGHT_LON, (myCell->southEast.longitude));
    
            return Madara::Knowledge_Record(1.0);
        }
    }

    // If we couldn't generate our cell for some reason, the function was not successful.
    return Madara::Knowledge_Record(0.0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
* Method that sets up the altitude of a drone, based on is index in the search area (which will be 0 if it has not
* been set).
* @return  Returns true (1) always.
**/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record madaraCalculateAndMoveToAltitude (Madara::Knowledge_Engine::Function_Arguments &args,
    Madara::Knowledge_Engine::Variables &variables)
{
    // Calculate and store my assigned or default altitude based on my index on the list.
    // (If the search area has not been initialized, all drones will end up at the same, default height).
    double minAltitude = variables.get(MV_MIN_ALTITUDE).to_double();
    int myIndexInList = (int) variables.get(MV_MY_POS_IN_MY_AREA).to_integer();
    double altitudeDifference = variables.get(MV_AREA_COVERAGE_HEIGHT_DIFF).to_double();
    double myDefaultAltitude = minAltitude + altitudeDifference * (double) myIndexInList;
    variables.set(variables.expand_statement(MV_ASSIGNED_ALTITUDE("{" MV_MY_ID "}")), myDefaultAltitude);

    // Send the command to go to this altitude.
    variables.set(MV_MOVEMENT_TARGET_ALT, myDefaultAltitude, Madara::Knowledge_Engine::Eval_Settings(true));
    variables.set(MV_MOVEMENT_REQUESTED, std::string(MO_MOVE_TO_ALTITUDE_CMD));
    printf("Moving to altitude %f!\n", myDefaultAltitude);

    return Madara::Knowledge_Record(1.0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
* Method that sets the next waypoint target in our search pattern, and tells the movement module to move there.
* Will be called from an external Madara function.
* @return  Returns true (1) always.
**/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record madaraSetNewTarget (Madara::Knowledge_Engine::Function_Arguments &args,
    Madara::Knowledge_Engine::Variables &variables)
{
    // Get the next target.
    Position nextTarget = m_coverageAlgorithm->getNextTargetLocation();

    // Update the drone status for the next target.
    variables.set(MV_NEXT_TARGET_LAT, (nextTarget.latitude),
        Madara::Knowledge_Engine::Knowledge_Update_Settings(false, false));
    variables.set(MV_NEXT_TARGET_LON, (nextTarget.longitude),
        Madara::Knowledge_Engine::Knowledge_Update_Settings(false, false));

    // Set the movement command for the movement module.
    variables.set(MV_MOVEMENT_TARGET_LAT, (nextTarget.latitude),
        Madara::Knowledge_Engine::Knowledge_Update_Settings(false, false));
    variables.set(MV_MOVEMENT_TARGET_LON, (nextTarget.longitude),
        Madara::Knowledge_Engine::Knowledge_Update_Settings(false, false));
    variables.set(MV_MOVEMENT_REQUESTED, std::string(MO_MOVE_TO_GPS_CMD));

    return Madara::Knowledge_Record(1.0);
}

/**
* Determines if this algorithm has ended.
* @return  Returns true (1) if ended, else returns false (0)
**/
Madara::Knowledge_Record madaraReachedFinalTarget(
    Madara::Knowledge_Engine::Function_Arguments &args,
    Madara::Knowledge_Engine::Variables &variables)
{
    // change to new coverage algorithm
    if(m_coverageAlgorithm->isTargetingFinalWaypoint())
    {
        printf("IS targeting final waypoint.\n");
        return Madara::Knowledge_Record(1.0);
    }
    printf("IS NOT targeting final waypoint.\n");
    return Madara::Knowledge_Record(0.0);
}

/**
 * Sets the new coverage to use
 * @return  Returns true (1) if new coverage selected, else returns false (0)
 */
Madara::Knowledge_Record madaraSetNewCoverage(Madara::Knowledge_Engine::Function_Arguments &args,
    Madara::Knowledge_Engine::Variables &variables)
{
    printf("Setting new coverage");
    string next = variables.get(variables.expand_statement(MV_NEXT_AREA_COVERAGE_REQUEST("{" MV_MY_ID "}"))).to_string();
    AreaCoverage* temp = m_coverageAlgorithm;
    Region searchArea(*(m_coverageAlgorithm->getSearchRegion()));
    m_coverageAlgorithm = AreaCoverage::continueCoverage(m_coverageAlgorithm, next);
    delete temp;
    if(m_coverageAlgorithm != NULL)
    {
        Region* myCell = m_coverageAlgorithm->initialize(searchArea);
    
        if(myCell != NULL)
        {
            // Store this cell in Madara.
            variables.set(MV_MY_CELL_TOP_LEFT_LAT, (myCell->northWest.latitude));
            variables.set(MV_MY_CELL_TOP_LEFT_LON, (myCell->northWest.longitude));
            variables.set(MV_MY_CELL_BOT_RIGHT_LAT, (myCell->southEast.latitude));
            variables.set(MV_MY_CELL_BOT_RIGHT_LON, (myCell->southEast.longitude));
    
            return Madara::Knowledge_Record(1.0);
        }
    }
    // If we couldn't generate our cell for some reason, the function was not successful.
    return Madara::Knowledge_Record(0.0);
}

////////////////////////////////////////////////////////////////////////////////////////////
// Returns the same region if its coordinates actually corresponded to the cardinal points.
// If not, if inverts either the north-south latitudes, west-east longitudes, or both.
// The end result is a region where the latitudes and longitudes match the names of the
// object fields.
////////////////////////////////////////////////////////////////////////////////////////////
Region invertRegionIfRequired(const Region& sourceRegion)
{
    // Start by assuming no inversion if required.
    Region cleanedRegion(sourceRegion);

    // Check if we need a north-south latitude inversion.
    if(sourceRegion.northWest.latitude < sourceRegion.southEast.latitude)
    {
        // If the south latitude is greater than the north one, we recieved an inverted grid.
        // Switch to get the real north and south latitudes.
        printf("Inverting north and south latitudes.\n");
        cleanedRegion.northWest.latitude = sourceRegion.southEast.latitude;
        cleanedRegion.southEast.latitude = sourceRegion.northWest.latitude;
    }

    // Check if we need a west-east longitude inversion.
    if(sourceRegion.northWest.longitude > sourceRegion.southEast.longitude)
    {
        // If the west longitude is greater than the east one, we recieved an inverted grid.
        // Switch to get the real west and east latitudes.
        printf("Inverting west and east latitudes.\n");
        cleanedRegion.northWest.longitude = sourceRegion.southEast.longitude;
        cleanedRegion.southEast.longitude = sourceRegion.northWest.longitude;
    }

    return cleanedRegion;
}
