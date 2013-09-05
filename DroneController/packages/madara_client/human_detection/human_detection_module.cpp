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
#include "utilities/Position.h"
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
#define MF_MAIN_LOGIC               "human_detection_doHumanDetection"		    // Function that calls detect human function
// if the drone isn't already detecting human.
#define MF_DETECT_HUMAN             "human_detection_detectHuman"             // Function that detects human.
#define MF_CALCULATE_AMBIENT_TEMP   "human_detection_calculateAmbientTemp"    // Function to calculate ambient temperature.

// Internal variables.
#define MV_AMBIENT_TEMP_CALCULATED  ".human_detection_ambientTempCalculated"  // Flag to check if ambient temperature has
// been initialized.

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

// Variables to store calculated ambient min and max.
static double ambient_min;
static double ambient_max;

// 2D array to store temperature.
static double m_thermal_buffer[8][8];

// Variable to store previously selected algorithm.
static string prev_algo;

// Flag to check if ambient temp has been set once.
bool is_ambient_temp_set = false;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private function declarations.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void defineFunctions(Madara::Knowledge_Engine::Knowledge_Base &knowledge);
static Madara::Knowledge_Record madaraDetectHuman (Madara::Knowledge_Engine::Function_Arguments &args,
    Madara::Knowledge_Engine::Variables &variables);
static Madara::Knowledge_Record madaraCalculateAmbientTemp (Madara::Knowledge_Engine::Function_Arguments &args,
    Madara::Knowledge_Engine::Variables &variables);
