/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#ifdef AR_DRONE_2

#include <stdio.h>
#include <drk.h>

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

double read_thermal()
{
	double buffer[8][8];
        sem_wait(serial_buf->semaphore);
        memcpy(&buffer, &((serial_buf->grideye_buf).temperature), sizeof(buffer));
        sem_post(serial_buf->semaphore);
        
        int x, y;
        double ret = 0.0;
        for (y = 0; y < 8; y++)
        {
		for (x = 0; x < 8; x++)
		{
		
			printf("%02f ", buffer[x][y]);
		
			if (buffer[x][y] > ret)
			{
				ret = buffer[x][y];
			}
		}
		printf("\n");
	}
	return ret;
}

double human_detected()
{
	return read_thermal();
}


#endif
