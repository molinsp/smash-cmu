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

// Macro to convert from int to std::string.
#define NUM_TO_STR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << std::setprecision(10) << x ) ).str()

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
		int currInputArgTypePosition = i*2+0;
		int expectedInputArgTypePosition = i + 1;

		// Check we have the correct type of arguments.
		if ( p->inputArgTypeAndSize[currInputArgTypePosition] != inputArgumentsDescription[expectedInputArgTypePosition] )
		{
			std::string errorMessage = std::string("Input parameter ") + NUM_TO_STR(expectedInputArgTypePosition) + 
				                       " is not of the expected type " + NUM_TO_STR(inputArgumentsDescription[expectedInputArgTypePosition]);
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
	// Setup the amount of arguments, and create the buffer for their types and sizes.
	p->outputArgCount = numberOfOutputs;
	p->outputArgTypeAndSize = (simInt*)simCreateBuffer(p->outputArgCount * (2 * sizeof(simInt)));

	// By default set all results to nil.
	for(int i=0; i<p->outputArgCount; i++)
	{
		int currOutputTypePos = i*2+0;
		int currOutputSizePos = currOutputTypePos + 1;
		p->outputArgTypeAndSize[currOutputTypePos] = sim_lua_arg_nil;
		p->outputArgTypeAndSize[currOutputSizePos] = 1;
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