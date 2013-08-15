/********************************************************************* 
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * CommonMadaraVariables.h - Madara variables used by the different
 * modules. These include variables that are either also used
 * by other modules, or that are required to be known for simulations.
 *********************************************************************/

#ifndef _COMMON_MADARA_VARIABLES_H
#define _COMMON_MADARA_VARIABLES_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MF_TARGET_REACHED                       "utilities_targetReached"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global variables.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Network information.
#define MV_TOTAL_DEVICES                        "devices"           // The total amount of devices in the system.
#define MV_MIN_ALTITUDE                         "device.min_alt"    // The minimum altitude to set a device to.

// Information about specific devices.
#define MV_DEVICE_LOCATION(deviceId)            "device." + std::string(deviceId) + ".location" // The location of a device in the format "lat,long".
#define MV_MOBILE(deviceId)                     "device." + std::string(deviceId) + ".mobile"   // Indicates if the device can fly (i.e., if it is a drone with enough battery left).
#define MV_BUSY(deviceId)                       "device." + std::string(deviceId) + ".busy"     // Indicates if the device is not available for procedures (such as coverage or bridging)

// Movement commands.
#define MV_DEVICE_MOVE_REQUESTED(deviceId)      "device." + std::string(deviceId) + ".movement_command" // Command to tell that we want certain movement.

// Region information.
#define MV_REGION_TYPE(regionId)                "region." + std::string(regionId) + ".type"                     // The type of a particular region.
#define MV_REGION_TOPLEFT_LOC(regionId)         "region." + std::string(regionId) + ".top_left.location"        // The location of the top left corner of the region.
#define MV_REGION_BOTRIGHT_LOC(regionId)        "region." + std::string(regionId) + ".bottom_right.location"    // The location of the top left corner of the region.

// Area coverage information.
#define MV_AREA_COVERAGE_REQUESTED(deviceId)    "device." + std::string(deviceId) + ".area_coverage_requested"      // Tells if this device was tasked with area coverage.
#define AREA_COVERAGE_RANDOM                    "random"                                                            // selects RandomAreaCoverage
#define AREA_COVERAGE_SNAKE                     "snake"                                                             // selects SnakeAreaCoverage
#define AREA_COVERAGE_INSIDEOUT                 "inside_out"                                                        // selects InsideOutAreaCoverage
#define MV_NEXT_AREA_COVERAGE_REQUEST(deviceId) "device." + std::string(deviceId) + ".next_area_coverage_requested" // next area coverage to be used
#define MV_ASSIGNED_SEARCH_AREA(deviceId)       "device." + std::string(deviceId) + ".search_area_id"               // The id of the area that I have been assigned to search.
#define MV_TOTAL_SEARCH_AREAS                   "search_areas"                                                      // The total number of search areas requested so far.
#define MV_SEARCH_AREA_REGION(areaId)           "search_area." + std::string(areaId) + ""                           // Returns the region associated to a certain search area.

// Bridge information.
#define MV_BRIDGE_REQUESTED                     "bridge.bridge_requested"                          // Tells if a bridge was requested.
#define MV_BRIDGE_ID(deviceId)                  "device." + std::string(deviceId) + ".bridge_id"   // If bridging, indicates the id of the associated bridge.
#define MV_TOTAL_BRIDGES                        "bridges"                                          // The total number of bridges requested so far.
#define MV_COMM_RANGE                           "bridge.max_communication_distance"                // The range of the high-banwidth radio, in meters.
#define MV_BRIDGE_SOURCE_REGION_ID(bridgeId)    "bridge." + std::string(bridgeId) + ".endpoint.1"  // The region where one of the endpoints of the bridge is.
#define MV_BRIDGE_SINK_REGION_ID(bridgeId)      "bridge." + std::string(bridgeId) + ".endpoint.2"  // The region where the other endpoint of the bridge is.

// Human detection information
#define MV_HUMAN_DETECTION_REQUESTED(deviceId)  "device." + std::string(deviceId) + ".human_detection_requested"        // Tells if this device was tasked with human detection.
#define HUMAN_DETECTION_BASIC                   "basic"                                                                 // Selects BasicHumanDetection strategy.
#define HUMAN_DETECTION_SLIDING_WINDOW          "sliding_window"                                                        // Selects SlidingWindowHumanDetection strategy.    
#define MV_ENVIRONMENT_TEMPERATURE              "environment.temperature"                                               // Expected environment temperature set by the user.
#define MV_LOCATION_(LAT)_(LONG)_THERMALS       "location_" + std::string(LAT) + "_" + std::string(LONG) + "_thermals"  // Location where human was detected.

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Local preprocessed variables, filled in by other modules in each drone.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Information about specific devices.
#define MV_MY_ID                            ".id"                                                   // The id of this device.
#define MV_MY_LOCATION                      ".location"                                             // My location ("x,y").
#define MV_DEVICE_LAT(i)                    ".device." + std::string(i) + ".location.latitude"      // The latitude of a device with id i.
#define MV_DEVICE_LON(i)                    ".device." + std::string(i) + ".location.longitude"     // The longtude of a device with id i.
#define MV_DEVICE_ALT(i)                    ".device." + std::string(i) + ".location.altitude"      // The longtude of a device with id i.
#define MV_ASSIGNED_ALTITUDE(deviceId)      ".device." + std::string(deviceId) + ".assigned_alt"    // Indicates the default atitude assigned to this device.

// Information about region bounding box.
#define MV_REGION_TOPLEFT_LAT(regionId)     ".region." + std::string(regionId) + ".top_left.location.latitude"      // Latitude of top left corner of a rectangular region.
#define MV_REGION_TOPLEFT_LON(regionId)     ".region." + std::string(regionId) + ".top_left.location.longitude"     // Longitude of top left corner of a rectangular region.
#define MV_REGION_BOTRIGHT_LAT(regionId)    ".region." + std::string(regionId) + ".bottom_right.location.latitude"  // Latitude of bottom right corner of a rectangular region.
#define MV_REGION_BOTRIGHT_LON(regionId)    ".region." + std::string(regionId) + ".bottom_right.location.longitude" // Longitude of bottom right corner of a rectangular region.

// Movement commands.
#define MV_MOVEMENT_REQUESTED               ".movement_command"                     // Internal command to tell that we want certain movement.
#define MV_MOVEMENT_CMD_ARG(i)              ".movement_command."  + std::string(i) + ""  // The i argument for the command, starting at i=0.

// Move to GPS.
#define MO_MOVE_TO_GPS_CMD                  "move_to_gps"           // Command used to tell drone to move to that a location.
#define MV_MOVEMENT_TARGET_LAT              ".movement_command.0"   // The latitude of the position the device is headed towards.
#define MV_MOVEMENT_TARGET_LON              ".movement_command.1"   // The longitude of the position the device is headed towards.

// Move to altitude.
#define MO_MOVE_TO_ALTITUDE_CMD             "move_to_altitude"      // Command used to tell drone to move to a specific altitude.
#define MV_MOVEMENT_TARGET_ALT              ".movement_command.0"   // The altitude of the position the device is headed towards.

// Land and takeoff
#define MO_LAND_CMD                         "land"          // Command used to tell drone to land at current location
#define MO_TAKEOFF_CMD                      "takeoff"       // Command used to tell drone to takeoff
#define MV_IS_LANDED                        ".landed"       // 1 if drone is landed, 0 otherwise
#define MV_IS_AT_ALTITUDE                   ".atAltitude"   // 1 if drone is at specified altitude, 0 otherwise

#endif
