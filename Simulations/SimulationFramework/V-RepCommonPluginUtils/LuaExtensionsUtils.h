/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * LuaExtensionsUtils.h - Util functions for creating custom Lua
 * functions in V-Rep.
 *********************************************************************/

#pragma once

#include "v_repLib.h"
#include <string>
#include <vector>

#ifndef _LUA_EXTENSIONS_UTILS_H
#define _LUA_EXTENSIONS_UTILS_H

bool checkInputArguments(SLuaCallBack* p, int inputArgumentsDescription[], const char* callerFunctionName);
void setupDefaultOutput(SLuaCallBack* p, int numberOfOutputs);
void setupStringOutputBuffer(SLuaCallBack* p, std::vector<std::string> stringOutputs);

#endif