#include <stdio.h>
#include <stdlib.h>

#include "sem.h"
#include "fifo.h"

void fifo_init (struct fifo *f){

	sem_init(&f->wr_sem,MYFIFO_BUFSIZ);
	sem_init(&f->rd_sem,1);
	sem_init(&f->ac_sem,1);

	f->head = 0;
	f->tail = 0;
	return;
}

void fifo_wr (struct fifo *f, unsigned long d){
	sem_wait(&f->wr_sem);
	sem_wait(&f->ac_sem);

	f->buf[f->head++] = d;
	f->head %= MYFIFO_BUFSIZ;

	sem_inc(&f->wr_sem);
	sem_inc(&f->ac_sem);
}

unsigned long fifo_rd (struct fifo *f){

	unsigned long ret;

	sem_wait(&f->rd_sem);
	sem_wait(&f->ac_sem);

	ret = f->buf[f->tail++];
	f->tail %= MYFIFO_BUFSIZ;

	sem_inc(&f->rd_sem);
	sem_inc(&f->ac_sem);

	return ret;
}
