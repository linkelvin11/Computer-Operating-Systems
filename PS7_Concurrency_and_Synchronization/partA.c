#include "constants.h"
#include "sem.h"


#define NUMPROC 64

struct sem *s;

int main(int argc, char** argv) {
	int procs[NUMPROC];

	int *mapped;
	s = mmap(0,(sizeof (struct sem)),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
	sem_init(s, 1);
	mapped = mmap(0,(sizeof (int)),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
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
		my_procnum = i;
	}

	int initial;
	int end;
	int j;
	for (j = 0; j < 1e6; j++){
		sem_wait(s);
		fprintf(stderr,"  locked for process %d\n",my_procnum);
		initial = mapped[0];
		end = ++mapped[0];
		if (end != initial+1){
			fprintf(stderr,"size mismatch! process %d started with %d and ended with %d\n",i,initial,end);
		    for(j = 0; j < NUMPROC; j++) {
		        if (j != my_procnum && s->pids[j]) {
		            kill(s->pids[j], SIGINT);
		        }
    		}
			exit (1);
		}
		else{
			fprintf(stderr,"process %d started with %d and ended with %d\n",my_procnum,initial,end);
		}
		fprintf(stderr,"unlocked for process %d\n",my_procnum);
		sem_inc(s);
	}
	return 0;
}