static void loadThermalInfo(Madara::Knowledge_Engine::Variables &variables);
static void on_human_detected();

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
    // Delete the human detection algorithm.
    delete m_humanDetectionAlgorithm;
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
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void defineFunctions(Madara::Knowledge_Engine::Knowledge_Base &knowledge)
{
    // Function that can be included in main loop of another method to introduce human detection. 
    // Only does the actual bridge calculations if the command to do so is on.
    // Assumes that MV_HUMAN_DETECTION_REQUESTED triggers the human detection logic.
    knowledge.define_function(MF_MAIN_LOGIC,  
        "(" 
            // First check if human detection is enabled at all.
            "(" MV_HUMAN_DETECTION_REQUESTED("{" MV_MY_ID "}") " ) => "
            "("
                // The basic detection algorithm needs specific setup.
                "(" MV_HUMAN_DETECTION_REQUESTED("{" MV_MY_ID "}") " == '" MO_HUMAN_DETECTION_BASIC "' ) => " 
                "("
                    // We have to calculate the ambient temperature once we have reached our assigned height, but only once.
                    "(" MV_IS_AT_ASSIGNED_ALTITUDE " && (" MV_AMBIENT_TEMP_CALCULATED " == 0) ) => "
                    "("
                        // Calculate the ambient temperature and mark that as done to prevent recalculating it.
                        MF_CALCULATE_AMBIENT_TEMP "();"
                        MV_AMBIENT_TEMP_CALCULATED " = 1;"
                    ");"

                    // Human detection for this algorithm will only be executed if the ambient temperature has been set.
                    "(" MV_AMBIENT_TEMP_CALCULATED  ")  => "
                    "("
                        MF_DETECT_HUMAN "();"
                    ");"
                ");"

                // If sliding window algorithm, then no need specific setup. Just call the detect function.
                "(" MV_HUMAN_DETECTION_REQUESTED("{" MV_MY_ID "}") " == '" MO_HUMAN_DETECTION_SLIDING_WINDOW "' ) => " MF_DETECT_HUMAN "();"
            ");"
        ");"
    );

    // Function that actually performs ambient temperature calculation for this drone.
    knowledge.define_function(MF_CALCULATE_AMBIENT_TEMP, madaraCalculateAmbientTemp);

    // Function that actually performs human detection for this drone.
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

    if (algo == MO_HUMAN_DETECTION_BASIC)
    {
        humanDetectionAlgorithm = new BasicStrategy(ambient_min, ambient_max); 
    }
    else if (algo ==MO_HUMAN_DETECTION_SLIDING_WINDOW)
    {  
        humanDetectionAlgorithm = new SlidingWindowStrategy();
    }
    else
    {
        string err = "selectHumanDetectionAlgorithm(algo = \"";
        err += algo;
        err += "\") failed to find match. Using BasicStrategy as default.\n";
        printf("%s", err.c_str());
        humanDetectionAlgorithm = new BasicStrategy(ambient_min, ambient_max);
    }

    return humanDetectionAlgorithm;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
* Method that loads thermal information from a set of madara variables into thermal_buffer which will be used
* by the detection algorithms.
**/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loadThermalInfo (Madara::Knowledge_Engine::Variables &variables)
{
    for (int row = 0; row < 8; ++row)
    {  
        for (int col = 0; col < 8; ++col)
        {
            std::string rowString = NUM_TO_STR(row);
            std::string colString = NUM_TO_STR(col);
            m_thermal_buffer[row][col] = variables.get(MV_THERMAL_BUFFER(rowString,colString)).to_double();
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
* Metho that calculates ambient temperature range for basic strategy.
*
* @return Returns true (1) always. 
**/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record madaraCalculateAmbientTemp (Madara::Knowledge_Engine::Function_Arguments &args,
    Madara::Knowledge_Engine::Variables &variables)
{
    double env_temp = variables.get(MV_ENVIRONMENT_TEMPERATURE).to_double();
    if (env_temp == 0)
    {
        // Set min to a large number.
        ambient_min = 2000;

        for (int i = 0; i < MAX_SAMPLE_SIZE; ++i)
        {
            // Call to read thermal buffer.
            loadThermalInfo(variables);

            // Use the above obtained (valid) buffer and determine the ambient temperature range.
            for (int row = 0; row < 8; ++row)
            {
                for (int col = 0; col < 8; ++col)
                {
                    if (m_thermal_buffer[row][col] < ambient_min)
                        ambient_min = m_thermal_buffer[row][col];

                    if (m_thermal_buffer[row][col] > ambient_max)
                        ambient_max = m_thermal_buffer[row][col];
                }
            }
        }

        // Apply error ranges to the above calculated ambient temperature range.
        ambient_min = ambient_min - ERROR_LIMIT;
        ambient_max = ambient_max + ERROR_LIMIT;

        // Once the error ranges have been applied make sure the ambient temperature
        // range is not greater than 10.
        if ((ambient_max - ambient_min) > 10)
            // If ambient temperature range is > 10 then increase the minimum such that
            // the range is not greater than 10.
            ambient_min = ambient_min + (ambient_max - ambient_min - 10);
    }
    else
    {
        ambient_min = env_temp - AMBIENT_RANGE;
        ambient_max = env_temp + AMBIENT_RANGE;
    }  

    printf("Final Ambient Min: %6.2f \n", ambient_min);
    printf("Final Ambient Max: %6.2f \n", ambient_max);
    printf("\n");

    return Madara::Knowledge_Record(1.0);  
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
* Method that invokes the functionality of detecting human.
* 
* @return  Returns true (1) always.
**/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record madaraDetectHuman (Madara::Knowledge_Engine::Function_Arguments &args,
    Madara::Knowledge_Engine::Variables &variables)
{
    // Get the current algorithm to use, and the current altitude.
    string algo = variables.get(variables.expand_statement(MV_HUMAN_DETECTION_REQUESTED("{" MV_MY_ID "}"))).to_string();
    double height = variables.get(variables.expand_statement(MV_DEVICE_ALT("{" MV_MY_ID  "}"))).to_double();

    if (prev_algo.empty() || algo != prev_algo)
    {
        if (m_humanDetectionAlgorithm != NULL)
            delete (m_humanDetectionAlgorithm); 

        m_humanDetectionAlgorithm = selectHumanDetectionAlgorithm(algo);
        prev_algo = algo;
    } 

    // Execute the human detection algorithm.
    int result_map[8][8];
    int result = 0;
    if (m_humanDetectionAlgorithm != NULL)
    {
        // First get the current thermal readings from Madara, then call the algorithm.
        loadThermalInfo(variables);
        result = m_humanDetectionAlgorithm->detect_human(m_thermal_buffer, height, result_map, on_human_detected);
    }

    if (result > 0)
    {
        printf("RESULT: %i \n", result);

        // Set the Madara variables to indicate we found thermals, and how many, at our current location.
        std::string lat = variables.get(variables.expand_statement(MV_DEVICE_LAT("{" MV_MY_ID "}"))).to_string();
        std::string lon = variables.get(variables.expand_statement(MV_DEVICE_LON("{" MV_MY_ID "}"))).to_string();
        variables.set(MV_THERMALS_AT_LOCATION(lat, lon), (Madara::Knowledge_Record::Integer) result);
    }
    else
        printf("No Human Detected \n");

    return Madara::Knowledge_Record(1.0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
*
**/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void on_human_detected()
{
    printf("**********************************************\n");
    printf("*              HUMAN DETECTED                *\n");
    printf("**********************************************\n");
    //sleep(1);
}
