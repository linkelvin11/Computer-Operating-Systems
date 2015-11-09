#ifndef SEM_H_
#define SEM_H_

#define NUMPROC 64
int my_procnum;

struct sem {
	char lock;
    int count;
    int pstatus[NUMPROC];
    int pids[NUMPROC];
};


void sem_init(struct sem *s, int count);

int sem_try(struct sem *s);

void sem_wait(struct sem *s);

void sem_inc(struct sem *s);

#endif