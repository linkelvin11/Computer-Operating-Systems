#ifndef SCHED_H
#define SCHED_H

#include "savectx64.h"

#define SCHED_NPROC     511  // (maximum pid)-1
#define SCHED_READY     0
#define SCHED_RUNNING   1
#define SCHED_SLEEPING  2
#define SCHED_ZOMBIE    -1
#define STACK_SIZE      65536



struct sched_proc {
	/*
	use this for each simulated task
	includes task state, priority, accumulated, cpu time, stack address
	*/

	// process properties
    int nice;               // Nice Value range from -20 to 19 to match unix
    int priority;           // Static priority (int in [0, 39])
    int state;              // Current state (SCHED_READY, SCHED_RUNNING, SCHED_ZOMBIE)
    int code;               // Exit code
    int pid;                
    int ppid;               // parent pid
    void *stack;            // pointer to stack memory
    struct savectx ctx; // save context

    // switch variables
    int cpu_time;           // cpu_time since last switch
    int cpu_time_tot;       // total processing time (used to calculate priority)
    int cpu_time_alloc;     // Allocated process time before task switch
};

struct sched_waitq {
	/*
	event/wakeup queue
	*/
    struct sched_proc *procs[SCHED_NPROC];
    int n_procs;
};

void sched_init(void (*init_fn)());

int sched_fork();
void sched_exit(int code);
int sched_wait(int *exit_code);
void sched_nice(int niceval);
int sched_getpid();
int sched_getppid();
int sched_gettick();
void sched_ps();
void sched_switch();


#endif