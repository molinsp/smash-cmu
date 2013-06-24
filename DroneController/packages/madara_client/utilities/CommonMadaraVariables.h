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
// Global variables.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Network information.
#define MV_TOTAL_DEVICES		        "devices"						                // The total amount of devices in the system.
#define MV_COMM_RANGE				    "max_communication_distance"				    // The range of the high-banwidth radio.

// Information about specific devices. Read only.
#define MV_MOBILE(deviceId)			    "device." + std::string(deviceId) + ".mobile"	// Indicates if the device can fly (i.e., if it is a drone with enough battery left).

// Information about specific devices. Read and write.
#define MV_BUSY(deviceId)			    "device." + std::string(deviceId) + ".busy"		 // Indicates if the device is not available for procedures (such as coverage or bridging).
#define MV_BRIDGE_ID(deviceId)   	    "device." + std::string(deviceId) + ".bridge_id" // If bridging, indicates the id of the associated bridge.

// Movement commands.
#define MV_MOVEMENT_REQUESTED           ".movement_command"                             // Command to tell that we want certain movement.
#define MV_MOVEMENT_TARGET_LAT	        ".movement_command.0"                           // The latitude of the position the device is headed towards.
#define MV_MOVEMENT_TARGET_LON  	    ".movement_command.1"                           // The longitude of the position the device is headed towards.
#define MO_MOVE_TO_GPS_CMD              "'move_to_gps'"                                 // Command used to tell drone to move to that a location.

// Region information.
#define MV_REGION_TYPE(regionId)        "region." + std::string(regionId) + ".type"     // The type of a particular region.

// Area coverage information.
#define MV_AREA_COVERAGE_REQUESTED(deviceId) "device." + std::string(deviceId) + ".area_coverage_requested"    // Tells if this device was tasked with area coverage.
#define MV_ASSIGNED_SEARCH_AREA(deviceId)    "device." + std::string(deviceId) + ".search_area"    // The id of the area that I have been assigned to search.
#define MV_SEARCH_AREA_REGION(areaId)        "search_area." + std::string(areaId)                  // Returns the region associated to a certain search area.

// Bridge information.
#define MV_TOTAL_BRIDGES                        "bridges"                                           // The total number of bridges requested so far.
#define MV_BRIDGE_SOURCE_REGION_ID(bridgeId)    "bridge." + std::string(bridgeId) + ".endpoint.1"   // The region where one of the endpoints of the bridge is.
#define MV_BRIDGE_SINK_REGION_ID(bridgeId)      "bridge." + std::string(bridgeId) + ".endpoint.2"   // The region where the other endpoint of the bridge is.

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Local preprocessed variables, filled in by other modules in each drone.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Information about specific devices.
#define MV_MY_ID					    ".id"                                                           // The id of this device.
#define MV_DEVICE_LAT(i)			    ".device." + std::string(i) + ".location.latitude"		        // The latitude of a device with id i.
#define MV_DEVICE_LON(i)			    ".device." + std::string(i) + ".location.longitude"		        // The longtude of a device with id i.

// Information about region bounding box.
#define MV_REGION_TOPLEFT_LAT(regionId)  ".region." + std::string(regionId) + ".top_left.location.latitude"      // Latitude of top left corner of a rectangular region.
#define MV_REGION_TOPLEFT_LON(regionId)  ".region." + std::string(regionId) + ".top_left.location.longitude"     // Longitude of top left corner of a rectangular region.
#define MV_REGION_BOTRIGHT_LAT(regionId) ".region." + std::string(regionId) + ".bottom_right.location.latitude"  // Latitude of bottom right corner of a rectangular region.
#define MV_REGION_BOTRIGHT_LON(regionId) ".region." + std::string(regionId) + ".bottom_right.location.longitude" // Longitude of bottom right corner of a rectangular region.

#endif
