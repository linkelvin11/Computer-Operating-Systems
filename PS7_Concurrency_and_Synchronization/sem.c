#include "constants.h"
#include "sem.h"

int tas(volatile char *lock);

void sem_init(struct sem *s, int count) {
	s->count = count;
	s->lock = 0; // unlock

	int i; // zero pstatus and pids
	for (i = 0; i < NUMPROC; i++) {
		s->pstatus[i] = 0;
		s->pids[i] = 0;
	}
}

int sem_try(struct sem *s) {
	int ret;
	while (tas(&s->lock));

	if (s->count > 0){
		s->count--;
		ret = 1;
	}
	else
		ret = 0;
	s->lock = 0;
	return ret;

}

void sigusr1_handler(){
	return; // do nothing; wake from sleep
}

void sem_wait(struct sem *s) {

	s->pids[my_procnum] = getpid();

	while (1) {
		while (tas(&s->lock));

		// block all signals except sigint and sigusr1
		if (signal(SIGUSR1,sigusr1_handler) == SIG_ERR)
			perror("Failed to set signal handler");
		sigset_t mask;
		sigfillset(&mask);
		sigdelset(&mask, SIGUSR1);
		sigdelset(&mask, SIGINT);

		if (s->count > 0) {
			s->count--;
			s->pstatus[my_procnum] = 0;
			s->lock = 0;
			return;
		}
		else {
			s->lock = 0;
			s->pstatus[my_procnum] = 1;
			sigsuspend(&mask);
		}
	}
}

void sem_inc(struct sem *s) {
	while (tas(&s->lock));

	s->count++;

	int i;
	for (i = 0; i < NUMPROC; i++){
		if (s->pstatus[i]){
			kill(s->pids[i], SIGUSR1);
		}
	}
	s->lock = 0;
}