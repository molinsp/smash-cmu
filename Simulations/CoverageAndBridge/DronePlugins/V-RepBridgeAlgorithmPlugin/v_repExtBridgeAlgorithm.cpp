/********************************************************************* 
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#ifdef _WIN32
#include "bridge\BridgeAlgorithm.h"
#include "utilities\Position.h"
#elif defined(__linux)
#include "bridge/BridgeAlgorithm.h"
#include "utilities/Position.h"
#endif

#include "v_repExtBridgeAlgorithm.h"
#include "v_repLib.h"
#include <iostream>
#include <sstream>

using SMASH::Utilities::Position;
using SMASH::Bridge::BridgeAlgorithm;

//#ifdef _WIN32
//	#include <afxwin.h>         // MFC core and standard components
//	#include <winsock2.h>
//	#include <Mmsystem.h>
//	#pragma message("-----------------------> Adding library: Winmm.lib") 
//	#pragma comment(lib,"Winmm.lib")
//#endif /* _WIN32 */

#ifdef __linux
// perform a string comparison while ignoring case
bool _stricmp(const char *left, const char *right)
{
  int i = 0;
  while(left[i] != 0 && right[i] != 0) // go until reaching end of string
  {
    if(tolower(left[i]) != tolower(right[i])) // compare lower case versions
      return false; // not same, so return false
    ++i; // go to next character
  }
  return (left[i] == 0 && right[i] == 0); // reached end at same time, so true
}
#endif

#ifdef _WIN32
    #include <shlwapi.h> // required for PathRemoveFileSpec function
    //#define WIN_AFX_MANAGE_STATE AFX_MANAGE_STATE(AfxGetStaticModuleState())
#endif /* _WIN32 */
#if defined (__linux) || defined (__APPLE__)
    #define WIN_AFX_MANAGE_STATE
#endif /* __linux || __APPLE__ */

#define PLUGIN_VERSION 1

