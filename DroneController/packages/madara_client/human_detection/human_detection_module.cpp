/********************************************************************* 
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * human_detection_module.cpp - Defines the manager for human detection.
 *********************************************************************/

#include <vector>
#include <map>
#include <string>
#include "utilities/CommonMadaraVariables.h"
#include "movement/platform_movement.h"
#include "human_detection_module.h"
#include "HumanDetection.h"
#include "BasicStrategy.h"
#include "SlidingWindowStrategy.h"

using namespace SMASH::HumanDetection;
using std::string;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Madara Variable Definitions
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions.
#define MF_MAIN_LOGIC               "human_detection_doHumanDetection"		  // Function that calls detect human function
                                                                            // if the drone isn't already detecting human.
#define MF_DETECT_HUMAN             "human_detection_detectHuman"           // Function that detects human.

// Internal variables.

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private variables.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Define the ids for the expressions that will exist to accomplish different aspects of human detection.
enum HumanDetectionMadaraExpressionId 
{
  // Expression to call function to update the positions of the drones available for human detection.
  HDE_FIND_AVAILABLE_DRONES_POSITIONS,
};

// Map of Madara expressions used in human detection.
static std::map<HumanDetectionMadaraExpressionId, Madara::Knowledge_Engine::Compiled_Expression> m_expressions;

// Stores information about human detection algorithm.
static HumanDetection* m_humanDetectionAlgorithm;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private function declarations.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void defineFunctions(Madara::Knowledge_Engine::Knowledge_Base &knowledge);
static Madara::Knowledge_Record madaraDetectHuman (Madara::Knowledge_Engine::Function_Arguments &args,
                                                   Madara::Knowledge_Engine::Variables &variables);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Initializer, gets the refence to the knowledge base and compiles expressions.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SMASH::HumanDetection::HumanDetectionModule::initialize(Madara::Knowledge_Engine::Knowledge_Base &knowledge)
{
  printf("SMASH::HumanDetection::initialize...\n");
 
  // Defines internal and external functions.
  defineFunctions(knowledge);

  printf("leaving SMASH::HumanDetection::initialize...\n");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Cleanup, not needed in this module.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SMASH::HumanDetection::HumanDetectionModule::cleanup(Madara::Knowledge_Engine::Knowledge_Base &knowledge)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gets the main logic to be run. This returns a function call that can be included in another block of logic.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string SMASH::HumanDetection::HumanDetectionModule::get_core_function()
{
  return MF_MAIN_LOGIC "()";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers functions with Madara.
// ASSUMPTION: Drone IDs are continuous, starting from 0.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void defineFunctions(Madara::Knowledge_Engine::Knowledge_Base &knowledge)
{
  // Function that can be included in main loop of another method to introduce human detection. 
  // Only does the actual bridge calculations if the command to do so is on.
  // Assumes that MV_HUMAN_DETECTION_REQUESTED triggers the human detection logic.
  knowledge.define_function(MF_MAIN_LOGIC, 
    "(" MV_HUMAN_DETECTION_REQUESTED("{.id}") " => "
        "("
            // If human detection requested, then check if currently performing human detection. If not
            // performing human detection currently, then perform human detection.
            "(!" MV_DETECTING_HUMAN("{" MV_MY_ID "}") ")"
                " => " MF_DETECT_HUMAN "();"
        ")"
    ")"
  );

  // Function that actually performs bridge building for this drone.
  knowledge.define_function(MF_DETECT_HUMAN, madaraDetectHuman);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Select a human detection algorithm based on the string from madara variable.
 *
 * @param algo  String that determines which human detection algorithm will be selected.
 **/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
HumanDetection* selectHumanDetectionAlgorithm (string algo)
{
  HumanDetection* humanDetectionAlgorithm = NULL;

  if (algo == HUMAN_DETECTION_BASIC)
    humanDetectionAlgorithm = new BasicStrategy(); 
  else if (algo == HUMAN_DETECTION_SLIDING_WINDOW)
    humanDetectionAlgorithm = new SlidingWindowStrategy();
  else
  {
    string err = "selectHumanDetectionAlgorithm(algo = \"";
    err += algo;
    err += "\") failed to find match\n";
    printf("%s", err.c_str());
  }
  
  return humanDetectionAlgorithm;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Method that invokes the functionality of detecting human.
 * @return  Returns true (1) always.
 **/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record madaraDetectHuman (Madara::Knowledge_Engine::Function_Arguments &args,
                                            Madara::Knowledge_Engine::Variables &variables)
{
  string algo = variables.get(MV_HUMAN_DETECTION_REQUESTED("{.id}")).to_string();

  m_humanDetectionAlgorithm = selectHumanDetectionAlgorithm(algo);
  
  int result_map[8][8];
  int result;

  printf("About to call human detection function\n");

  result = m_humanDetectionAlgorithm->detect_human(result_map, spin);

  if (result > 0)
    variables.set(MV_HUMAN_DETECTED("{.id}"), 1.0);  

  return Madara::Knowledge_Record(1.0);
}
