/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#ifdef AR_DRONE_2

#include <stdio.h>
#include <drk.h>

#include "control_functions.h"

bool init_control_functions()
{
	drk_init();
	return true;
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
	drk_move_up(0.5, 1000, DRK_HOVER);
}

void move_down()
{
	drk_move_down(0.5, 1000, DRK_HOVER);
}

void move_left()
{
	drk_move_left(0.5, 1000, DRK_HOVER);
}

void move_right()
{
	drk_move_right(0.5, 1000, DRK_HOVER);
}


#endif
