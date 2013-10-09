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
#include "PluginUtils.h"
#include "utilities/gps_utils.h"
#include <string>
#include <vector>

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
VREP::ISimplePlugin* createPlugin()
{
    VREP::SystemControllerPlugin* plugin = new VREP::SystemControllerPlugin();
    return plugin;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ISimpleInterface: Called when plugin is initialized.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void VREP::SystemControllerPlugin::initialize(int suffix)
{
    simAddStatusbarMessage("SystemControllerPlugin::initialize: Initializing System Controller.");

    m_searchRequestId = 0;
    m_bridgeRequestId = 0;

    // Setup Madara for communications.
    m_madaraController = new MadaraController(PLUGIN_CONTROLLER_ID);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ISimpleInterface: Called when plugin is closing.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void VREP::SystemControllerPlugin::cleanup()
{
    simAddStatusbarMessage("SystemControllerPlugin::cleanup: Cleaning up System Controller.");

    if(m_madaraController != NULL)
    {
        delete m_madaraController;
        m_madaraController = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ISimpleInterface: Called in each step of the simulation.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void VREP::SystemControllerPlugin::executeStep()
{
    //simAddStatusbarMessage("SystemControllerPlugin::executeStep: Executing step.");
    handleNewCommand();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ISimpleInterface: Returns a textual ID of this plugin.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string VREP::SystemControllerPlugin::getId()
{
    return "SystemController";
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Handle button presses.
///////////////////////////////////////////////////////////////////////////////////////////////
void VREP::SystemControllerPlugin::handleNewCommand()
{
    std::string buttonPressedText = PluginUtils::getButtonPressedText(SYSTEM_CONTROLLER_COMMANDS_UI_NAME);
    if(buttonPressedText != "")
    {
        // Send network-wide parameters (radio range, num drones, min height).
        if(buttonPressedText == SYSTEM_CONTROLLER_COMMAND_SETUP_PARAMS)
        {
            // Get the various user setup parameters.
            double numDrones = (int) PluginUtils::getDoubleParam("numberOfDrones");
            double radioRange = PluginUtils::getDoubleParam("radioRange");
            double minAltitude = PluginUtils::getDoubleParam("minimumAltitude");
            double heightDiff = PluginUtils::getDoubleParam("heightDiff");

            // Send the parameters.
            m_madaraController->updateGeneralParameters(numDrones, radioRange, minAltitude, heightDiff);
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
            sendSearchRequest();
        }
		
        // Start a bridge request if that button was pressed.
        if(buttonPressedText == SYSTEM_CONTROLLER_COMMAND_START_BRIDGE)
        {
            //sendBridgeRequestForLastPersonFound();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Sets all variables to be sent to the drones for a search request.
///////////////////////////////////////////////////////////////////////////////////////////////
void VREP::SystemControllerPlugin::sendSearchRequest()
{
    // Setup the search area.
    int areaId = setupSearchArea();

    // Add the drones to the search area, and tell them to go search.
    double numDrones = (int) PluginUtils::getDoubleParam("numberOfDrones");
    sendSearchRequestToDrones(numDrones, areaId);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Sets up the search area for the whole network.
///////////////////////////////////////////////////////////////////////////////////////////////
int VREP::SystemControllerPlugin::setupSearchArea()
{
    // Set up the search area, getting the boundaries from the parameters.
    double x1 =  PluginUtils::getDoubleParam("x1");
    double y1 = PluginUtils::getDoubleParam("y1");
    double x2 = PluginUtils::getDoubleParam("x2");
    double y2 = PluginUtils::getDoubleParam("y2");

    // This reference point is chosen to get better latitudes. Now it is at CMU.
    SMASH::Utilities::Position referencePoint;
    referencePoint.latitude = 40.44108;
    referencePoint.longitude = -79.947164;

    // Transform the x and y positions we have into lat and long using the reference point.
    SMASH::Utilities::Position startPoint = SMASH::Utilities::getLatAndLong(x1, y1, referencePoint);
    SMASH::Utilities::Position endPoint = SMASH::Utilities::getLatAndLong(x2, y2, referencePoint);
    simAddStatusbarMessage(std::string("Search area: " + startPoint.toString() + "; " + endPoint.toString()).c_str());

    // Create a region with these endpoints.
    SMASH::Utilities::Region searchArea(startPoint, endPoint);

    // Set the area in Madara.
    int searchAreaId = m_searchRequestId++;
    m_madaraController->setNewSearchArea(searchAreaId, searchArea);

    return searchAreaId;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Adds drones to a search area, by requesting that out of each of them.
///////////////////////////////////////////////////////////////////////////////////////////////
void VREP::SystemControllerPlugin::sendSearchRequestToDrones(int numDrones, int areaId)
{
	// Get configurable parameters for the search.
	std::string coverageAlgorithm = PluginUtils::getStringParam("coverageAlgorithm");
	std::string humanDetectionAlgorithm = PluginUtils::getStringParam("humanDetectionAlgorithm");
    double lineWidth = PluginUtils::getDoubleParam("searchLineWidth");
	int waitForRest = (int) PluginUtils::getDoubleParam("waitForRest");
    
    // Load the drone ids into a vector. We assume that numDrones is equivalent to the max id of the drones in the simulation,
    // and that all drone ids are sequential starting from 0.
    std::vector<int> droneIds = std::vector<int>(numDrones);
    for(int currDroneId = 0; currDroneId < numDrones; currDroneId++)
    {
        droneIds.push_back(currDroneId);
    }
    simAddStatusbarMessage(std::string("Drone ids size: " +numDrones).c_str());
    
    // Ask Madara to send the search request.
    m_madaraController->requestAreaCoverage(droneIds, areaId, coverageAlgorithm, waitForRest, lineWidth, humanDetectionAlgorithm);
}
