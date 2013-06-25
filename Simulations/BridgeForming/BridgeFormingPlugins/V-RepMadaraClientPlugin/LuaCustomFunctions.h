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
void registerMadaraClientSetupLuaCallback();
void registerMadaraClientBridgeRequestLuaCallback();
void registerMadaraClientUpdateStatusLuaCallback();
void registerMadaraClientGetNewMovementCommandLuaCallback();
void registerMadaraClientStopDroneLuaCallback();
void registerMadaraClientCleanupLuaCallback();

// The actual callbacks implementing the custom Lua functions.
void simExtMadaraClientSetup(SLuaCallBack* p);
void simExtMadaraClientBridgeRequest(SLuaCallBack* p);
void simExtMadaraClientUpdateStatus(SLuaCallBack* p);
void simExtMadaraClientGetNewMovementCommand(SLuaCallBack* p);
void simExtMadaraClientStopDrone(SLuaCallBack* p);
void simExtMadaraClientCleanup(SLuaCallBack* p);

#endif