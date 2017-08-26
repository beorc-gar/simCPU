#ifndef __SCHEDULE_H_
#define __SCHEDULE_H_

#include "mode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define BUFF_LEN 1024

/*

Schedule schedule:
    
    int nprocesses
    int switchSame
    int switchNew
    Process* process:
        
        int processNum
        int nthreads
        Process* next
        Thread* thread:
            
            int threadNum
            int arriveTime
            int nbursts
            Thread* next
            Burst* burst:
                
                int burstNum
                int cpuTime
                int ioTime
                Burst* next
            
            Data data:
                
                int serviceTime
                int ioTime
                int turnAroundTime
                int finishTime

Queue queue:
    
    int size
    Event* event:
        
        int ofProcess
        int ready
        Thread* thread:
         
            int threadNum
            int arriveTime
            int nbursts
            Thread* next
            Burst* burst:
                
                int burstNum
                int cpuTime
                int ioTime
                Burst* next
            
            Data data:
                
                int serviceTime
                int ioTime
                int turnAroundTime
                int finishTime
        
        
*/

typedef struct Data {
    int serviceTime;
    int ioTime;
    int turnAroundTime;
    int finishTime;
} Data;

typedef struct Burst {
    int burstNum;
    int cpuTime;
    int ioTime;
    struct Burst* next;
} Burst;

typedef struct Thread {
    int threadNum;
    int arriveTime;
    int nbursts;
    Burst* burst;
    Data data;
    struct Thread* next;
} Thread;

typedef struct Process {
    int processNum;
    int nthreads;
    Thread* thread;
    struct Process* next;
} Process;

typedef struct Schedule {
    int nprocesses;
    int switchSame;
    int switchNew;
    Process* process;
} Schedule;

typedef struct Event {
    Thread* thread;
    int ofProcess;
    int ready;
} Event;

typedef struct Queue {
    int size;
    Event* event;
} Queue;

/* reads input from a stream to create a schedule
 * args: the stream to read from
 * returns: a schedule with specs from file */
Schedule readSchedule(FILE* fp);

/* frees all memory allocated for the schedule
 * args: the schedule to deallocate */
void freeSchedule(Schedule schedule);

#endif
