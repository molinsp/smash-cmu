/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * SimplePluginInterface.h - Specifies the function that a plugin has to
 * implement in order to act as a complete but simple plugin.
 *********************************************************************/

#pragma once

#ifndef _SIMPLE_PLUGIN_INTERFACE_H
#define _SIMPLE_PLUGIN_INTERFACE_H

#include <string>

// Interface that a simple plugin must implement.
namespace VREP
{
	class ISimplePlugin
	{
	public:
		/**
		 * Sets up the plugin.
		 * @param suffix the suffix of the script that called this plugin.
		 **/
		virtual void initialize(int suffix) = 0;

		/**
		 * Cleans up the plugin.
         * @param suffix the suffix of the script that called this plugin.
		 **/
		virtual void cleanup(int suffix) = 0;

	   /**
		 * Executed in every step of the simulation.
         * @param suffix the suffix of the script that called this plugin.
		 **/
		virtual void executeStep(int suffix) = 0;

	   /**
		 * Returns a short text id of the plugin, used for registering Lua functions.
         * @return a string containing an id or name of the plugin, to add to the names of Lua functions.
		 **/
		virtual std::string getId() = 0;
	};
}

// Helper function to create the actual plugin.
VREP::ISimplePlugin* createPlugin();


#endif