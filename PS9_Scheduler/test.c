#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include "sched.h"

#define NUM_CHILDREN 10
#define CHILD_LOOPS 1e9

int children = 0;

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
        sum += rand()%10;
    }
    fprintf(stdout,"%d\n",sum);
    sched_exit(rand()%10);
    return;
}

void test(){
    fprintf(stderr,"successfully sched_init the test function\n");

    int i;
    for (i = 0; i < NUM_CHILDREN; i++){
        children++;
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
    struct sigaction sa;
    sa.sa_flags=0;
    sigemptyset(&sa.sa_mask);
    //sa.sa_handler=abrt_handler;
    sigaction(SIGABRT,&sa,NULL);
    sched_init(test);
    fprintf(stdout, "Should not ever get here.\n");
    return 0;
}
