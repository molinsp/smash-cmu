/********************************************************************* 
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * CommonMadaraBridgeVariables.h - Madara variables used by the bridge
 * control module. These include variables that are either also used
 * by other modules, or that are required to be known for simulations.
 *********************************************************************/

#ifndef _COMMON_MADARA_BRIDGE_VARIABLES_H
#define _COMMON_MADARA_BRIDGE_VARIABLES_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global variables.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Network information.
#define MV_TOTAL_DEVICES		        "devices"						                // The total amount of devices in the system.
#define MV_COMM_RANGE				    "max_communication_distance"				    // The range of the high-banwidth radio.

// Information about specific devices. Read only.
#define MV_MOBILE(i)				    "device." + std::string(i) + ".mobile"		    // Indicates if the device can fly (i.e., if it is a drone with enough battery left).

// Information about specific devices. Read and write.
#define MV_BUSY(i)				        "device." + std::string(i) + ".busy"		    // Indicates if the device is not available for procedures (such as coverage or bridging).
#define MV_BRIDGE_ID(i)				    "device." + std::string(i) + ".bridge_id"	    // If bridging, indicates the id of the associated bridge.
#define MV_DEVICE_TARGET_LAT(i)	        "device." + std::string(i) + ".target.latitude" // The latitude of the position the device is headed towards.
#define MV_DEVICE_TARGET_LON(i)	        "device." + std::string(i) + ".target.longitude"// The longitude of the position the device is headed towards.

// Region information.
#define MV_REGION_TYPE(regionId)        "region." + std::string(regionId) + ".type"     // The type of a particular region.

// Bridge information.
#define MV_TOTAL_BRIDGES                        "bridges"                                           // The total number of bridges requested so far.
#define MV_BRIDGE_SOURCE_REGION_ID(bridgeId)    "bridge." + std::string(bridgeId) + ".endpoint.1"   // The region where one of the endpoints of the bridge is.
#define MV_BRIDGE_SINK_REGION_ID(bridgeId)      "bridge." + std::string(bridgeId) + ".endpoint.2"   // The region where the other endpoint of the bridge is.

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Local preprocessed variables, filled in by other modules in each drone.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Information about specific devices.
#define MV_MY_ID					    ".id"                                                           // The id of this device.
#define MV_DEVICE_LAT(i)			    ".device." + std::string(i) + ".latitude"		                // The latitude of a device with id i.
#define MV_DEVICE_LON(i)			    ".device." + std::string(i) + ".longitude"		                // The longtude of a device with id i.

// Information about region bounding box.
#define MV_REGION_TOPLEFT_LAT(regionId)  ".region." + std::string(regionId) + ".top_left.latitude"      // Latitude of top left corner of a rectangular region.
#define MV_REGION_TOPLEFT_LON(regionId)  ".region." + std::string(regionId) + ".top_left.longitude"     // Longitude of top left corner of a rectangular region.
#define MV_REGION_BOTRIGHT_LAT(regionId) ".region." + std::string(regionId) + ".bottom_right.latitude"  // Latitude of bottom right corner of a rectangular region.
#define MV_REGION_BOTRIGHT_LON(regionId) ".region." + std::string(regionId) + ".bottom_right.longitude" // Longitude of bottom right corner of a rectangular region.

#endif
