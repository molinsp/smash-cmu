/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
#include <stdio.h>

#include "platforms/platform.h"
#include "platforms/comm/comm.h"
#include "movement/platform_movement.h"
#include "sensors/platform_sensors.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Platform.h interface implementations.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool platform_init()
{
	return true;
}

Madara::Knowledge_Engine::Knowledge_Base* platform_setup_knowledge_base(int id)
{    
    Madara::Knowledge_Engine::Knowledge_Base* knowledge = comm_setup_knowledge_base(id, false);
    return knowledge;
}

bool platform_cleanup()
{
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Platform_movement.h interface implementations.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool platform_init_control_functions()
{
	return true;
}

void platform_takeoff()
{
	printf("Executing takeoff()\n");
}
void platform_land()
{
	printf("Executing land()\n");
}

void platform_move_up()
{
	printf("Executing move_up()\n");
}

void platform_move_down()
{
	printf("Executing move_down()\n");
}

void platform_move_left()
{
	printf("Executing move_left()\n");
}

void platform_move_right()
{
	printf("Executing move_right()\n");
}

void platform_move_forward()
{
	printf("Executing move_forward()\n");
}

void platform_move_backward()
{
	printf("Executing move_backward()\n");
}

void platform_stop_movement()
{
    printf("Executing stop_movement()\n");
}

void platform_move_to_location(double lat, double lon)
{
	printf("Executing move_to_location(%02f, %02f)\n", lat, lon);
}

void platform_move_to_altitude(double alt)
{
	printf("Executing platform move_to_altitude(%02f)\n", alt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Platform_sensors.h interface implementations.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool platform_init_sensor_functions()
{
	return true;
}

void platform_read_thermal(double buffer[8][8])
{
	printf("Executing platform_read_thermal()\n");
}

void platform_read_gps(struct madara_gps * ret)
{
	printf("In platform_read_gps\n");
	ret->latitude = 24;
	ret->longitude = 121;
	ret->num_sats = 8;
}

double platform_read_ultrasound()
{
    printf("Executing platform_read_ultrasound()\n");
    return 2.0;
}

double platform_get_gps_accuracy()
{
    printf("Executing platform_get_gps_accuracy()\n");
    return 1.0;
}

