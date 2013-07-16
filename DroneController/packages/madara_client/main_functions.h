/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * main_functions.h - Declares functions for the main module.
 *********************************************************************/

#ifndef _MAIN_FUNCTIONS_H
#define _MAIN_FUNCTIONS_H

#include "madara/knowledge_engine/Knowledge_Base.h"
#include <string>

//Setup of pre-compiled expressions
void main_compile_expressions (Madara::Knowledge_Engine::Knowledge_Base & knowledge);

// Returns the main logic expression.
Madara::Knowledge_Engine::Compiled_Expression main_get_main_expression();

#endif
