/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "MadaraSystemController.h"
#include "utilities/CommonMadaraVariables.h"
#include <vector>

#ifdef _WIN32
  // Only include the custom transport in Windows, as it is not necessary in Linux.
  #include "Windows_Multicast_Transport.h"
#endif

// Multicast address.
#define DEFAULT_MULTICAST_ADDRESS "239.255.0.1:4150"

using namespace SMASH::Utilities;

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor, sets up a Madara knowledge base and basic values.
////////////////////////////////////////////////////////////////////////////////////////////////////////
MadaraController::MadaraController(int id, double commRange)
{
    // Start the counter at 0.
    m_regionId = 0;

    // Define the transport settings.
    m_host = "";
    m_transportSettings.hosts_.resize (1);
    m_transportSettings.hosts_[0] = DEFAULT_MULTICAST_ADDRESS;
    m_transportSettings.id = id;

    // Setup the actual transport.
#ifdef __linux
    // In Linux we can use the default Mulitcast transport.
    m_transportSettings.type = Madara::Transport::MULTICAST;
#elif defined(WIN32)
    // In Windows we need to delay the transport launch to use a custom transport.
    m_transportSettings.delay_launch = true;
#endif

    // Create the knowledge base.
    m_knowledge = new Madara::Knowledge_Engine::Knowledge_Base(m_host, m_transportSettings);

    // Setup a log.
    //m_knowledge->log_to_file("madaralog.txt", true);
    //m_knowledge->evaluate("#log_level(1)");

#ifdef _WIN32
    // In Windows we need a custom transport to avoid crashes due to incompatibilities between Win V-Rep and ACE.
    m_knowledge->attach_transport(new Windows_Multicast_Transport (m_knowledge->get_id (),
        m_knowledge->get_context (), m_transportSettings, true));
#endif
   
    // Set our id and comm range.
    m_id = id;
    m_commRange = commRange;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Destructor, simply cleans up.
////////////////////////////////////////////////////////////////////////////////////////////////////////
MadaraController::~MadaraController()
{
    terminate();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Cleanup, terminating all threads and open communications.
////////////////////////////////////////////////////////////////////////////////////////////////////////
void  MadaraController::terminate()
{
    if(m_knowledge != NULL)
    {
        m_knowledge->close_transport();
        m_knowledge->clear();
        delete m_knowledge;
        m_knowledge = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Convenience method that updates the status of the drones with the information form the simulator.
////////////////////////////////////////////////////////////////////////////////////////////////////////
void MadaraController::updateNetworkStatus(const int& numberOfDrones)
{
    // This is done just to ensure this is propagated, since we are just setting this value to the same value it already has.
    m_knowledge->set (MV_COMM_RANGE, m_commRange, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);

    // This call has no delay to flush all past changes, and updates the total of drones in the system.
    m_knowledge->set(MV_TOTAL_DEVICES, (Madara::Knowledge_Record::Integer) numberOfDrones);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sets up all the variables required for a bridge request.
////////////////////////////////////////////////////////////////////////////////////////////////////////
void MadaraController::setupBridgeRequest(int bridgeId, Position sourceTopLeft, Position sourceBottomRight, Position sinkTopLeft, Position sinkBottomRight)
{
    int rectangleType = 0;
    std::string bridgeIdString = NUM_TO_STR(bridgeId);

    // We set the total bridges to the bridge id + 1, since it starts at 0.
    int totalBridges = bridgeId + 1;
    m_knowledge->set(MV_TOTAL_BRIDGES, (Madara::Knowledge_Record::Integer) totalBridges, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);

    // Store the id of the source region for this bridge.
    int sourceRegionId = m_regionId++;
    std::string sourceRegionIdString = NUM_TO_STR(sourceRegionId);
    m_knowledge->set(MV_BRIDGE_SOURCE_REGION_ID(bridgeIdString), (Madara::Knowledge_Record::Integer) sourceRegionId, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);
    m_knowledge->set(MV_REGION_TYPE(sourceRegionIdString), (Madara::Knowledge_Record::Integer) rectangleType, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);

    // Set the bounding box of the regions. For now, the rectangle will actually just be a point.
    // NOTE: we use substring below to store the information not in the local but a global variable, which is only needed in a simulation.
    std::string sourceTopLeftLocation = sourceTopLeft.toString();
    std::string sourceBotRightLocation = sourceBottomRight.toString();
    m_knowledge->set(MV_REGION_TOPLEFT_LOC(sourceRegionIdString), sourceTopLeftLocation, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);
    m_knowledge->set(MV_REGION_BOTRIGHT_LOC(sourceRegionIdString), sourceBotRightLocation, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);

    // Store the id of the sink region for this bridge.
    int sinkRegionId = m_regionId++;
    std::string sinkRegionIdString = NUM_TO_STR(sinkRegionId);
    m_knowledge->set(MV_BRIDGE_SINK_REGION_ID(bridgeIdString), (Madara::Knowledge_Record::Integer) sinkRegionId, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);
    m_knowledge->set(MV_REGION_TYPE(sinkRegionIdString), (Madara::Knowledge_Record::Integer) rectangleType, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);

    // Set the bounding box of the regions. For now, the rectangle will actually just be a point.
    std::string sinkTopLeftLocation = sinkTopLeft.toString();
    std::string sinkBotRightLocation = sinkBottomRight.toString();
    m_knowledge->set(MV_REGION_TOPLEFT_LOC(sinkRegionIdString), sinkTopLeftLocation, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);
    m_knowledge->set(MV_REGION_BOTRIGHT_LOC(sinkRegionIdString), sinkBotRightLocation, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);

    // Indicates that we are requesting a bridge.
    // This call has no delay to flush all past changes.
    m_knowledge->set(MV_BRIDGE_REQUESTED, (Madara::Knowledge_Record::Integer) 1.0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Requests a drone to be part of area coverage.
////////////////////////////////////////////////////////////////////////////////////////////////////////
void MadaraController::requestAreaCoverage(int droneId, int searchAreaId)
{
    // Set the given search area as the area for this drone to search; and tell it to start searching.
    std::string droneIdString = NUM_TO_STR(droneId);
    m_knowledge->set(MV_ASSIGNED_SEARCH_AREA(droneIdString), (Madara::Knowledge_Record::Integer) searchAreaId, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);
    m_knowledge->set(MV_AREA_COVERAGE_REQUESTED(droneIdString), (Madara::Knowledge_Record::Integer) 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Requests a drone to be part of area coverage.
////////////////////////////////////////////////////////////////////////////////////////////////////////
void MadaraController::setNewSearchArea(int searchAreaId, SMASH::Utilities::Region areaBoundaries)
{
    // Add a new search area.
    int searchAreaRegionId = m_regionId++;
    std::string searchAreaIdString = NUM_TO_STR(searchAreaId);
    m_knowledge->set(MV_SEARCH_AREA_REGION(searchAreaIdString), (Madara::Knowledge_Record::Integer) searchAreaRegionId, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);

    // Set the type and bounding box of the region associated with this search area.
    int rectangleType = 0;
    std::string sourceRegionIdString = NUM_TO_STR(searchAreaRegionId);
    std::string topLeftLocation = areaBoundaries.topLeftCorner.toString();
    std::string botRightLocation = areaBoundaries.bottomRightCorner.toString();
    m_knowledge->set(MV_REGION_TYPE(sourceRegionIdString), (Madara::Knowledge_Record::Integer) rectangleType, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);
    m_knowledge->set(MV_REGION_TOPLEFT_LOC(sourceRegionIdString), topLeftLocation, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);
    m_knowledge->set(MV_REGION_BOTRIGHT_LOC(sourceRegionIdString), botRightLocation, Madara::Knowledge_Engine::DELAY_ONLY_EVAL_SETTINGS);

    // Update the total amount of search areas. No delay to apply all changes.
    int totalSearchAreas = searchAreaId + 1;
    m_knowledge->set(MV_TOTAL_SEARCH_AREAS, (Madara::Knowledge_Record::Integer) totalSearchAreas);
}
