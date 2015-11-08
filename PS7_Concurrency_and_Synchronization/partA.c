#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define NUMPROC 64



int main(int argc, char** argv) {
	int procs[NUMPROC];

	int *mapped;
	mapped = mmap(0,(10* sizeof (int)),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
	mapped[0] = 0;

	int curr_proc;
	int i;
	for (i = 1; i < NUMPROC; i++) {
		curr_proc = fork();
		procs[i] = curr_proc;
		if (curr_proc == 0)
			break;
		else if (curr_proc == -1){
			perror("couldn't fork\n");
			exit(1);
		}
	}

	int initial;
	int end;
	int j;
	for (j = 0; j < 1e6; j++){
		initial = mapped[0];
		end = ++mapped[0];
		if (end != initial+1){
			fprintf(stderr,"size mismatch! process %d started with %d and ended with %d\n",i,initial,end);
			exit (1);
		}

	}
	return 0;
}