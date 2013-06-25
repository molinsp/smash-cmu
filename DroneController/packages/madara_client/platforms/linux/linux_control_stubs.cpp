/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
#include <stdio.h>

#include "platforms/platform.h"
#include "movement/platform_movement.h"
#include "sensors/platform_sensors.h"

bool init_platform()
{
	return true;
}

bool init_sensor_functions()
{
	return true;
}

bool init_control_functions()
{
	return true;
}

void takeoff()
{
	printf("Executing takeoff()\n");
}
void land()
{
	printf("Executing land()\n");
}

void move_up()
{
	printf("Executing move_up()\n");
}

void move_down()
{
	printf("Executing move_down()\n");
}

void move_left()
{
	printf("Executing move_left()\n");
}

void move_right()
{
	printf("Executing move_right()\n");
}

void move_forward()
{
	printf("Executing move_forward()\n");
}

void move_backward()
{
	printf("Executing move_backward()\n");
}

void read_thermal(double buffer[8][8])
{
	printf("Executing read_thermal()\n");
}

void read_gps(struct madara_gps * ret)
{
	printf("In read_gps\n");
	ret->latitude = 24;
	ret->longitude = 121;
	ret->num_sats = 8;
}

void move_to_location(double lat, double lon)
{
	printf("Executing move_to_location(%02f, %02f)\n", lat, lon);
}
