
#include <iostream>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>

//#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>

void getData()
{
	int cur = 0;

	if ((cur = sched_getscheduler(getpid())) == -1)
		std::cout << "Error getting scheduler." << std::endl;
	else
	{
		std::cout << "Got scheduler: ";
		switch (cur)
		{
			case SCHED_OTHER: std::cout << "SCHED_OTHER"; break;
			case SCHED_BATCH: std::cout << "SCHED_BATCH"; break;
			//case SCHED_IDLE: std::cout << "SCHED_IDLE"; break;
			case SCHED_FIFO: std::cout << "SCHED_FIFO"; break;
			case SCHED_RR: std::cout << "SCHED_RR"; break;
			default: std::cout << "Unknown"; break;
		}
		std::cout << std::endl;
	}
	
	struct sched_param p;
	
	
	if (sched_getparam(getpid(), &p) != 0)
		std::cout << "Error getting process priority" << std::endl;
	else
		std::cout << "Got priority: " << p.sched_priority << std::endl;
}

int setMaxPriority(int type)
{
	struct sched_param p;
	int max = sched_get_priority_max(type);
	p.sched_priority = max;
	
	return sched_setscheduler(getpid(), type, &p);
}

int main (int argc, char ** argv)
{

	setMaxPriority(SCHED_FIFO);

	getData();
	

	return 0;
}
