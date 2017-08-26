#include "schedule.h"

static bool readBurst(FILE* fp, Burst* burst) {
    char buffer[BUFF_LEN];
    
    if(!burst) {
        return false;
    }
    
    if(fgets(buffer, BUFF_LEN-1, fp)) { buffer[strlen(buffer)-1] = '\0';
        char* token = strtok(buffer, " \t");
        int ntokens = 0;
        
        while(token) {
            if(isEmpty(token)) {
                token = strtok(NULL, " \t");
                continue;
            } else if(isInt(token)) {
                switch(ntokens) {
                    case 0:  burst->burstNum = atol(token); break;
                    case 1:  burst->cpuTime  = atol(token); break;
                    case 2:  burst->ioTime   = atol(token); break;
                    default: return false;
                }
            } else {
                return false;
            }
            ntokens++;
            token = strtok(NULL, " \t");
        }
        if(ntokens == 2) {
            burst->ioTime = 0;
        } else if(ntokens != 3) {
            return false;
        }
    } else {
        return false;
    }  
    
    return true;
}

static bool readThread(FILE* fp, Thread* thread) {
    char buffer[BUFF_LEN];
    Burst* brst = NULL;
    Burst* prev = NULL;
    
    if(!thread) {
        return false;
    }
    
    thread->burst = malloc(sizeof(Burst));
    assert(thread->burst);
    brst = thread->burst;
    
    if(fgets(buffer, BUFF_LEN-1, fp)) { buffer[strlen(buffer)-1] = '\0';
        char* token = strtok(buffer, " \t");
        int ntokens = 0;
        
        while(token) {
            if(isEmpty(token)) {
                token = strtok(NULL, " \t");
                continue;
            } else if(isInt(token)) {
                switch(ntokens) {
                    case 0:  thread->threadNum  = atol(token); break;
                    case 1:  thread->arriveTime = atol(token); break;
                    case 2:  thread->nbursts    = atol(token); break;
                    default: return false;
                }
            } else {
                return false;
            }
            ntokens++;
            token = strtok(NULL, " \t");
        }
        if(ntokens != 3) {
            return false;
        }
    } else {
        return false;
    }
    
    for(int i=0; i<thread->nbursts; i++) {
        if(readBurst(fp, brst)) {
            brst->next = malloc(sizeof(Burst));
            assert(brst->next);
            prev = brst;
            brst = brst->next;
        } else {
            return false;
        }
    }
    if(thread->nbursts == 0) {
        free(thread->burst);
        thread->burst = NULL;
    } else {
        free(brst);  
        prev->next = NULL; 
    }
    
    thread->data.serviceTime    = 0;
    thread->data.ioTime         = 0;
    thread->data.turnAroundTime = 0;
    thread->data.finishTime     = 0;
    
    return true;
}

static bool readProcess(FILE* fp, Process* process) {
    char buffer[BUFF_LEN];
    Thread* thrd = NULL;
    Thread* prev = NULL;
    
    if(!process) {
        return false;
    }
    
    process->thread = malloc(sizeof(Thread));
    assert(process->thread);
    thrd = process->thread;
    
    if(fgets(buffer, BUFF_LEN-1, fp)) { buffer[strlen(buffer)-1] = '\0';
        char* token = strtok(buffer, " \t");
        int ntokens = 0;
        
        while(token) {
            if(isEmpty(token)) {
                token = strtok(NULL, " \t");
                continue;
            } else if(isInt(token)) {
                switch(ntokens) {
                    case 0:  process->processNum = atol(token); break;
                    case 1:  process->nthreads   = atol(token); break;
                    default: return false;
                }
            } else {
                return false;
            }
            ntokens++;
            token = strtok(NULL, " \t");
        }
        if(ntokens != 2) {
            return false;
        }
    } else {
        return false;
    }
    
    for(int i=0; i<process->nthreads; i++) {
        if(readThread(fp, thrd)) {
            thrd->next = malloc(sizeof(Thread));
            assert(thrd->next);
            prev = thrd;
            thrd = thrd->next;
        } else {
            return false;
        }
    }
    if(process->nthreads == 0) {
        free(process->thread);
        process->thread = NULL;
    } else {
        free(thrd);
        prev->next = NULL;  
    }
    
    return true;
}

/* reads input from a stream to create a schedule
 * args: the stream to read from
 * returns: a schedule with specs from file */
Schedule readSchedule(FILE* fp) {
    Schedule schedule;
    char buffer[BUFF_LEN];
    Process* proc = NULL;
    Process* prev = NULL;
    
    schedule.nprocesses = 0;
    schedule.switchSame = 0;
    schedule.switchNew  = 0;
    schedule.process = NULL;

    if(fp == NULL) {
        fprintf(stderr, "Unable to read from file.\n");
        exit(EXIT_FAILURE);
    }
    
    schedule.process = malloc(sizeof(Process));
    assert(schedule.process);
    proc = schedule.process;
    
    while(fgets(buffer, BUFF_LEN-1, fp)) { buffer[strlen(buffer)-1] = '\0';
        char* token = strtok(buffer, " \t");
        int ntokens = 0;
        
        while(token) {
            if(isEmpty(token)) {
                token = strtok(NULL, " \t");
                continue;
            } else if(isInt(token)) {
                switch(ntokens) {
                    case 0: schedule.nprocesses = atol(token); break;
                    case 1: schedule.switchSame = atol(token); break;
                    case 2: schedule.switchNew  = atol(token); break;
                    default: 
                        fprintf(stderr, "Unexpected file contents.\n");
                        freeSchedule(schedule);
                        exit(EXIT_FAILURE);
                }
            } else {
                fprintf(stderr, "Unexpected file contents.\n");
                freeSchedule(schedule);
                exit(EXIT_FAILURE);
            }
            ntokens++;
            token = strtok(NULL, " \t");
        }
        
        if(ntokens != 3) {
            fprintf(stderr, "Unexpected file contents.\n");
            freeSchedule(schedule);
            exit(EXIT_FAILURE);
        }
        
        for(int i=0; i<schedule.nprocesses; i++) {
            if(readProcess(fp, proc)) {
                proc->next = malloc(sizeof(Process));
                assert(proc->next);
                prev = proc;
                proc = proc->next;
            } else {
                fprintf(stderr, "Unexpected file contents.\n");
                freeSchedule(schedule);
                exit(EXIT_FAILURE);
            }
        }
    }
    
    if(schedule.nprocesses == 0) {
        fprintf(stderr, "Unexpected file contents.\n");
        freeSchedule(schedule);
        exit(EXIT_FAILURE);
    } else {
        free(proc);   
        prev->next = NULL;
    }
    
    return schedule;
}

static void freeBurst(Burst* burst) {
    if(!burst) return;
    
    freeBurst(burst->next);
    free(burst);
}

static void freeThread(Thread* thread) {
    if(!thread) return;
    
    freeThread(thread->next);
    freeBurst(thread->burst);
    free(thread);
}

static void freeProcess(Process* process) {
    if(!process) return;
    
    freeProcess(process->next);
    freeThread(process->thread);
    free(process);
}


/* frees all memory allocated for the schedule
 * args: the schedule to deallocate */
void freeSchedule(Schedule schedule) {
    freeProcess(schedule.process);
}
