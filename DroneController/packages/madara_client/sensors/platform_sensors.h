/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#ifndef _PLATFORM_SENSORS_H
#define _PLATFORM_SENSORS_H

struct madara_gps
{
	double latitude;
	double longitude;
	double altitude;
	double UTC;
	int quality;
	int num_sats;
	unsigned int index;
};

bool platform_init_sensor_functions();

void platform_read_thermal(double[8][8]);
void platform_read_gps(struct madara_gps *);
double platform_read_ultrasound();

double platform_get_gps_accuracy();
double platform_get_landed_height_accuracy();
double platform_get_battery_remaining();

#endif
