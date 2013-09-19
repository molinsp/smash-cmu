/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * SystemControllerPlugin.cpp - Specifies a SystemController plugin for VRep.
 *********************************************************************/

#include "SystemControllerPlugin.h"
#include "v_repLib.h"
#include <string>

// Id for a controller, a plugin must have this value as its Madara id.
const int PLUGIN_CONTROLLER_ID = 202;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// We have to create an actual object of this type so that the plugin DLL entry points will have access to it.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SMASH::ISimplePlugin* createPlugin()
{
    SMASH::SystemControllerPlugin* plugin = new SMASH::SystemControllerPlugin();
    return plugin;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gets a param of double type.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double getDoubleParam(std::string paramName)
{
    // Buffer to get the parameter as a string.
    int paramBufferSize = 100;
    char* paramBuffer;

    // Get the param as a string.
    double paramValue = 0;
	paramBuffer = simGetScriptSimulationParameter(sim_handle_main_script, paramName.c_str(), &paramBufferSize);

    // If we got it, try to convert it to double and clear the buffer.
    if(paramBuffer == NULL)
    {
        paramValue = atof(paramBuffer);
        simReleaseBuffer(paramBuffer);
    }

    return paramValue;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SMASH::SystemControllerPlugin::initialize(int suffix)
{
    simAddStatusbarMessage("SystemControllerPlugin::initialize: Initializing System Controller.");

    m_searchRequestId = 0;
    m_bridgeRequestId = 0;

    // Get the various user setup parameters.
    m_numDrones = (int) getDoubleParam("numberOfDrones");
    double radioRange = getDoubleParam("radioRange");
    double minAltitude = getDoubleParam("minimumAltitude");
    double lineWidth = getDoubleParam("searchLineWidth");
    double heightDiff = getDoubleParam("searchHeightDiff");

    // Setup Madara for communications.
    m_madaraController = new MadaraController(PLUGIN_CONTROLLER_ID, radioRange, minAltitude, lineWidth, heightDiff);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SMASH::SystemControllerPlugin::cleanup()
{
    simAddStatusbarMessage("SystemControllerPlugin::cleanup: Cleaning up System Controller.");

    if(m_madaraController != NULL)
    {
        delete m_madaraController;
        m_madaraController = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SMASH::SystemControllerPlugin::executeStep()
{
    //simAddStatusbarMessage("SystemControllerPlugin::executeStep: Executing step.");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string SMASH::SystemControllerPlugin::getId()
{
    return "SystemController";
}