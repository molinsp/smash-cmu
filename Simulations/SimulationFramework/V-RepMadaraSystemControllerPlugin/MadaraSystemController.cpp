/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#include "MadaraSystemController.h"
#include "VRepKnowledgeBaseUtils.h"
#include "utilities/CommonMadaraVariables.h"
#include "platforms/v_rep/v-rep_main_madara_transport_settings.h"
#include "utilities/string_utils.h"
#include <map>

using namespace SMASH::Utilities;

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor, sets up a Madara knowledge base and basic values.
////////////////////////////////////////////////////////////////////////////////////////////////////////
MadaraController::MadaraController(int id, double commRange, double minAltitude, double lineWidth, double heightDiff)
{
    // Start the counter at 0.
    m_regionId = 0;

    // Set our state.
    m_id = id;
    m_commRange = commRange;
    m_minAltitude = minAltitude;
    m_lineWidth = lineWidth;
    m_heightDiff = heightDiff;

    // Get a proper simulation knowledge base.
    m_knowledge = setupVRepKnowledgeBase(id, "systemcmadaralog.txt", MAIN_MULTICAST_ADDRESS);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Destructor, simply cleans up.
////////////////////////////////////////////////////////////////////////////////////////////////////////
MadaraController::~MadaraController()
{
    terminateVRepKnowledgeBase(m_knowledge);
    m_knowledge = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Convenience method that updates general parameters of thw swarm.
////////////////////////////////////////////////////////////////////////////////////////////////////////
void MadaraController::updateGeneralParameters(const int& numberOfDrones)
{
    // Set up the general parameters from the class into Madara variables.
    m_knowledge->set (MV_COMM_RANGE, m_commRange, Madara::Knowledge_Engine::Eval_Settings(true));
    m_knowledge->set (MV_MIN_ALTITUDE, m_minAltitude, Madara::Knowledge_Engine::Eval_Settings(true));
    m_knowledge->set (MV_AREA_COVERAGE_LINE_WIDTH, m_lineWidth, Madara::Knowledge_Engine::Eval_Settings(true));
    m_knowledge->set (MV_AREA_COVERAGE_HEIGHT_DIFF, m_heightDiff, Madara::Knowledge_Engine::Eval_Settings(true));
    m_knowledge->set(MV_TOTAL_DEVICES, (Madara::Knowledge_Record::Integer) numberOfDrones, Madara::Knowledge_Engine::Eval_Settings(true));
    
    // This call will flush all past changes.
    m_knowledge->apply_modified();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sets up all the variables required for a bridge request.
////////////////////////////////////////////////////////////////////////////////////////////////////////
void MadaraController::setupBridgeRequest(int bridgeId, Region startRegion, Region endRegion)
{
    int rectangleType = 0;
    std::string bridgeIdString = NUM_TO_STR(bridgeId);

    // We set the total bridges to the bridge id + 1, since it starts at 0.
    int totalBridges = bridgeId + 1;
    m_knowledge->set(MV_TOTAL_BRIDGES, (Madara::Knowledge_Record::Integer) totalBridges,
      Madara::Knowledge_Engine::Eval_Settings(true));

    // Store the id of the source region for this bridge.
    int sourceRegionId = m_regionId++;
    std::string sourceRegionIdString = NUM_TO_STR(sourceRegionId);
    m_knowledge->set(MV_BRIDGE_SOURCE_REGION_ID(bridgeIdString),
      (Madara::Knowledge_Record::Integer)sourceRegionId,
      Madara::Knowledge_Engine::Eval_Settings(true));
    m_knowledge->set(MV_REGION_TYPE(sourceRegionIdString),
      (Madara::Knowledge_Record::Integer) rectangleType,
      Madara::Knowledge_Engine::Eval_Settings(true));

    // Set the bounding box of the regions. For now, the rectangle will actually just be a point.
    // NOTE: we use substring below to store the information not in the local but a global variable, which is only needed in a simulation.
    std::string sourceTopLeftLocation = startRegion.topLeftCorner.toString();
    std::string sourceBotRightLocation = startRegion.bottomRightCorner.toString();
    m_knowledge->set(MV_REGION_TOPLEFT_LOC(sourceRegionIdString), sourceTopLeftLocation,
      Madara::Knowledge_Engine::Eval_Settings(true));
    m_knowledge->set(MV_REGION_BOTRIGHT_LOC(sourceRegionIdString), sourceBotRightLocation,
      Madara::Knowledge_Engine::Eval_Settings(true));

    // Store the id of the sink region for this bridge.
    int sinkRegionId = m_regionId++;
    std::string sinkRegionIdString = NUM_TO_STR(sinkRegionId);
    m_knowledge->set(MV_BRIDGE_SINK_REGION_ID(bridgeIdString), (Madara::Knowledge_Record::Integer) sinkRegionId,
      Madara::Knowledge_Engine::Eval_Settings(true));
    m_knowledge->set(MV_REGION_TYPE(sinkRegionIdString), (Madara::Knowledge_Record::Integer) rectangleType,
      Madara::Knowledge_Engine::Eval_Settings(true));

    // Set the bounding box of the regions. For now, the rectangle will actually just be a point.
    std::string sinkTopLeftLocation = endRegion.topLeftCorner.toString();
    std::string sinkBotRightLocation = endRegion.bottomRightCorner.toString();
    m_knowledge->set(MV_REGION_TOPLEFT_LOC(sinkRegionIdString), sinkTopLeftLocation,
      Madara::Knowledge_Engine::Eval_Settings(true));
    m_knowledge->set(MV_REGION_BOTRIGHT_LOC(sinkRegionIdString), sinkBotRightLocation,
      Madara::Knowledge_Engine::Eval_Settings(true));

    // Indicates that we are requesting a bridge.
    // This call has no delay to flush all past changes.
    m_knowledge->set(MV_BRIDGE_REQUESTED, (Madara::Knowledge_Record::Integer) 1.0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Requests a drone to be part of area coverage.
////////////////////////////////////////////////////////////////////////////////////////////////////////
void MadaraController::requestAreaCoverage(std::vector<int> droneIds, int searchAreaId, string algorithm, int wait)
{
    // Set the given search area as the area for this drone to search; and tell it to start searching.

	// Set the values for each drone.
	for(unsigned int i=0; i<droneIds.size(); i++)
	{
		std::string droneIdString = NUM_TO_STR(droneIds[i]);
		m_knowledge->set(MV_ASSIGNED_SEARCH_AREA(droneIdString), (Madara::Knowledge_Record::Integer) searchAreaId,
		  Madara::Knowledge_Engine::Eval_Settings(true)); 
		m_knowledge->set(MV_AREA_COVERAGE_REQUESTED(droneIdString), algorithm,
		  Madara::Knowledge_Engine::Eval_Settings(true));
        m_knowledge->set(MV_SEARCH_WAIT, (Madara::Knowledge_Record::Integer) wait,
		  Madara::Knowledge_Engine::Eval_Settings(true));

	}

	m_knowledge->apply_modified();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Requests a drone to be part of area coverage.
////////////////////////////////////////////////////////////////////////////////////////////////////////
void MadaraController::setNewSearchArea(int searchAreaId, SMASH::Utilities::Region areaBoundaries)
{
    // Add a new search area.
    int searchAreaRegionId = m_regionId++;
    std::string searchAreaIdString = NUM_TO_STR(searchAreaId);
    m_knowledge->set(MV_SEARCH_AREA_REGION(searchAreaIdString), (Madara::Knowledge_Record::Integer) searchAreaRegionId,
      Madara::Knowledge_Engine::Eval_Settings(true));

    // Set the type and bounding box of the region associated with this search area.
    int rectangleType = 0;
    std::string sourceRegionIdString = NUM_TO_STR(searchAreaRegionId);
    std::string topLeftLocation = areaBoundaries.topLeftCorner.toString();
    std::string botRightLocation = areaBoundaries.bottomRightCorner.toString();
    m_knowledge->set(MV_REGION_TYPE(sourceRegionIdString), (Madara::Knowledge_Record::Integer) rectangleType,
      Madara::Knowledge_Engine::Eval_Settings(true));
    m_knowledge->set(MV_REGION_TOPLEFT_LOC(sourceRegionIdString), topLeftLocation,
      Madara::Knowledge_Engine::Eval_Settings(true));
    m_knowledge->set(MV_REGION_BOTRIGHT_LOC(sourceRegionIdString), botRightLocation,
      Madara::Knowledge_Engine::Eval_Settings(true));

    // Ensure that the min altitude is sent.
    m_knowledge->set (MV_MIN_ALTITUDE, m_minAltitude, Madara::Knowledge_Engine::Eval_Settings(true));

    // Update the total amount of search areas. No delay to apply all changes.
    int totalSearchAreas = searchAreaId + 1;
    m_knowledge->set(MV_TOTAL_SEARCH_AREAS, (Madara::Knowledge_Record::Integer) totalSearchAreas);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Convenience method that updates general parameters of thw swarm.
////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<SMASH::Utilities::Position> MadaraController::getCurrentLocations()
{
    std::vector<SMASH::Utilities::Position> locations = std::vector<Position>();

    // Loop over all drones to get the current location for all of them.
    int numDrones = m_knowledge->get(MV_TOTAL_DEVICES).to_integer();
    for(int i=0; i < numDrones; i++)
    {
        Position currDroneLocation;
        std::string currDroneIdString = NUM_TO_STR(i);
        currDroneLocation.latitude = m_knowledge->get(MV_DEVICE_LAT(currDroneIdString)).to_double();
        currDroneLocation.longitude = m_knowledge->get(MV_DEVICE_LON(currDroneIdString)).to_double();
        locations.push_back(currDroneLocation);
    }

    return locations;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Convenience method that updates general parameters of thw swarm.
////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<SMASH::Utilities::Position> MadaraController::getCurrentThermals()
{
    std::vector<SMASH::Utilities::Position> locations = std::vector<Position>();

    // Loop over all drones to get the current location for all of them.
    std::map<std::string, Madara::Knowledge_Record> thermalLocations;
    m_knowledge->to_map("location_*", thermalLocations);
    
    // Transfer the locations to the vector.
    std::map<std::string, Madara::Knowledge_Record>::iterator iter;
    for (iter = thermalLocations.begin(); iter != thermalLocations.end(); ++iter)
    {
        // Split the madara variable name to get the coordinates.
        std::string locationString = iter->first;
        std::vector<std::string> variableParts = stringSplit(locationString, '_');

        // The coordinates will be in the position 1 and 2 of the split string (lat and long).
        if(variableParts.size() >= 3)
        {
            Position currThermal;
            currThermal.latitude = atof(variableParts[1].c_str());
            currThermal.latitude = atof(variableParts[2].c_str());
            locations.push_back(currThermal);
        }
    }
     
    return locations;
}
