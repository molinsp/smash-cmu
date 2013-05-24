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

#ifndef _LUA_CUSTOM_FUNCTIONS_H
#define _LUA_CUSTOM_FUNCTIONS_H

#include "MadaraController.h"
#include "v_repLib.h"

// The actual controller to manage the Madara stuff.
extern MadaraController* madaraController;

// Functions to register the callbacks to the custom Lua functions.
void registerMadaraClientSetupLuaCallback();
void registerMadaraClientBridgeRequestLuaCallback();
void registerMadaraClientUpdateStatusLuaCallback();
void registerMadaraClientGetPositionInBridge();

// The actual callbacks implementing the custom Lua functions.
void simExtMadaraClientSetup(SLuaCallBack* p);
void simExtMadaraClientBridgeRequest(SLuaCallBack* p);
void simExtMadaraClientUpdateStatus(SLuaCallBack* p);
void simExtMadaraClientGetPositionInBridge(SLuaCallBack* p);

#endif