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
#include "movement/platform_movement.h"
#include "sensors/platform_sensors.h"

static bool drk_init_status = false;

int frame_number;
double thermal_data[8][8];

bool init_platform()
{
	if (!drk_init_status)
	{
		drk_init();
		drk_init_status = true;
	}
	return drk_init_status;
}

bool init_sensor_functions()
{
	return drk_init_status;
}

bool init_control_functions()
{
	return drk_init_status;
}

void takeoff()
{
	printf("In AR_DRDONE_2 execute_takeoff()\n");
    drk_ar_flat_trim();
	drk_takeoff();
}
void land()
{
	printf("In AR_DRDONE_2 execute_land()\n");
    drk_hover(0);
	drk_land();
}

void move_up()
{
    printf("In AR_DRDONE_2 move_up()\n");
    drk_move_up(0.5, 1000, DRK_HOVER);
}

void move_down()
{
	printf("In AR_DRDONE_2 move_down()\n");
	drk_move_down(0.5, 1000, DRK_HOVER);
}

void move_left()
{
	printf("In AR_DRDONE_2 move_left()\n");
	drk_move_left(0.5, 1000, DRK_HOVER);
}

void move_right()
{
	printf("In AR_DRDONE_2 move_right()\n");
	drk_move_right(0.5, 1000, DRK_HOVER);
}

void move_forward()
{
	printf("In AR_DRDONE_2 move_forward()\n");
	drk_move_forward(0.5, 1000, DRK_HOVER);
}

void move_backward()
{
	printf("In AR_DRDONE_2 move_backward()\n");
	drk_move_backward(0.5, 1000, DRK_HOVER);
}

void read_thermal(double buffer[8][8])
{
	printf("in read_thermal()\n");
    sem_wait(serial_buf->semaphore);
    memcpy(&buffer, &((serial_buf->grideye_buf).temperature), sizeof(buffer));
    sem_post(serial_buf->semaphore);
    printf("done copying\n");
    
    int x, y;
    for (y = 0; y < 8; y++)
    {
		for (x = 0; x < 8; x++)
		{
			printf("in loop %d, %d\n", x, y);
			printf("%02f ", buffer[x][y]);
		}
	}
}

void read_gps(struct madara_gps * ret)
{
    printf("entering read_gps\n");
	struct gps gps= drk_gps_data();
	ret->latitude = gps.latitude;
	ret->longitude = gps.longitude;
	ret->num_sats = gps.num_sats;
    printf("leaving read_gps\n");
}

double read_ultrasound()
{
    return drk_ultrasound_altitude();
}

double get_distance_to_gps(double lat, double lon)
{
    return drk_gps_coordinates_mydistance(lat, lon);
}

/**
 * Global to be able to stop movement
 */
pthread_t moving;

void stop_movement()
{
    // from drk library
    stop = 1;
    if(moving != 0)
        pthread_join(moving, NULL);
    moving = 0;
    stop = 0;
    drk_hover(0);
}

void move_to_location(double lat, double lon, double alt)
{
	printf("entering platform::move_to_location(%08f, %08f)...\n", lat, lon);
    moving = drk_gps_goto_coordinate(lat, lon, alt, 0.1, 2, true);
    // lat, long alt, max speed, tolerance, threaded
    drk_hover(0);
}

void move_to_altitude(double alt)
{
	printf("In platform move_to_altitude(%02f)\n", alt);
    drk_goto_altitude(alt);
}

bool cleanup_platform()
{
    drk_hover(0);
    drk_land();
    drk_exit(EXIT_SUCCESS);
}

#endif
