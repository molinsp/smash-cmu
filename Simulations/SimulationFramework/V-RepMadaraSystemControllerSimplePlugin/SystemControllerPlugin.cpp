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

// The name of the UI with button commands for the System Controller.
const std::string SYSTEM_CONTROLLER_COMMANDS_UI_NAME = "commandsUI";
const std::string SYSTEM_CONTROLLER_COMMAND_SETUP_PARAMS = "Setup Network";
const std::string SYSTEM_CONTROLLER_COMMAND_TAKEOFF = "Take Off";
const std::string SYSTEM_CONTROLLER_COMMAND_LAND = "Land";
const std::string SYSTEM_CONTROLLER_COMMAND_START_SEARCH = "Start Search";
const std::string SYSTEM_CONTROLLER_COMMAND_START_BRIDGE = "Form Bridge";

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

///////////////////////////////////////////////////////////////////////////////////////////////
// Check for button presses, and return its text.
///////////////////////////////////////////////////////////////////////////////////////////////
std::string getButtonPressedText(std::string uiName)
{
    // Get the details of the last button press, if any.
    int commandsUIHandle = simGetUIHandle(uiName.c_str());
    int eventDetails[2];
    int buttonHandle = simGetUIEventButton(commandsUIHandle, eventDetails);

    // If the button handle is valid and the second event detail is 0, it means a button was released.
    bool buttonEventHappened = (buttonHandle != -1) ;
    bool buttonWasReleased = (eventDetails[1] == 0);
    if(buttonEventHappened && buttonWasReleased)
    {
        // Get the text of the button that was pressed.
        char* buttonText = simGetUIButtonLabel(commandsUIHandle, buttonHandle);
        std::string buttonTextString(buttonText);
        simReleaseBuffer(buttonText);

        simAddStatusbarMessage(("Button pressed and released! " + buttonTextString).c_str());        
        return buttonTextString;
    }

    return "";
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Check for button presses.
///////////////////////////////////////////////////////////////////////////////////////////////
void SMASH::SystemControllerPlugin::handleNewCommand()
{
    std::string buttonPressedText = getButtonPressedText(SYSTEM_CONTROLLER_COMMANDS_UI_NAME);
    if(buttonPressedText != "")
    {
        // Send network-wide parameters (radio range, num drones, min height).
        if(buttonPressedText == SYSTEM_CONTROLLER_COMMAND_SETUP_PARAMS)
        {
            m_madaraController->updateGeneralParameters(m_numDrones);
        }

        // Send network-wide parameters (radio range, num drones, min height).
        if(buttonPressedText == SYSTEM_CONTROLLER_COMMAND_TAKEOFF)
        {
            m_madaraController->sendTakeoffCommand();
        }

        // Send network-wide parameters (radio range, num drones, min height).
        if(buttonPressedText == SYSTEM_CONTROLLER_COMMAND_LAND)
        {
            m_madaraController->sendLandCommand();
        }
		
        // Start a search request if that button was pressed.
        if(buttonPressedText == SYSTEM_CONTROLLER_COMMAND_START_SEARCH)
        {
            //sendSearchRequest();
        }
		
        // Start a bridge request if that button was pressed.
        if(buttonPressedText == SYSTEM_CONTROLLER_COMMAND_START_BRIDGE)
        {
            //sendBridgeRequestForLastPersonFound();
        }
    }
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
    handleNewCommand();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string SMASH::SystemControllerPlugin::getId()
{
    return "SystemController";
}