#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include "sched.h"

void test_parent(){
	fprintf(stderr,"parent process start\n");
	int *child_code;
	sched_wait(child_code);
	fprintf(stderr,"child exited with code %d\n",*child_code);
	return;
}


void test_child(){
	fprintf(stderr,"child process start\n");
	sched_exit(1);
	return;
}

void test(){
	fprintf(stderr,"successfully sched_init the test function\n");

	switch (sched_fork()){
		case 0:
			test_child();
			break;
		case -1:
			err_exit("couldn't fork to child");
		default:
			test_parent();
			break;
	}

	exit(0);
}

int main(int argc, char **argv) {
    struct sigaction sa;
    sa.sa_flags=0;
    sigemptyset(&sa.sa_mask);
    //sa.sa_handler=abrt_handler;
    sigaction(SIGABRT,&sa,NULL);
    sched_init(test);
    fprintf(stdout, "Should not ever get here.\n");
    return 0;
}
