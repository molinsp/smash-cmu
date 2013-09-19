/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * LuaFunctionRegistration.h - Specifies the function that a plugin has to
 * implement in order to register all its Lua extensions.
 *********************************************************************/

#pragma once

#ifndef _LUA_FUNCTION_REGISTRATION_H
#define _LUA_FUNCTION_REGISTRATION_H

// Registers callbacks to all new Lua functions defined by the plugin.
void registerAllLuaExtensions();

#endif