/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#ifdef AR_DRONE_2

#include <stdio.h>
#include <drk.h>

#include "platform_functions.h"

static bool drk_init_status = false;

static int frame_number;
static double thermal_data[8][8];

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
	drk_takeoff();
}
void land()
{
	printf("In AR_DRDONE_2 execute_land()\n");
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

void read_thermal()
{
	printf("in read_thermal()\n");
	sem_wait(serial_buf->semaphore);
	memcpy(&thermal_data, &((serial_buf->grideye_buf).temperature), sizeof(thermal_data));
	frame_number = serial_buf->grideye_buf.index;
	sem_post(serial_buf->semaphore);
	printf("done in read_thermal()\n");
}

double human_detected()
{

	printf("in human_detected()\n");

	read_thermal();
	double max = 0.0;
	for (int x = 0; x < 8; ++x)
	{
		for (int y = 0; y < 8; ++y)
		{
			printf("%f ", thermal_data[x][y]);
		
			if(thermal_data[x][y] > max)
				max = thermal_data[x][y];
		}
		printf("\n");
	}
	return max;
}


#endif
