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
#include "gps_utils.h"
#include <string>
#include <vector>

using namespace SMASHSim;
using namespace VREP;

// Id for a controller, a plugin must have this value as its Madara id.
const int PLUGIN_CONTROLLER_ID = 202;

// The name of the UI with button commands for the System Controller.
const std::string SYSTEM_CONTROLLER_COMMANDS_UI_NAME = "commandsUI";
const std::string SYSTEM_CONTROLLER_COMMAND_SETUP_PARAMS = "Setup Network";
const std::string SYSTEM_CONTROLLER_COMMAND_TAKEOFF = "Take Off";
const std::string SYSTEM_CONTROLLER_COMMAND_LAND = "Land";
const std::string SYSTEM_CONTROLLER_COMMAND_START_SEARCH = "Start Search";
const std::string SYSTEM_CONTROLLER_COMMAND_START_BRIDGE = "Form Bridge";

// The name of the system controller object in the simulation; the trailing hash is to ensure we don't get any other instances, if any.
const std::string SYSTEM_CONTROLLER_OBJECT_NAME = "laptop#";

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// We have to create an actual object of this type so that the plugin DLL entry points will have access to it.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
VREP::ISimplePlugin* createPlugin()
{
    SystemControllerPlugin* plugin = new SystemControllerPlugin();
    return plugin;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ISimpleInterface: Called when plugin is initialized.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SystemControllerPlugin::initialize(int suffix)
{
    simAddStatusbarMessage("SystemControllerPlugin::initialize: Initializing System Controller.");

    m_searchRequestId = 0;
    m_bridgeRequestId = 0;

    // Setup Madara for communications.
    m_madaraController = new MadaraController(PLUGIN_CONTROLLER_ID, Madara::Transport::MULTICAST);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ISimpleInterface: Called when plugin is closing.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SystemControllerPlugin::cleanup(int suffix)
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
void SystemControllerPlugin::executeStep(int suffix)
{
    //simAddStatusbarMessage("SystemControllerPlugin::executeStep: Executing step.");
    handleNewCommand();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ISimpleInterface: Returns a textual ID of this plugin.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string SystemControllerPlugin::getId()
{
    return "SystemController";
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Handle button presses.
///////////////////////////////////////////////////////////////////////////////////////////////
void SystemControllerPlugin::handleNewCommand()
{
    std::string buttonPressedText = PluginUtils::getButtonPressedText(SYSTEM_CONTROLLER_COMMANDS_UI_NAME);
    if(buttonPressedText != "")
    {
        // Send network-wide parameters (radio range, num drones, min height).
        if(buttonPressedText == SYSTEM_CONTROLLER_COMMAND_SETUP_PARAMS)
        {
            // Get the various user setup parameters.
            int numDrones = PluginUtils::getIntParam("numberOfDrones");
            double radioRange = PluginUtils::getDoubleParam("radioRange");
            double minAltitude = PluginUtils::getDoubleParam("minimumAltitude");
            double heightDiff = PluginUtils::getDoubleParam("heightDiff");
            double coverageTrackingEnabled = PluginUtils::getIntParam("coverageTracking");
            double coverageTrackingFileEnabled = PluginUtils::getIntParam("coverageTrackingFile");
            double sensorAngle = PluginUtils::getDoubleParam("sensorAngle");

            // Send the parameters.
            m_madaraController->updateGeneralParameters(numDrones, radioRange, 
              minAltitude, heightDiff, coverageTrackingEnabled, 
              coverageTrackingFileEnabled, sensorAngle);
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
            sendBridgeRequestForLastPersonFound();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Sets all variables to be sent to the drones for a search request.
///////////////////////////////////////////////////////////////////////////////////////////////
void SystemControllerPlugin::sendSearchRequest()
{
    // Setup the search area.
    int areaId = setupSearchArea();

    // Add the drones to the search area, and tell them to go search.
    double numDrones = PluginUtils::getIntParam("numberOfDrones");
    sendSearchRequestToDrones(numDrones, areaId);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Sets up the search area for the whole network.
///////////////////////////////////////////////////////////////////////////////////////////////
int SystemControllerPlugin::setupSearchArea()
{
    // Set up the search area, getting the boundaries from the parameters.
    double x1 = PluginUtils::getDoubleParam("x1");
    double y1 = PluginUtils::getDoubleParam("y1");
    double x2 = PluginUtils::getDoubleParam("x2");
    double y2 = PluginUtils::getDoubleParam("y2");

    // Print params we got.
    std::stringstream sstream;
    sstream << std::setprecision(10) << "Search area in meters: " << x1 << "," << y1 << ";" << x2 << "," << y2;
    simAddStatusbarMessage(sstream.str().c_str());

    // Transform the x and y positions we have into lat and long using the reference point.
    SMASH::Utilities::Position refPoint = getReferencePoint();
    SMASH::Utilities::Position startPoint = SMASH::Utilities::getLatAndLong(x1, y1, refPoint);
    SMASH::Utilities::Position endPoint = SMASH::Utilities::getLatAndLong(x2, y2, refPoint);
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
void SystemControllerPlugin::sendSearchRequestToDrones(int numDrones, int areaId)
{
  // Get configurable parameters for the search.
  std::string coverageAlgorithm = PluginUtils::getStringParam("coverageAlgorithm");
  std::string humanDetectionAlgorithm = PluginUtils::getStringParam("humanDetectionAlgorithm");
    double lineWidth = PluginUtils::getDoubleParam("searchLineWidth");
  int waitForRest = PluginUtils::getIntParam("waitForRest");

    // Print info of what we got.
    std::stringstream sstream1;
    sstream1 << "Search params: coverage: " << coverageAlgorithm << "; human: " << humanDetectionAlgorithm 
            << "; lineWidth: " << lineWidth << "; waitForRest: " << waitForRest << std::endl;
    simAddStatusbarMessage(sstream1.str().c_str());
    
    // Load the drone ids into a vector. We assume that numDrones is equivalent to the max id of the drones in the simulation,
    // and that all drone ids are sequential starting from 0.
    std::vector<int> droneIds = std::vector<int>();
    for(int currDroneId = 0; currDroneId < numDrones; currDroneId++)
    {
        droneIds.push_back(currDroneId);
    }

    // Print info of what we got.
    std::stringstream sstream;
    sstream << "Drone ids size: " << droneIds.size();
    simAddStatusbarMessage(sstream.str().c_str());
    
    // Ask Madara to send the search request.
    m_madaraController->requestAreaCoverage(droneIds, areaId, coverageAlgorithm, waitForRest, lineWidth, humanDetectionAlgorithm);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Sends a bridge request to the network.
///////////////////////////////////////////////////////////////////////////////////////////////
void SystemControllerPlugin::sendBridgeRequestForLastPersonFound()
{
  // Only do this if at least one person has been found.
    std::string personFoundName = PluginUtils::getStringParam("personFoundName");
  if(personFoundName != "") 
    {
    // Get sink and source info.
        SMASH::Utilities::Position personPosition = getObjectPositionInDegrees(personFoundName);
    SMASH::Utilities::Position controllerPosition = getObjectPositionInDegrees(SYSTEM_CONTROLLER_OBJECT_NAME);

        // Create the regions, which will basically be a point each, for now at least.
        SMASH::Utilities::Region startRegion(personPosition, personPosition);
        SMASH::Utilities::Region endRegion(controllerPosition, controllerPosition);
    
    // Do the external bridge request.
    simAddStatusbarMessage("Sending bridge request for last person found.");
        m_madaraController->setupBridgeRequest(m_bridgeRequestId++, startRegion, endRegion);
    }
  else
    {
    simAddStatusbarMessage("No person found yet!");
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////    
// Returns the position of a given object in degrees.
/////////////////////////////////////////////////////////////////////////////////////////////
SMASH::Utilities::Position SystemControllerPlugin::getObjectPositionInDegrees(std::string objectName)
{
    // Get the handle for the object.
    int objectHandle = simGetObjectHandle(objectName.c_str());
    if(objectHandle == -1)
    {
        // Not very safe, we return a position with 0,0 as the value if the handle was not found.
        simAddStatusbarMessage(("Object " + objectName + " not found!").c_str());
        return SMASH::Utilities::Position(0,0);
    }

    // Get the cartesian position first.
    float vrepPosition[3];
    int relativeTo = -1; // Absolute position.
    simGetObjectPosition(objectHandle, relativeTo, vrepPosition);
    SMASH::Utilities::Position cartesianPosition;
    int x = vrepPosition[0];
    int y = vrepPosition[1];

    // Get the long and lat now from the cartesian position.
    SMASH::Utilities::Position latAndLong = SMASH::Utilities::getLatAndLong(x, y, getReferencePoint());
    return latAndLong;
}

/////////////////////////////////////////////////////////////////////////////////////////////    
// Gets the reference point for the coordinate translation, from the scene parameters.
/////////////////////////////////////////////////////////////////////////////////////////////
SMASH::Utilities::Position SystemControllerPlugin::getReferencePoint()
{
    double refLat = PluginUtils::getDoubleParam("referenceLat");
    double refLong = PluginUtils::getDoubleParam("referenceLong");
    SMASH::Utilities::Position refPoint;
    refPoint.latitude = refLat;
    refPoint.longitude = refLong;

    return refPoint;
}
