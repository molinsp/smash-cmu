/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#ifdef AR_DRONE_2

#include <stdio.h>

#include "transport/DroneRK_Transport.h"
#include "drk.h"

#include "platforms/platform.h"
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

Madara::Knowledge_Engine::Knowledge_Base* platform_setup_knowledge_base(int id)
{
    // should move this to init_platform
    Madara::Transport::Settings settings;
    settings.id = id;
    settings.hosts_.resize (1);
    settings.hosts_[0] = "192.168.1.255:15000";
    settings.type = Madara::Transport::BROADCAST;
    //settings.type = Madara::Transport::NO_TRANSPORT;
    settings.queue_length = 1024; //Smaller queue len to preserve memory

    // Name the host based on the drone id.
    char host[30];
    sprintf(host, "drone%d", id);

    // Create the knowledge base.
    Madara::Knowledge_Engine::Knowledge_Base* knowledge = new Madara::Knowledge_Engine::Knowledge_Base(host, settings);

    //knowledge->attach_transport(new DroneRK_Transport(out.str(),
    //knowledge->get_context(), settings, true, 500));
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
    drk_move_up(0.5, 1000, DRK_HOVER);
}

void platform_move_down()
{
	printf("In AR_DRDONE_2 move_down()\n");
	drk_move_down(0.5, 1000, DRK_HOVER);
}

void platform_move_left()
{
	printf("In AR_DRDONE_2 move_left()\n");
	drk_move_left(0.5, 1000, DRK_HOVER);
}

void platform_move_right()
{
	printf("In AR_DRDONE_2 move_right()\n");
	drk_move_right(0.5, 1000, DRK_HOVER);
}

void platform_move_forward()
{
	printf("In AR_DRDONE_2 move_forward()\n");
	drk_move_forward(0.5, 1000, DRK_HOVER);
}

void platform_move_backward()
{
	printf("In AR_DRDONE_2 move_backward()\n");
	drk_move_backward(0.5, 1000, DRK_HOVER);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Platform_sensors.h interface implementations.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool platform_init_sensor_functions()
{
	return drk_init_status;
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
	struct gps gps= drk_gps_data();
	ret->latitude = gps.latitude;
	ret->longitude = gps.longitude;
	ret->num_sats = gps.num_sats;
    printf("leaving read_gps\n");
}

double platform_read_ultrasound()
{
    return drk_ultrasound_altitude();
}

// Gets the accuracy of the GPS for this platform, in meters.
double platform_get_gps_accuracy()
{
    return 7.5;
}

#endif
