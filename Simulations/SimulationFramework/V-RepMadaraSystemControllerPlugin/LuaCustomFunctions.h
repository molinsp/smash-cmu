/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * LuaCalbacks.h - Declares the callback functions for custom Lua
 * methods, as well as internal functions to register them.
 *********************************************************************/

#pragma once

#include "v_repLib.h"

#ifndef _LUA_CUSTOM_FUNCTIONS_H
#define _LUA_CUSTOM_FUNCTIONS_H

// Functions to register the callbacks to the custom Lua functions.
void registerMadaraSystemControllerSetupLuaCallback();
void registerMadaraSystemControllerCleanupLuaCallback();

void registerMadaraSystemControllerUpdateStatusLuaCallback();
void registerMadaraSystemControllerBridgeRequestLuaCallback();
void registerMadaraSystemControllerSetupSearchAreaLuaCallback();
void registerMadaraSystemControllerSearchRequestLuaCallback();

// The actual callbacks implementing the custom Lua functions.
void simExtMadaraSystemControllerSetup(SLuaCallBack* p);
void simExtMadaraSystemControllerCleanup(SLuaCallBack* p);

void simExtMadaraSystemControllerUpdateStatus(SLuaCallBack* p);
void simExtMadaraSystemControllerBridgeRequest(SLuaCallBack* p);
void simExtMadaraSystemControllerSetupSearchArea(SLuaCallBack* p);
void simExtMadaraSystemControllerSearchRequest(SLuaCallBack* p);


#endif