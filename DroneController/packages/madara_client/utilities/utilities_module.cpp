/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
 
#include "utilities/utilities_module.h"

Madara::Knowledge_Record inflate_coords (Madara::Knowledge_Engine::Function_Arguments & args, Madara::Knowledge_Engine::Variables & variables)
{

	if (args.size() != 2)
		return Madara::Knowledge_Record::Integer(0);

	//inflate_lat_long(String input_data, String output_variable_prefix)
	//Example: inflate_lat_long("12,5", ".reigon.1.topleft");
	std::string input = args[0].to_string();
	
	std::vector<std::string> tokens;
	std::vector<std::string> splitters;
	std::vector<std::string> pivots;
	
	double latitude, longitude;
	
	splitters.push_back(",");
	Madara::Utility::tokenizer(input, splitters, tokens, pivots);
	
	for (int x = 0; x < tokens.size(); x++)
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
		variables.set(nameBuffer.str(), current_value);
	}
	return Madara::Knowledge_Record::Integer(1);
}

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
			
			evalBuffer << "." << iter->first << "=" << iter->first << ";inflate_coords(." << iter->first << ",'." << iter->first << "');";
			variables.evaluate(evalBuffer.str());
		}
		
	}
	return Madara::Knowledge_Record::Integer(1);
}

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

void define_utilities_functions (Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{
	knowledge.define_function("inflate_coords", inflate_coords);
	knowledge.define_function("inflate_coord_array_to_local", inflate_coord_array_to_local);
	knowledge.define_function("copy_vector", copy_vector);
}

void SMASH::Utilities::initialize(Madara::Knowledge_Engine::Knowledge_Base& knowledge)
{
	define_utilities_functions(knowledge);
}

