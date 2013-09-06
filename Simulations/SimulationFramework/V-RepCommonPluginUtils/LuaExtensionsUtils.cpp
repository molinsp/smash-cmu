/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * LuaExtensionsUtils.cpp - Util functions for creating custom Lua
 * functions in V-Rep.
 *********************************************************************/

#include "LuaExtensionsUtils.h"
#include <iostream>     // std::dec
#include <sstream>      // std::ostringstream
#include <iomanip>		// std::setprecision
#include <cstdarg>
#include <string>
using std::string;

// Macro to convert from int to std::string.
#define NUM_TO_STR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << std::setprecision(10) << x ) ).str()

struct type_to_string
{
    int type;
    string description;
};
const type_to_string TYPES[] =
{
    { sim_lua_arg_nil, "nil" },
    { sim_lua_arg_bool, "bool" },
    { sim_lua_arg_int, "int" },
    { sim_lua_arg_float, "float" },
    { sim_lua_arg_string, "string" },
};
unsigned int TYPES_SIZE = sizeof(TYPES) / sizeof(TYPES[0]);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Checks if we have the expected amount and type of characters.
// - p: the Lua callback pointer.
// - inputArgumentsDescription: array with the amount of expected arguments as the first value, and their types
//   as the rest of the array values.
// - callerFunctionName: a string indicating the function that is processing this, to set error messages if required.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool checkInputArguments(SLuaCallBack* p, int inputArgumentsDescription[], const char* callerFunctionName)
{
	// The first value in the array is the number of arguments, which determines how many more values are in the array too.
	int numberOfArguments = inputArgumentsDescription[0];

    // Check we have to correct amount of parameters.
    if (p->inputArgCount != numberOfArguments)
    { 
        simSetLastError(callerFunctionName, "Not enough arguments.");
        return false;
    }

	// Loop over all arguments to check if we have the correct type for each one.
	for(int i = 0; i < numberOfArguments; i++)
	{
        int actualInputType = p->inputArgTypeAndSize[i * 2];
        int expectInputType = inputArgumentsDescription[i + 1];

		// Check we have the correct type of arguments.
		if (actualInputType != expectInputType)
		{
			string errorMessage = std::string("Input parameter ") +
                NUM_TO_STR(i + 1) + " is of type ";
            string actual = "INVALID_TYPE";
            for(unsigned int i = 0; i < TYPES_SIZE; ++i)
            {
                if((actualInputType & ~sim_lua_arg_table) == TYPES[i].type)
                {
                    actual = TYPES[i].description;
                    if(actualInputType & sim_lua_arg_table)
                    {
                        actual += " table";
                    }
                    break;
                }
            }
            errorMessage += actual + " and not of expected type ";
            string expected = "INVALID_TYPE";
            for(unsigned int i = 0; i < TYPES_SIZE; ++i)
            {
                if((expectInputType & ~sim_lua_arg_table) == TYPES[i].type)
                {
                    expected = TYPES[i].description;
                    if(expectInputType & sim_lua_arg_table)
                    {
                        expected += " table";
                    }
                    break;
                }
            }
            errorMessage += expected;
			simSetLastError(callerFunctionName, errorMessage.c_str());
			return false;
		}
	}

	// Everything was ok.
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sets up the output buffer description about the outputs, setting all of them to nil by default.
// Does NOT setup the buffers for the actual values, if any.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setupDefaultOutput(SLuaCallBack* p, int numberOfOutputs)
{
    setupOutputTypes(p, numberOfOutputs, sim_lua_arg_nil);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sets up the output buffer description about the outputs, setting all of them to the given type.
// Does NOT setup the buffers for the actual values, if any.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setupOutputTypes(SLuaCallBack* p, int numberOfOutputs, int type)
{
	// Setup the amount of arguments, and create the buffer for their types and sizes.
	p->outputArgCount = numberOfOutputs;
	int bufferSize = p->outputArgCount * (2 * sizeof(simInt));
	p->outputArgTypeAndSize = (simInt*)simCreateBuffer(bufferSize);

	// Set up the types to the given value.
	for(int i=0; i<p->outputArgCount; i++)
	{
		int currOutputTypePos = i*2;
		int currOutputSizePos = currOutputTypePos + 1;
		p->outputArgTypeAndSize[currOutputTypePos] = type;
		p->outputArgTypeAndSize[currOutputSizePos] = 1;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sets up the output buffer description about the outputs, setting only one output type, table, all of the same size.
// Does NOT setup the buffers for the actual values, if any.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setupOutputsToTables(SLuaCallBack* p, int numberOfTables, int tableSize)
{
	// Setup the amount of arguments, and create the buffer for their types and sizes.
	p->outputArgCount = numberOfTables;
	int bufferSize = p->outputArgCount * (2 * sizeof(simInt));
	p->outputArgTypeAndSize = (simInt*)simCreateBuffer(bufferSize);

	// Set the type and size.
    for(int i=0; i < p->outputArgCount; i++)
    {
		int currOutputTypePos = i*2;
		int currOutputSizePos = currOutputTypePos + 1;
        p->outputArgTypeAndSize[currOutputTypePos] = sim_lua_arg_table;
	    p->outputArgTypeAndSize[currOutputSizePos] = tableSize;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sets up the char* output buffer will all the given strings.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setupStringOutputBuffer(SLuaCallBack* p, std::vector<std::string> stringOutputs)
{
	int charBufferSize = 0;
	for(unsigned int i=0; i<stringOutputs.size(); i++)
	{
		// Plus 1 for the \0 terminating char that we will have to add to the char buffer to separate each string.
		charBufferSize += stringOutputs[i].length() + 1;
	}

	// Create a string buffer to return all return values in it.
	p->outputChar = (simChar*) simCreateBuffer(charBufferSize * sizeof(simChar));

	// Go over all strings, appending them together as char strings in the buffer, with \0 separators.
	unsigned int absolutBufferPos = 0;
	for(unsigned int i=0; i<stringOutputs.size(); i++)
	{
		std::string currString = stringOutputs[i];
		for(unsigned int i = 0; i < currString.length(); ++i)
		{
			p->outputChar[absolutBufferPos++] = currString.at(i);
		}

		p->outputChar[absolutBufferPos++] = '\0';
	}
}