LIBRARY vrepLib; // the V-REP library that we will dynamically load and bind

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback of the Lua simExtGetPositionInBridge command.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void simExtGetPositionInBridge(SLuaCallBack* p)
{
    //WIN_AFX_MANAGE_STATE;
    simLockInterface(1);

    bool positionWasFound = false;

    float myNewX;
    float myNewY;

    bool paramsOk = true;

    // Check we have to correct amount of parameters.
    if (p->inputArgCount != 6)
    { 
        simSetLastError("simExtGetPositionInBridge", "Not enough arguments.");
        paramsOk = false;
    }

    // Check we have the correct type of arguments.
    if ( p->inputArgTypeAndSize[0*2+0] != sim_lua_arg_int )
    {
        simSetLastError("simExtGetPositionInBridge", "MyID parameter is not an int.");
        paramsOk = false;
    }

    if(p->inputArgTypeAndSize[1*2+0] != sim_lua_arg_float)
    {
        simSetLastError("simExtGetPositionInBridge", "RadioRange parameter is not a float.");
        paramsOk = false;
    }

    if(p->inputArgTypeAndSize[2*2+0] != (sim_lua_arg_float|sim_lua_arg_table) )
    {
        std::string errorMessage = "SourcePosition parameter is not an table of floats, it is a ";
        std::stringstream sstm; sstm << errorMessage << p->inputArgTypeAndSize[2*2+0] ; std::string fullErrorMessage = sstm.str();
        simSetLastError("simExtGetPositionInBridge", fullErrorMessage.c_str());
        paramsOk = false;
    }

    if(p->inputArgTypeAndSize[2*2+1] != 3)
    {
        std::string errorMessage = "SourcePosition parameter does not have 3 elements, it has ";
        std::stringstream sstm; sstm << errorMessage << p->inputArgTypeAndSize[2*2+1]; std::string fullErrorMessage = sstm.str();
        simSetLastError("simExtGetPositionInBridge", fullErrorMessage.c_str());
        paramsOk = false;
    }

    if(p->inputArgTypeAndSize[3*2+0] != (sim_lua_arg_float|sim_lua_arg_table))
    {
        std::string errorMessage = "SinkPosition parameter is not an table of floats, it is a ";
        std::stringstream sstm; sstm << errorMessage << p->inputArgTypeAndSize[3*2+0] ; std::string fullErrorMessage = sstm.str();
        simSetLastError("simExtGetPositionInBridge", fullErrorMessage.c_str());
        paramsOk = false;
    }        
        
    if(p->inputArgTypeAndSize[3*2+1] != 3)
    {
        std::string errorMessage = "SinkPosition parameter does not have 3 elements, it has ";
        std::stringstream sstm; sstm << errorMessage << p->inputArgTypeAndSize[3*2+1]; std::string fullErrorMessage = sstm.str();
        simSetLastError("simExtGetPositionInBridge", fullErrorMessage.c_str());
        paramsOk = false;
    }

    if(p->inputArgTypeAndSize[4*2+0] != (sim_lua_arg_int|sim_lua_arg_table)) 
    {
        std::string errorMessage = "AvailableDroneIds parameter is not an table of ints, it is a ";
        std::stringstream sstm; sstm << errorMessage << p->inputArgTypeAndSize[4*2+0]; std::string fullErrorMessage = sstm.str();
        simSetLastError("simExtGetPositionInBridge", fullErrorMessage.c_str());
        paramsOk = false;
    }             
        
    if(p->inputArgTypeAndSize[4*2+1] < 2)
    {
        simSetLastError("simExtGetPositionInBridge","AvailableDroneIds parameter is empty.");
        paramsOk = false;
    }

    if(p->inputArgTypeAndSize[5*2+0] != (sim_lua_arg_float|sim_lua_arg_table))
    {
        std::string errorMessage = "AvailableDronePositions parameter is not an table of floats, it is a ";
        std::stringstream sstm; sstm << errorMessage << p->inputArgTypeAndSize[5*2+0]; std::string fullErrorMessage = sstm.str();
        simSetLastError("simExtGetPositionInBridge", fullErrorMessage.c_str());
        paramsOk = false;
    }
        
    if(p->inputArgTypeAndSize[5*2+1] < 2) 
    {
        simSetLastError("simExtGetPositionInBridge", "AvailableDronePositions parameter is empty.");
        paramsOk = false;
    }

    // Continue forward calling the external functions only if we have all parameters ok.
    if(paramsOk)
    { 
        // Get the simple input values.
        int myId = p->inputInt[0];
        double commRange = p->inputFloat[0];
        Position sourcePosition = Position(p->inputFloat[1], p->inputFloat[2]); // 3 is the Z coord which we are not using.
        Position sinkPosition = Position(p->inputFloat[4], p->inputFloat[5]);   // 6 is the Z coord which we are not using.

        // Create the map with the available drone positions.
        int droneIdsIntArrayIdx = 1;
        int droneIdsAmount = p->inputArgTypeAndSize[4*2+1];
        int dronePositionsFloatArrayIdx = 7;
        int currDroneIdIntArrayIdx = droneIdsIntArrayIdx;
        int currDronePosFloatArrayIdx = dronePositionsFloatArrayIdx;
        std::map<int, Position> availableDronePositions = std::map<int, Position>();
        for(int currDroneIdx = 0; currDroneIdx < droneIdsAmount; currDroneIdx++)
        {
            // Update the current positions in the int and float arrays from which the id and (x,y) positions will be obtained, respectively.
            currDroneIdIntArrayIdx = droneIdsIntArrayIdx + currDroneIdx;
            currDronePosFloatArrayIdx = dronePositionsFloatArrayIdx + currDroneIdx * 2;	// Note that every position uses up 2 spaces in the float array, 1 for x and 2 for y.

            // Get the actual values from the array.
            int currDroneId = p->inputInt[currDroneIdIntArrayIdx];
            double currDronePositionX = p->inputFloat[currDronePosFloatArrayIdx];
            double currDronePositionY = p->inputFloat[currDronePosFloatArrayIdx + 1];

            // Create the position and add it to the map.
            Position currDronePosition = Position(currDronePositionX, currDronePositionY);
            availableDronePositions[currDroneId] = currDronePosition;
        }

        // For debugging, print out what we received.
        std::stringstream sstm; 
        sstm << "Values received inside C function: myId:" << myId << ", commRange:" << commRange <<
            ", sourcePosX:" << sourcePosition.x << ", sourcePosY:" << sourcePosition.y << 
            ", sinkPosX:" << sinkPosition.x << ", sinkPosY:W" << sinkPosition.y<< std::endl; 

        typedef std::map<int, Position>::iterator it_type;
	    for(it_type iterator = availableDronePositions.begin(); iterator != availableDronePositions.end(); iterator++) 
	    {
		    // Get info for current drone ID and position, and mark this drone as not assigned for now.
		    int currentDroneId = iterator->first;
		    Position currentDronePos = iterator->second;
            sstm << "Drone " << currentDroneId << " with pos " << currentDronePos.x << "," << currentDronePos.y << std::endl;
        }
        std::string message = sstm.str();
        simAddStatusbarMessage(message.c_str());

        // Call the bridge algorithm to find out if I have to move to help witht the bridge.
        BridgeAlgorithm algorithm;
        Position* myNewPosition = algorithm.getPositionInBridge(myId, commRange, sourcePosition, sinkPosition, availableDronePositions);

        // Store the position to be returned, if any.
        if(myNewPosition != NULL)
        {
            positionWasFound = true;
            myNewX = (float) myNewPosition->x;
            myNewY = (float) myNewPosition->y;

            std::stringstream sstmpos; 
            sstmpos << "Position was found! " << myNewX << "," << myNewY << std::endl;
            message = sstmpos.str();
            simAddStatusbarMessage(message.c_str());
        }
    }

    // Now we prepare the return value(s):
    p->outputArgCount = 2; // 2 return values
    p->outputArgTypeAndSize = (simInt*)simCreateBuffer(p->outputArgCount*2*sizeof(simInt)); // x return values takes x*2 simInt for the type and size buffer

    // Set the actual return values depending on whether we found a position or not.
    if (positionWasFound)
    {
        p->outputArgTypeAndSize[2*0+0] = sim_lua_arg_float;			 // The first return value is a float
        p->outputArgTypeAndSize[2*0+1] = 1;							 // Not used (table size if the return value was a table)

        p->outputArgTypeAndSize[2*1+0] = sim_lua_arg_float;			 // The second return value is a float
        p->outputArgTypeAndSize[2*1+1] = 1;							 // Not used (table size if the return value was a table)

        p->outputFloat=(simFloat*)simCreateBuffer(2*sizeof(myNewX)); // 2 float return value
        p->outputFloat[0] = myNewX;									 // This is the float value we want to return
        p->outputFloat[1] = myNewY;									 // This is the float value we want to return
    }
    else
    {
        // Both values will just be nil.
        p->outputArgTypeAndSize[2*0+0] = sim_lua_arg_nil;
        p->outputArgTypeAndSize[2*0+1] = 1;					// Not used (table size if the return value was a table)
        p->outputArgTypeAndSize[2*1+0] = sim_lua_arg_nil;
        p->outputArgTypeAndSize[2*1+1] = 1;					// Not used (table size if the return value was a table)
    }

    simLockInterface(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This is the plugin start routine (called just once, just after the plugin was loaded):
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VREP_DLLEXPORT unsigned char v_repStart(void* reservedPointer,int reservedInt)
{
    // Dynamically load and bind V-REP functions:
    // ******************************************
    // 1. Figure out this plugin's directory:
    char curDirAndFile[1024];
#ifdef _WIN32
    GetModuleFileName(NULL,curDirAndFile,1023);
    PathRemoveFileSpec(curDirAndFile);
#elif defined (__linux) || defined (__APPLE__)
    getcwd(curDirAndFile, sizeof(curDirAndFile));
#endif
    std::string currentDirAndPath(curDirAndFile);
    // 2. Append the V-REP library's name:
    std::string temp(currentDirAndPath);
#ifdef _WIN32
    temp+="\\v_rep.dll";
#elif defined (__linux)
    temp+="/libv_rep.so";
#elif defined (__APPLE__)
    temp+="/libv_rep.dylib";
#endif /* __linux || __APPLE__ */
    // 3. Load the V-REP library:
    vrepLib=loadVrepLibrary(temp.c_str());
    if (vrepLib==NULL)
    {
        std::cout << "Error, could not find or correctly load the V-REP library. Cannot start 'PluginSkeleton' plugin.\n";
        return(0); // Means error, V-REP will unload this plugin
    }
    if (getVrepProcAddresses(vrepLib)==0)
    {
        std::cout << "Error, could not find all required functions in the V-REP library. Cannot start 'PluginSkeleton' plugin.\n";
        unloadVrepLibrary(vrepLib);
        return(0); // Means error, V-REP will unload this plugin
    }
    // ******************************************

    // Check the version of V-REP:
    // ******************************************
    int vrepVer;
    simGetIntegerParameter(sim_intparam_program_version,&vrepVer);
    if (vrepVer<20604) // if V-REP version is smaller than 2.06.04
    {
        std::cout << "Sorry, your V-REP copy is somewhat old. Cannot start 'PluginSkeleton' plugin.\n";
        unloadVrepLibrary(vrepLib);
        return(0); // Means error, V-REP will unload this plugin
    }
    // ******************************************

    simLockInterface(1);

    // Here you could handle various initializations
    // Here you could also register custom Lua functions or custom Lua constants
    // etc.

    // Define the LUA function input parameters.
    int inArgs1[] = {6, sim_lua_arg_int,					  // My ID.
                        sim_lua_arg_float,					  // Radio range.
                        sim_lua_arg_float|sim_lua_arg_table,  // X and Y source position. (size: 3)
                        sim_lua_arg_float|sim_lua_arg_table,  // X and Y sink position. (size: 3)
                        sim_lua_arg_int|sim_lua_arg_table,  // IDs of available drones. (size: num available drones)
                        sim_lua_arg_float|sim_lua_arg_table}; // X and Y posotions for all available drones. (size: num available drones x 2).

    // Register the simExtGetPositionInBridge function.
    simRegisterCustomLuaFunction("simExtGetPositionInBridge",                       // The Lua function name.
                                 "number myNewX,number myNewY="                     // A tooltip to be shown to help the user know how to call it.
                                 "simExtGetPositionInBridge(int myId, "
                                                           "number radioRange, "
                                                           "table_3 sourcePos, "	// Note that "table_3" is only a tip to the reader about the table size.
                                                           "table_3 sinkPos, "
                                                           "table availableDroneIds, "
                                                           "table availableDronesPos)",
                                 inArgs1,                                           // The argument types.
                                 simExtGetPositionInBridge);                        // The C function that will be called by the Lua function.

    simLockInterface(0);
    return(PLUGIN_VERSION); // initialization went fine, we return the version number of this plugin (can be queried with simGetModuleName)
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This is the plugin end routine (called just once, when V-REP is ending, i.e. releasing this plugin):
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VREP_DLLEXPORT void v_repEnd()
{
    // Here you could handle various clean-up tasks
    unloadVrepLibrary(vrepLib); // release the library
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This is the plugin messaging routine (i.e. V-REP calls this function very often, with various messages):
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VREP_DLLEXPORT void* v_repMessage(int message,int* auxiliaryData,void* customData,int* replyData)
{ // This is called quite often. Just watch out for messages/events you want to handle
    // Keep following 6 lines at the beginning and unchanged:
    simLockInterface(1);
    static bool refreshDlgFlag=true;
    int errorModeSaved;
    simGetIntegerParameter(sim_intparam_error_report_mode,&errorModeSaved);
    simSetIntegerParameter(sim_intparam_error_report_mode,sim_api_errormessage_ignore);
    void* retVal=NULL;

    // Here we can intercept many messages from V-REP (actually callbacks). Only the most important messages are listed here.
    // For a complete list of messages that you can intercept/react with, search for "sim_message_eventcallback"-type constants
    // in the V-REP user manual.

    if (message==sim_message_eventcallback_refreshdialogs)
        refreshDlgFlag=true; // V-REP dialogs were refreshed. Maybe a good idea to refresh this plugin's dialog too

    if (message==sim_message_eventcallback_menuitemselected)
    { // A custom menu bar entry was selected..
        // here you could make a plugin's main dialog visible/invisible
    }

    if (message==sim_message_eventcallback_instancepass)
    {	// This message is sent each time the scene was rendered (well, shortly after) (very often)
        // It is important to always correctly react to events in V-REP. This message is the most convenient way to do so:

        int flags=auxiliaryData[0];
        bool sceneContentChanged=((flags&(1+2+4+8+16+32+64+256))!=0); // object erased, created, model or scene loaded, und/redo called, instance switched, or object scaled since last sim_message_eventcallback_instancepass message 
        bool instanceSwitched=((flags&64)!=0);

        if (instanceSwitched)
        {
            // React to an instance switch here!!
        }

        if (sceneContentChanged)
        { // we actualize plugin objects for changes in the scene

            //...

            refreshDlgFlag=true; // always a good idea to trigger a refresh of this plugin's dialog here
        }
    }

    if (message==sim_message_eventcallback_mainscriptabouttobecalled)
    { // The main script is about to be run (only called while a simulation is running (and not paused!))
        
    }

    if (message==sim_message_eventcallback_simulationabouttostart)
    { // Simulation is about to start

    }

    if (message==sim_message_eventcallback_simulationended)
    { // Simulation just ended

    }

    if (message==sim_message_eventcallback_moduleopen)
    { // A script called simOpenModule (by default the main script). Is only called during simulation.
        if ( (customData==NULL)||(_stricmp("PluginSkeleton",(char*)customData)==0) ) // is the command also meant for this plugin?
        {
            // we arrive here only at the beginning of a simulation
        }
    }

    if (message==sim_message_eventcallback_modulehandle)
    { // A script called simHandleModule (by default the main script). Is only called during simulation.
        if ( (customData==NULL)||(_stricmp("PluginSkeleton",(char*)customData)==0) ) // is the command also meant for this plugin?
        {
            // we arrive here only while a simulation is running
        }
    }

    if (message==sim_message_eventcallback_moduleclose)
    { // A script called simCloseModule (by default the main script). Is only called during simulation.
        if ( (customData==NULL)||(_stricmp("PluginSkeleton",(char*)customData)==0) ) // is the command also meant for this plugin?
        {
            // we arrive here only at the end of a simulation
        }
    }

    if (message==sim_message_eventcallback_instanceswitch)
    { // Here the user switched the scene. React to this message in a similar way as you would react to a full
      // scene content change. In this plugin example, we react to an instance switch by reacting to the
      // sim_message_eventcallback_instancepass message and checking the bit 6 (64) of the auxiliaryData[0]
      // (see here above)

    }

    if (message==sim_message_eventcallback_broadcast)
    { // Here we have a plugin that is broadcasting data (the broadcaster will also receive this data!)

    }

    if (message==sim_message_eventcallback_scenesave)
    { // The scene is about to be saved. If required do some processing here (e.g. add custom scene data to be serialized with the scene)

    }

    // You can add many more messages to handle here

    if ((message==sim_message_eventcallback_guipass)&&refreshDlgFlag)
    { // handle refresh of the plugin's dialogs
        // ...
        refreshDlgFlag=false;
    }

    // Keep following unchanged:
    simSetIntegerParameter(sim_intparam_error_report_mode,errorModeSaved); // restore previous settings
    simLockInterface(0);
    return(retVal);
}
