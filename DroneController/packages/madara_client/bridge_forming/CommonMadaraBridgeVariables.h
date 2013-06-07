/********************************************************************* 
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * CommonMadaraBridgeVariables.h - Common Madara variables used for bridging.
 *********************************************************************/

#ifndef _COMMON_MADARA_BRIDGE_VARIABLES_H
#define _COMMON_MADARA_BRIDGE_VARIABLES_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Madara Variable Definitions
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Network status.
#define MV_TOTAL_DRONES		        "devices"						            /* The total amount of drones in the system. */
#define MV_COMM_RANGE				"max_communication_distance"				/* The range of the high-banwidth radio, in meters. */

// Device status.
#define MV_MY_ID					".id"
#define MV_MOBILE(i)				"device." + std::string(i) + ".mobile"				/* True of drone with ID i is flying and available for bridging. */

// Bridge status.
#define MV_BRIDGE_SOURCE_REGION_ID(bridgeId)    "bridge." + std::string(bridgeId) + ".endpoint.1"  /* The region where one of the endpoints of the bridge is. */
#define MV_BRIDGE_SINK_REGION_ID(bridgeId)      "bridge." + std::string(bridgeId) + ".endpoint.2"  /* The region where one of the endpoints of the bridge is. */
#define MV_REGION_TYPE(regionId)                "region." + std::string(regionId) + ".type"

// Bridge output.
#define MV_DRONE_TARGET_POSX(i)		"device." + std::string(i) + ".target_pos.x"		/* The x target position of a drone with ID i, where it should head to, in meters. */
#define MV_DRONE_TARGET_POSY(i)		"device." + std::string(i) + ".target_pos.y"		/* The y target position of a drone with ID i, where it should head to, in meters. */

// TO REMAP
#define MV_USER_BRIDGE_REQUEST_ID	"user_bridge_request.request_id"			/* Identifies a unique bridge request, along with its bridge. */
#define MV_USER_BRIDGE_REQUEST_ON	"user_bridge_request.enabled"				/* Being true triggers the bridge behavior. */
#define MV_BRIDGING(i)				"device." + std::string(i) + ".bridging"			/* True if drone with ID i is bridging. */
#define MV_BRIDGE_ID(i)				"device." + std::string(i) + ".bridge_id"			/* If bridging, indicates the ID of the associated request or bridge. */

// Local, preprocessed variables.
#define MV_DRONE_POSX(i)			"device." + std::string(i) + ".latitude"		    /* The x position of a drone with ID i, in meters. */
#define MV_DRONE_POSY(i)			"device." + std::string(i) + ".longitude"		    /* The y position of a drone with ID i, in meters. */

#define MV_REGION_TOPLEFT_LAT(regionId)  "region." + std::string(regionId) + ".top_left.latitude"
#define MV_REGION_TOPLEFT_LON(regionId)  "region." + std::string(regionId) + ".top_left.longitude"
#define MV_REGION_BOTRIGHT_LAT(regionId) "region." + std::string(regionId) + ".bottom_right.latitude"
#define MV_REGION_BOTRIGHT_LON(regionId) "region." + std::string(regionId) + ".bottom_right.longitude"

// Internal variables:
#define MV_AVAILABLE_DRONES_AMOUNT	".available_drones"					// The amount of available drones.
#define MV_AVAILABLE_DRONES_IDS		".available_drones_ids"				// Array of the ids of the available drones.
#define MV_AVAILABLE_DRONES_POSX	".available_drones_position_x"		// Array of the x part of the position of the drones indicated by .available_drones_ids.
#define MV_AVAILABLE_DRONES_POSY	".available_drones_position_y"		// Array of the y part of the position of the drones indicated by .available_drones_ids.

#endif
