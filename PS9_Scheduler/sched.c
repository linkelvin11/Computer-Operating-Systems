#include <sys/time.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>

#include "sched.h"
#include "adjstack.c"

#define STACK_SIZ 65536
#define SCHED_NPROC     511  // (maximum pid)-1
#define SCHED_READY     0
#define SCHED_RUNNING   1
#define SCHED_SLEEPING  2
#define SCHED_ZOMBIE    -1


struct sched_proc *curr_proc;
struct sched_waitq *procsq;
unsigned int num_ticks;
sigset_t full_sigset;

// exit with error message
void err_exit(char *msg){
    fprintf(stderr,"ERROR: %s: %s\n",msg,strerror(errno));
    exit(1);
}

// check running processes and return lowest available pid
int gen_pid() {
    int i;
    for (i = 0; i < SCHED_NPROC; i++){
        if (!procsq->procs[i])
            // need to add 1 since minimum pid is 1, but need to index from 0
            return i+1;
    }
    return -1;
}

void sched_init(void (*init_fn)()){
    /*
    initialize scheduling system
        set up periodic interval timer (setitimer)
        establish sched_tick as the signal handler for the timer
        create initial task with pid 1
        make pid 1 runnable and transfer execution to at location init_fn.
    */

    // set up periodic interval timer
    struct timeval interval;
    interval.tv_sec = 0;
    interval.tv_usec = 1e5;

    struct itimerval timer;
    timer.it_interval = interval;
    timer.it_value = interval;

    num_ticks = 0;
    setitimer(ITIMER_VIRTUAL, &timer, NULL);
    //signal(SIGVTALRM, sched_tick);

    // Set up signal masks needed
    sigfillset(&full_sigset);

    // malloc task queue
    if (!(procsq = (struct sched_waitq *)malloc(sizeof(struct sched_waitq))))
        err_exit("could not malloc process queue");

    // malloc initial proc
    struct sched_proc *init_proc;
    if (!(init_proc = (struct sched_proc *)malloc(sizeof(struct sched_proc))))
        err_exit("could not malloc initial process struct");

    // mmap a new stack
    void *init_stack = mmap(0, STACK_SIZ, PROT_READ | PROT_WRITE,MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if(init_stack == MAP_FAILED)
        err_exit("could not mmap stack");

    // initialize process properites
    init_proc->cpu_time_alloc = 10;
    init_proc->state = SCHED_RUNNING;
    init_proc->pid = gen_pid();
    init_proc->ppid = init_proc->pid;
    init_proc->stack = init_stack;
    init_proc->nice = 0;

    // add init_proc to process queue
    procsq->procs[0] = init_proc;
    procsq->n_procs = 1;
    curr_proc = init_proc;

    // run
    struct savectx curr_ctx;
    curr_ctx.regs[JB_BP] = init_stack + STACK_SIZ;
    curr_ctx.regs[JB_SP] = init_stack + STACK_SIZ;
    curr_ctx.regs[JB_PC] = init_fn;
    restorectx(&curr_ctx, 0);
}

int sched_fork(){

    sigprocmask(SIG_BLOCK, &full_sigset, NULL);

    // get new pid
    int child_pid;
    if ((child_pid = gen_pid()) < 0)
        err_exit("maximum number of processes reached");

    // malloc child proc
    struct sched_proc *child = (struct sched_proc *)malloc(sizeof (struct sched_proc));
    if (!child)
        err_exit("could not malloc child process");

    // mmap new stack space
    void* new_stack = mmap(0, STACK_SIZ, PROT_READ | PROT_WRITE,MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if (new_stack == MAP_FAILED){
        fprintf(stderr,"could not create stack for child process");
        free(child);
        return -1;
    }

    // copy old stack to new stack
    memcpy(new_stack,curr_proc->stack, STACK_SIZ);

    // initialize child process params
    child->state = SCHED_READY;
    child->pid = child_pid;
    child->ppid = curr_proc->pid;
    child->stack = new_stack;
    child->nice = 0;
    child->cpu_time = curr_proc->cpu_time;
    child->cpu_time_alloc = curr_proc->cpu_time_alloc;
    child->cpu_time_tot = 0;

    // adjstack and add child to procsq
    adjstack(new_stack, new_stack + STACK_SIZ, child->stack - curr_proc->stack);
    procsq->procs[child->pid -1] = child; // pid is 1 off from index (see gen_pid())
    procsq->n_procs++;

    if(!savectx(&child->ctx)){
        child->ctx.regs[JB_SP] += child->stack - curr_proc->stack;
        child->ctx.regs[JB_BP] += child->stack - curr_proc->stack;
        sigprocmask(SIG_UNBLOCK, &full_sigset, NULL);
        return child->pid;
    }
    else {

        sigprocmask(SIG_UNBLOCK, &full_sigset, NULL);
        return 0;
    }


}

void sched_exit(int code) {

    // store exit code and terminate process
    sigprocmask(SIG_BLOCK, &full_sigset, NULL);
    curr_proc->state = SCHED_ZOMBIE;
    curr_proc->code = code;
    procsq->n_procs--;
    procsq->procs[curr_proc->pid - 1] = curr_proc;

    // find and wake parent
    int i;
    int ppid = sched_getppid();
    for (i = 0; i < SCHED_NPROC; i++){
        if (procsq->procs[i] &&
            procsq->procs[i]->pid == curr_proc->ppid){
            if (procsq->procs[i]->state == SCHED_SLEEPING){
                procsq->procs[i]->state == SCHED_RUNNING;
                curr_proc->state == SCHED_ZOMBIE;
                // curr_proc = procsq->procs[i];
                // curr_proc->state = SCHED_RUNNING;
            }
            break;
        }
    }

    sigprocmask(SIG_UNBLOCK, &full_sigset, NULL);
    sched_switch();
}


int sched_wait(int *exit_code){

    sigprocmask(SIG_BLOCK, &full_sigset, NULL);

    // check if children exist
    int i;
    int children_found = 0;
    for (i = 0; i < SCHED_NPROC; i++){
        if (procsq->procs[i] &&
            procsq->procs[i]->ppid == curr_proc->pid){
            children_found = 1;
            break;
        }
    }
    if (!children_found)
        return -1;

    // sleep and switch
    curr_proc->state = SCHED_SLEEPING;
    sigprocmask(SIG_UNBLOCK, &full_sigset, NULL);
    sched_switch();

    // search for zombie child and get exit code
    sigprocmask(SIG_BLOCK, &full_sigset, NULL);
    children_found = 0;
    for (i = 0; i < SCHED_NPROC; i++){
        if (procsq->procs[i] &&
            procsq->procs[i]->ppid == curr_proc->pid &&
            procsq->procs[i]->state == SCHED_ZOMBIE){
            children_found = 1;
            break;
        }
    }
    sched_ps();
    if (!children_found)
        err_exit("no zombie children found after wakeup");

    *exit_code = procsq->procs[i]->code;
    free(procsq->procs[i]);
    procsq->procs[i] = NULL;
    sigprocmask(SIG_UNBLOCK, &full_sigset, NULL);
    return 0;

}
void sched_nice(int niceval){
    if (niceval > 19) niceval = 19;
    else if (niceval < -20) niceval = -20;
    curr_proc->nice = niceval;
    curr_proc->priority = 20 - niceval;
    return;
}
int sched_getpid(){
    return curr_proc->pid;
}
int sched_getppid(){
    return curr_proc->ppid;
}
int sched_gettick(){
    return num_ticks;
}

void sched_ps(){
    int i;
    int header = 1;
    for (i = 0; i < SCHED_NPROC; i++){
        if(procsq->procs[i]){
            if (header){
                fprintf(stderr,"listing all processes:\n");
                fprintf(stderr,"index\tpid\tppid\tstate\tnice\n");
                header = 0;
            }
            fprintf(stderr,"%d\t%d\t%d\t%d\t%d\n",
                i,
                procsq->procs[i]->pid,
                procsq->procs[i]->ppid,
                procsq->procs[i]->state,
                procsq->procs[i]->nice);
        }
    }
}

void sched_switch(){
    if (curr_proc->state == SCHED_RUNNING)
        curr_proc->state = SCHED_READY;

    // get process with lowest nice value
    // if equal nice values, pick if process is not the current process.
    int i;
    int best_idx = 0;
    int least_nice = 50; // greater than max nice
    for (i = 0; i < SCHED_NPROC; i++){
        if (procsq->procs[i] && 
            procsq->procs[i]->state != SCHED_ZOMBIE &&
            procsq->procs[i]->state != SCHED_SLEEPING)
            if (procsq->procs[i]->nice < least_nice){
                best_idx = i;
                least_nice = procsq->procs[i]->nice;
            }
            else if (procsq->procs[i]->nice == least_nice){
                if (procsq->procs[i] != curr_proc)
                    best_idx = i;
            }
    }

    if(!(savectx(&(curr_proc->ctx)))){
        curr_proc = procsq->procs[best_idx];
        curr_proc->cpu_time = 0;
        curr_proc->state = SCHED_RUNNING;
        restorectx(&(curr_proc->ctx),1);
    }
    return;
}

void sched_tick(){
    num_ticks++;
    curr_proc->cpu_time++;
    curr_proc->cpu_time_tot++;
    if (curr_proc->cpu_time > curr_proc->cpu_time_alloc)
        sched_switch();
}