#include "mode.h"
#include "schedule.h"
#include "simulation.h"

int main(int argc, char** argv) {
    Mode mode         = newMode(argv, argc);
    Schedule schedule = readSchedule(stdin);
    
    simulate(schedule, mode);
    freeSchedule(schedule);
    
    return EXIT_SUCCESS;
}
