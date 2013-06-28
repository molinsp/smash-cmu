/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/**
 * LuaCustomFunctions.h
 * Anton Dukeman
 *
 * Declares the callback functions for custom Lua methods, as well as internal
 * functions to register them.
 */

#ifndef _LUACUSTOMFUNCTIONS_H_
#define _LUACUSTOMFUNCTIONS_H_

#include "v_repLib.h"

// setup the MadaraQuadrotorControl
void registerMadaraQuadrotorControlSetupLuaCallback();
void simExtMadaraQuadrotorControlSetup(SLuaCallBack* p);

// update status and get new command
void registerMadaraQuadrotorControlUpdateStatusLuaCallback();
void simExtMadaraQuadrotorControlUpdateStatus(SLuaCallBack* p);
void registerMadaraQuadrotorControlGetNewCmdLuaCallback();
void simExtMadaraQuadrotorControlGetNewCmd(SLuaCallBack* p);

// determine if cmd is turn on, turn off, or go to
void registerMadaraQuadrotorControlIsGoToCmdLuaCallback();
void simExtMadaraQuadrotorControlIsGoToCmd(SLuaCallBack* p);
void registerMadaraQuadrotorControlIsLandCmdLuaCallback();
void simExtMadaraQuadrotorControlIsLandCmd(SLuaCallBack* p);
void registerMadaraQuadrotorControlIsTakeoffCmdLuaCallback();
void simExtMadaraQuadrotorControlIsTakeoffCmd(SLuaCallBack* p);

// terminate the MadaraQuadrotorControl
void registerMadaraQuadrotorControlCleanupLuaCallback();
void simExtMadaraQuadrotorControlCleanup(SLuaCallBack* p);

#endif // _LUACUSTOMFUNCTIONS_H_
