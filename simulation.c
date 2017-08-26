#include "simulation.h"

/* compare function for qsort. sort queue on arrival times */
static int compareEvent(const void* event1, const void* event2) {
    Event e1 = *(Event*)event1;
    Event e2 = *(Event*)event2;
   
    return (e1.ready > e2.ready) - (e1.ready < e2.ready);
}

/* create an initial queue from a fresh schedule */
static Queue newQueue(Schedule schedule) {
    Queue queue   = {.size = 0, .event = NULL};
    Process* proc = schedule.process;
    
    for(int i=0; i<schedule.nprocesses; i++) {
        Thread* thread = proc->thread;
        
        for(int j=0; j<proc->nthreads; j++) {
            queue.event = realloc(queue.event, (queue.size+1)*sizeof(Event));
            assert(queue.event);
            queue.event[queue.size].thread = thread;
            queue.event[queue.size].ofProcess = proc->processNum;
            queue.event[queue.size].ready = thread->arriveTime;
            queue.size++;
   
            thread = thread->next;
        }
        proc = proc->next;
    } 
    qsort(queue.event, queue.size, sizeof(Event), compareEvent);
    return queue;
}

/* Simulates a cpu thread scheduler for rr or fcfs.
 * args: the schedule to simulate, and the mode in which to simulate it */
