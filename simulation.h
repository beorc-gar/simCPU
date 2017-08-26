#ifndef _SIMULATION_H_
#define _SIMULATION_H_

#include "schedule.h"
#include "mode.h"

/* Simulates a cpu thread scheduler for rr or fcfs.
 * args: the schedule to simulate, and the mode in which to simulate it */
void simulate(Schedule schedule, Mode mode);

#endif
