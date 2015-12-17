#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include "sched.h"

#define NUM_CHILDREN 10
#define CHILD_LOOPS 5e6

int children = 0;

sigset_t wait_sigset;

void test_parent(){
    fprintf(stderr,"parent process start\n");
    int *child_code;
    fprintf(stderr,"waiting for child\n");
    sched_wait(child_code);
    fprintf(stderr,"child exited with code %d\n",*child_code);
    return;
}


void test_child(){
    fprintf(stderr,"child process start\n");
    int i;
    int sum = 0;

    for (i = 0; i < CHILD_LOOPS; i++){
        sigpending(&wait_sigset);
        if (sigismember(&wait_sigset,SIGVTALRM)){
        	fprintf(stderr, "caught a SIGVTALRM but didn't call the signal handler\n");
        }
    }

    fprintf(stderr,"child about to exit\n");
    sched_ps();

    sched_exit(rand()%10);
    return;
}

void test(){
    fprintf(stderr,"successfully sched_init the test function\n");

    int i;
    for (i = 0; i < NUM_CHILDREN; i++){
        children++;
        sched_nice(rand()%19);
        switch (sched_fork()){
            case 0:
                test_child();
                break;
            case -1:
                children--;
                break;
            default:
                break;
        }
    }
    fprintf(stderr,"loop done. %d children\n",children);
    for (i = 0; i < children; i++){
        test_parent();
    }

    exit(0);
}

int main(int argc, char **argv) {
    srand(time(NULL));
    sigemptyset(&wait_sigset);
    sigaddset(&wait_sigset,SIGVTALRM);
    sched_init(test);
    fprintf(stdout, "Should not ever get here.\n");
    return 0;
}