void simulate(Schedule schedule, Mode mode) {
    Queue queue    = newQueue(schedule);
    Process* proc  = schedule.process;
    char* verbose  = malloc(1); verbose[0] = '\0';
    int idle       = 0;
    int cpuTime    = 0;
    int timeSlice  = 0;
    int turnAround = 0;
    int totalTime  = 0;
    int nthreads   = 0;
    
    if(mode.roundRobin) {
        printf("Round Robin Scheduling (quantum = %d time units)\n", mode.quantum);
    } else {
        printf("First Come First Serve Scheduling\n");
    }
        
    for(int i=0; i<queue.size; i++) {
        if(mode.verbose) {
            char buff[BUFF_LEN];
            int sysTime = timeSlice;
                
            if(timeSlice <= queue.event[i].ready) {
                sysTime = queue.event[i].ready;
            }
                
            sprintf(buff, "At time %d: Thread %d of Process %d moves from new to ready\n",
                    sysTime, queue.event[i].thread->threadNum, queue.event[i].ofProcess);
            verbose = realloc(verbose, strlen(verbose)+strlen(buff)+1);
            assert(verbose);
            strcat(verbose, buff);
        }
            
        if(i > 0) { // context switch
            if(queue.event[i].ofProcess == queue.event[i-1].ofProcess) {
                if(queue.event[i].thread->threadNum == queue.event[i-1].thread->threadNum) {
                    //no overhead
                } else {
                    timeSlice += schedule.switchSame;
                    idle      += schedule.switchSame;
                }
            } else {
                timeSlice += schedule.switchNew;
                idle      += schedule.switchNew;
            }
        }
        
        if(timeSlice <= queue.event[i].ready) { // nothing waiting for the cpu
            idle += queue.event[i].ready - timeSlice;
            timeSlice = queue.event[i].ready; // advance the timer
        }
            
        if(mode.verbose) {
            char buff[BUFF_LEN];
                
            sprintf(buff, "At time %d: Thread %d of Process %d moves from ready to running\n",
                    timeSlice, queue.event[i].thread->threadNum, queue.event[i].ofProcess);
            verbose = realloc(verbose, strlen(verbose)+strlen(buff)+1);
            assert(verbose);
            strcat(verbose, buff);
        }
        
        // run threads in queue's burst. then kick it out for io or quantum  
        if(mode.roundRobin) {
            if(queue.event[i].thread->burst->cpuTime > mode.quantum) {
                timeSlice += mode.quantum;
                cpuTime   += mode.quantum;
                queue.event[i].thread->burst->cpuTime   -= mode.quantum;
                queue.event[i].thread->data.serviceTime += mode.quantum;
            } else {
                timeSlice += queue.event[i].thread->burst->cpuTime;
                cpuTime   += queue.event[i].thread->burst->cpuTime;
                queue.event[i].thread->data.serviceTime += queue.event[i].thread->burst->cpuTime;
                queue.event[i].thread->data.ioTime      += queue.event[i].thread->burst->ioTime;
                queue.event[i].thread->burst->cpuTime    = 0;
            }
        } else {
            timeSlice += queue.event[i].thread->burst->cpuTime;
            cpuTime   += queue.event[i].thread->burst->cpuTime;
            queue.event[i].thread->data.ioTime += queue.event[i].thread->burst->ioTime;
            queue.event[i].thread->data.serviceTime += queue.event[i].thread->burst->cpuTime;
        }        
        queue.event[i].thread->data.turnAroundTime = timeSlice - queue.event[i].thread->arriveTime;
        queue.event[i].thread->data.finishTime     = timeSlice;
            
        if(mode.roundRobin && queue.event[i].thread->burst->cpuTime > 0) {
            queue.event = realloc(queue.event, (queue.size+1)*sizeof(Event));
            assert(queue.event);
            queue.event[queue.size] = queue.event[i];
            queue.event[queue.size].ready = timeSlice;
            queue.size++;
            qsort(queue.event, queue.size, sizeof(Event), compareEvent);
                
            if(mode.verbose) {
                char buff[BUFF_LEN];
                
                sprintf(buff, "At time %d: Thread %d of Process %d moves from running to blocked\n",
                        timeSlice, queue.event[i].thread->threadNum, queue.event[i].ofProcess);
                verbose = realloc(verbose, strlen(verbose)+strlen(buff)+1);
                assert(verbose);
                strcat(verbose, buff);
            }          
            continue;
        }
        
        // place unfinished threads back into the queue with their apropriate ready time
        if(queue.event[i].thread->burst->next) {
            Burst* temp = queue.event[i].thread->burst;
            
            if(mode.verbose) {
                char buff[BUFF_LEN];
                
                sprintf(buff, "At time %d: Thread %d of Process %d moves from running to blocked\n",
                        timeSlice, queue.event[i].thread->threadNum, queue.event[i].ofProcess);
                verbose = realloc(verbose, strlen(verbose)+strlen(buff)+1);
                assert(verbose);
                strcat(verbose, buff);
            } 
            queue.event = realloc(queue.event, (queue.size+1)*sizeof(Event));
            assert(queue.event);
            queue.event[queue.size] = queue.event[i];
            queue.event[queue.size].thread->burst = queue.event[queue.size].thread->burst->next;
            queue.event[queue.size].ready = timeSlice + queue.event[i].thread->burst->ioTime;
            queue.size++;
            qsort(queue.event, queue.size, sizeof(Event), compareEvent);
            free(temp);
        } else if(mode.verbose) {
            char buff[BUFF_LEN];
                
            sprintf(buff, "At time %d: Thread %d of Process %d moves from running to terminated\n",
                    timeSlice, queue.event[i].thread->threadNum, queue.event[i].ofProcess);
            verbose = realloc(verbose, strlen(verbose)+strlen(buff)+1);
            assert(verbose);
            strcat(verbose, buff);
        }
    }
    
    // simulation is over, begin calculating and displaying stats
    
    totalTime = timeSlice;
    printf("Total Time required is %d time units.\n", totalTime);
    
    for(int i=0; i<schedule.nprocesses; i++) {
        Thread* thread = proc->thread;
        
        for(int j=0; j<proc->nthreads; j++) {
            turnAround += thread->data.turnAroundTime;
            thread = thread->next;
            nthreads++;
        }
        proc = proc->next;
    }
    
    printf("Average Turnaround Time is %d time units.\n", turnAround/nthreads);
    printf("CPU Utilization is %.1f%%.\n", ((float)cpuTime/(float)totalTime)*100);
    
    if(mode.detailed) {
        proc = schedule.process;
        
        printf("\n");
        
        for(int i=0; i<schedule.nprocesses; i++) {
            Thread* thread = proc->thread;
            
            for(int j=0; j<proc->nthreads; j++) {
                printf("Thread %d of Process %d:\n", thread->threadNum, proc->processNum);
                printf("  arrival time: %d\n", thread->arriveTime);
                printf("  service time: %d units, I/O time: %d units, turnaround time: %d units\n"
                    , thread->data.serviceTime, thread->data.ioTime, thread->data.turnAroundTime);
                printf("  finish time: %d\n", thread->data.finishTime);
                thread = thread->next;
            }
            proc = proc->next;
        }
    }
    
    if(mode.verbose) {
        printf("\n%s", verbose);
    }
    
    free(verbose);
    free(queue.event);
}
