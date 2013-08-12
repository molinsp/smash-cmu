/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
 
#include "utilities/utilities_module.h"
#include <iomanip>		// std::setprecision
#include "Position.h"

#include "utilities/CommonMadaraVariables.h"

#define STRING_ENDS_WITH(str, end) (str.length() >= end.length() ? (0 == str.compare (str.length() - end.length(), end.length(), end)) : false)

// For Windows compatibility.
#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

// Conversion from degrees to radians.
#define DEG_TO_RAD(x) (x)*M_PI/180.0

#define REACHED_ACCURACY_METERS         0.2         // Margin (in meters) to use when checking if we have reached a location.

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record inflate_coords (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{

	if(args.size() != 2)
    {
		return Madara::Knowledge_Record::Integer(0);
    }

	//inflate_lat_long(String input_data, String output_variable_prefix)
	//Example: inflate_lat_long("12,5", ".reigon.1.topleft");
	std::string input = args[0].to_string();
	
	std::vector<std::string> tokens;
	std::vector<std::string> splitters;
	std::vector<std::string> pivots;
	
	splitters.push_back(",");
	Madara::Utility::tokenizer(input, splitters, tokens, pivots);
	
	for (unsigned int x = 0; x < tokens.size(); x++)
	{
		std::stringstream doubleBuffer;
		std::stringstream nameBuffer;
		
		double current_value;
		doubleBuffer << tokens[x];
		doubleBuffer >> current_value;

		nameBuffer << args[1].to_string();
		switch (x)
		{
			case 0: nameBuffer << ".latitude"; break;
			case 1: nameBuffer << ".longitude"; break;
			case 2: nameBuffer << ".altitude"; break;
			default: nameBuffer << ".null"; break;
		}
		//printf("Curr string: %s; value %.10f\n",nameBuffer.str().c_str(), current_value);
		variables.set(nameBuffer.str(), NUM_TO_STR(current_value));
	}

	return Madara::Knowledge_Record::Integer(1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record inflate_coord_array_to_local (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	if (args.size() != 1)
		return Madara::Knowledge_Record::Integer(0);
	std::map<std::string, Madara::Knowledge_Record> map;
	variables.to_map(args[0].to_string(), map);
	
	std::map<std::string, Madara::Knowledge_Record>::iterator iter;
	for (iter = map.begin(); iter != map.end(); ++iter)
	{
		std::stringstream evalBuffer;
		if (STRING_ENDS_WITH(iter->first, std::string(".location")))
		{
			
			evalBuffer << std::setprecision(10) << "." << iter->first << "=" << iter->first << ";inflate_coords(." << iter->first << ",'." << iter->first << "');";
			variables.evaluate(evalBuffer.str());
		}
		
	}
	return Madara::Knowledge_Record::Integer(1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record copy_vector (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{
	if (args.size() != 2)
		return Madara::Knowledge_Record::Integer(0);
	std::map<std::string, Madara::Knowledge_Record> map;
	
	variables.to_map(args[0].to_string(), map);
	
	if (map.size() == 0)
		return Madara::Knowledge_Record::Integer(1);
	
	std::string destination_prefix = args[1].to_string();
	
	int counter = 0;
	std::map<std::string, Madara::Knowledge_Record>::iterator iter;
	for (iter = map.begin(); iter != map.end(); ++iter)
	{
		std::stringstream buffer;
		buffer << destination_prefix << counter++;
		
		variables.set(buffer.str(), iter->second);
	}
	
	return Madara::Knowledge_Record::Integer(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Calculate the distance between two coordinate pairs.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
static double gps_coordinates_distance (double lat1, double long1, double lat2, double long2)
{
    const double EARTH_RADIUS = 6371000;

    // Get the difference between our two points then convert the difference into radians
    double lat_diff = DEG_TO_RAD(lat2 - lat1);
    double long_diff = DEG_TO_RAD(long2 - long1);

    lat1 =  DEG_TO_RAD(lat1);
    lat2 =  DEG_TO_RAD(lat2);

    double a =  pow(sin(lat_diff/2),2)+
                cos(lat1) * cos(lat2) *
                pow ( sin(long_diff/2), 2 );

    double c = 2 * atan2( sqrt(a), sqrt( 1 - a));
    return EARTH_RADIUS * c;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
* Checks if we are within a certain accuracy of a target location.
* @return  Returns true (1) if we are, or false (0) if not.
**/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record madaraTargetReached (Madara::Knowledge_Engine::Function_Arguments &args,
    Madara::Knowledge_Engine::Variables &variables)
{
    // All the params come from Madara.
    double currLat = args[0].to_double();
    double currLon = args[1].to_double();
    double targetLat = args[2].to_double();
    double targetLon = args[3].to_double();

    printf("Lat:      %.10f Long:   %.10f\n", currLat, currLon);
    printf("T_Lat:    %.10f T_Long: %.10f\n", targetLat, targetLon);

    double dist = gps_coordinates_distance(currLat, currLon, targetLat, targetLon);
    printf("Distance: %.2f\n", dist);

    if(dist < REACHED_ACCURACY_METERS)
    {
        printf("HAS reached target\n");
        return Madara::Knowledge_Record(1.0);
    }
    else
    {
        printf("HAS NOT reached target\n");
        return Madara::Knowledge_Record(0.0);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void define_utilities_functions (Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{
	knowledge.define_function("inflate_coords", inflate_coords);
	knowledge.define_function("inflate_coord_array_to_local", inflate_coord_array_to_local);
	knowledge.define_function("copy_vector", copy_vector);
    knowledge.define_function(MF_TARGET_REACHED, madaraTargetReached);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SMASH::Utilities::UtilitiesModule::initialize(Madara::Knowledge_Engine::Knowledge_Base& knowledge)
{
    printf("SMASH::Utilities::initialize...\n");
	define_utilities_functions(knowledge);
    printf("leaving SMASH::Utilities::initialize...\n");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SMASH::Utilities::UtilitiesModule::cleanup(Madara::Knowledge_Engine::Knowledge_Base& knowledge)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string SMASH::Utilities::UtilitiesModule::get_core_function()
{
    // No core function... only functions defined inside Madara so others can use them.
	return "";
}

