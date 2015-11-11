#include "constants.h"
#include "sem.h"

struct fifo {
	struct sem rd_sem; // read
	struct sem wr_sem; // write
	struct sem ac_sem; // access

	unsigned long buf[MYFIFO_BUFSIZ];
	int head;
	int tail;
};

void fifo_init (struct fifo *f);

void fifo_wr (struct fifo *f, unsigned long d);

unsigned long fifo_rd (struct fifo *f);