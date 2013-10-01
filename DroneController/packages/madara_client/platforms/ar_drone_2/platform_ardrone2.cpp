/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#ifdef AR_DRONE_2

#include <stdio.h>

#include "drk.h"

#include "platforms/platform.h"
#include "platforms/comm/comm.h"
#include "movement/platform_movement.h"
#include "sensors/platform_sensors.h"

static bool drk_init_status = false;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Platform.h interface implementations.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool platform_init()
{
	if (!drk_init_status)
	{
		drk_init();
		drk_init_status = true;
	}
	return drk_init_status;
}

Madara::Knowledge_Engine::Knowledge_Base* platform_setup_knowledge_base(int id, bool enableLogging)
{
    // Create the knowledge base.
    Madara::Knowledge_Engine::Knowledge_Base* knowledge = comm_setup_knowledge_base(id, enableLogging);
    return knowledge;
}

bool platform_cleanup()
{
    drk_hover(0);
    drk_land();
    drk_exit(EXIT_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Platform_movement.h interface implementations.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Rate at which to move; basically speed, as a percentage of the max speed. Must be between 0 and 1.
#define RATE_TO_MOVE_DRONE      0.5

// Time for the simple movement commands to execute, in milliseconds.
#define TIME_TO_MOVE_DRONE      1000

bool platform_init_control_functions()
{
	return drk_init_status;
}

void platform_takeoff()
{
	printf("In AR_DRDONE_2 execute_takeoff()\n");
  drk_ar_flat_trim();
	drk_takeoff();
}
void platform_land()
{
	printf("In AR_DRDONE_2 execute_land()\n");
    drk_hover(0);
	drk_land();
}

void platform_move_up()
{
    printf("In AR_DRDONE_2 move_up()\n");
    drk_move_up(RATE_TO_MOVE_DRONE, TIME_TO_MOVE_DRONE, DRK_HOVER);
}

void platform_move_down()
{
	printf("In AR_DRDONE_2 move_down()\n");
	drk_move_down(RATE_TO_MOVE_DRONE, TIME_TO_MOVE_DRONE, DRK_HOVER);
}

void platform_move_left()
{
	printf("In AR_DRDONE_2 move_left()\n");
	drk_move_left(RATE_TO_MOVE_DRONE, TIME_TO_MOVE_DRONE, DRK_HOVER);
}

void platform_move_right()
{
	printf("In AR_DRDONE_2 move_right()\n");
	drk_move_right(RATE_TO_MOVE_DRONE, TIME_TO_MOVE_DRONE, DRK_HOVER);
}

void platform_move_forward()
{
	printf("In AR_DRDONE_2 move_forward()\n");
	drk_move_forward(RATE_TO_MOVE_DRONE, TIME_TO_MOVE_DRONE, DRK_HOVER);
}

void platform_move_backward()
{
	printf("In AR_DRDONE_2 move_backward()\n");
	drk_move_backward(RATE_TO_MOVE_DRONE, TIME_TO_MOVE_DRONE, DRK_HOVER);
}

void platform_stop_movement()
{
    drk_stop_movement();
}

void platform_move_to_location(double lat, double lon, double alt)
{
    printf("entering platform::move_to_location(%08f, %08f, %f)...\n", lat, lon, alt);
    double speed = 0.1;
    double tolerance = platform_get_gps_accuracy();
    drk_goto_gps(lat, lon, alt, speed, tolerance);
}

void platform_move_to_altitude(double alt)
{
	printf("In platform move_to_altitude(%02f)\n", alt);
    drk_goto_altitude(alt);
}

bool platform_location_reached()
{
    // TODO: implement this when there is a function for this in the DroneRK API. Currently not being used.
    return false;
}

bool platform_altitude_reached()
{
    // TODO: implement this when there is a function for this in the DroneRK API. Currently not being used.
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Platform_sensors.h interface implementations.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Where to stop using ultrasound readings, since higher than this they are very innaccurate.
#define ULTRASOUND_LIMIT    5.5       

bool platform_init_sensor_functions()
{
	return drk_init_status;
}

double platform_get_battery_remaining()
{
    int batteryLevel = drk_get_battery();
    return batteryLevel;
}

void platform_read_thermal(double buffer[8][8])
{
  printf("in read_thermal()\n");
  int row, col;
  sem_wait(serial_buf->semaphore);
  //memcpy(&buffer, &((serial_buf->grideye_buf).temperature), sizeof(buffer));
  for (row = 0; row < 8; row++)
  {
		for (col = 0; col < 8; col++)
			buffer[row][col] = serial_buf->grideye_buf.temperature[row][col];
	}
  sem_post(serial_buf->semaphore);
  printf("done copying\n");
    
  /*int x, y;
  for (y = 0; y < 8; y++)
  {
		for (x = 0; x < 8; x++)
		{
			printf("in loop %d, %d\n", x, y);
			printf("%02f ", buffer[x][y]);
		}
	}*/
}

void platform_read_gps(struct madara_gps * ret)
{
    printf("entering read_gps\n");
	struct gps gps = drk_gps_data();
	ret->latitude = gps.latitude;
	ret->longitude = gps.longitude;
    ret->altitude = gps.altitude;
	ret->num_sats = gps.num_sats;
    printf("leaving read_gps\n");
}

double platform_get_altitude()
{
    // By default we get the ultrasound height. 
    double ultrasoundAltitude = drk_ultrasound_altitude();

    // The ultrasound sensor has a max limit over which it will not provide meaningful results.
    // Check if we are within this range or not.
    double currentAltitude = 0;
    if(ultrasoundAltitude < ULTRASOUND_LIMIT)
    {
        // If we are below the ultrasound threshold, we can simply get the altitude it provides.
        currentAltitude = ultrasoundAltitude;
    }
    else
    {
        // If we are higher than the ultrasound sensor's limits, this height can't  be
        // trusted, and we should get the GPS-provided altitude instead.
        struct gps gpsData = drk_gps_data();
        double gpsAltitude = gpsData.altitude;
        currentAltitude = gpsAltitude;
    }

    return currentAltitude;
}

// Gets the accuracy of the GPS for this platform, in meters.
double platform_get_gps_accuracy()
{
    return 7.5;
}

#endif
