/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/
#include <stdio.h>

#include "platforms/platform.h"
#include "platforms/kb_setup.h"
#include "movement/platform_movement.h"
#include "sensors/platform_sensors.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Platform.h interface implementations.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool platform_init()
{
  return true;
}

Madara::Knowledge_Engine::Knowledge_Base* platform_setup_knowledge_base(int id, bool enableLogging)
{    
    Madara::Knowledge_Engine::Knowledge_Base* knowledge = setup_knowledge_base(id, enableLogging, Madara::Transport::MULTICAST);
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

void platform_move_to_location(double lat, double lon, double distance)
{
  printf("Executing move_to_location(%02f, %02f, %02f)\n", lat, lon, distance);
}

void platform_move_to_altitude(double alt)
{
  printf("Executing platform move_to_altitude(%02f)\n", alt);
}

bool platform_location_reached()
{
    printf("Checking if location has been reached.\n");
    return false;
}

bool platform_altitude_reached()
{
    printf("Checking if altitude has been reached.\n");
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Platform_sensors.h interface implementations.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool platform_init_sensor_functions()
{
  return true;
}

double platform_get_battery_remaining()
{
    // Because we are not running on batteries...
    return 100;
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

double platform_get_altitude()
{
    printf("Executing platform_get_altitude()\n");
    return 2.0;
}

double platform_get_gps_accuracy()
{
    printf("Executing platform_get_gps_accuracy()\n");
    return 1.0;
}
