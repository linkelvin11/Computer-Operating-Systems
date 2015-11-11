#include "constants.h"
#include "sem.h"
#include "fifo.h"

struct fifo *f;
int* child_rem;

void err_exit(char* buf){
	fprintf(stdout,"ERROR: %s: %s\n",buf,strerror(errno));
	exit(1);
}

int err_fork(){
	int frk;
	if ((frk = fork()) == -1)
		err_exit("couldn't fork");

}

void *err_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset){
    void *map;
    if ((map = mmap(addr, length, prot, flags, fd, offset)) == MAP_FAILED){
        err_exit("could not map file");
    }
    return map;
}

void kill_children(int parent_id){
	int i;
	for(i = 0; i < NUMPROC; i++) {
        if (i != parent_id && f->ac_sem.pids[i]) {
            kill(f->ac_sem.pids[i], SIGINT);
        }
	}
}

void parent_proc(){
	unsigned long lastread[NUMPROC];
	unsigned long rd;


	int index;
	for (index = 0; index < NUMPROC; index++)
		lastread[index] = 0;
	int i;
	for(i = 0; i < (NUMPROC-1) * (NUMLOOP); i++){
		rd = fifo_rd(f);
		index = (int)rd/1e6;
		if (lastread[index] > rd){
			kill_children(my_procnum);
			fprintf(stderr,"lastread[%d] = %lu, rd = %lu\n",index,lastread[index],rd);
			fprintf(stderr,"f->head = %d, f->tail = %d\n",f->head,f->tail);
			err_exit("fifo out of order. exiting...");
		}
		lastread[index] = rd;
	}
	fprintf(stderr,"done\n");
}

void child_proc(){
	unsigned long my_procid;
	my_procid = my_procnum * 1e6;
	int i;
	for (i = 0; i < NUMLOOP; i++){
		fifo_wr(f,my_procid + i);
	}
	child_rem[0]--;
	if (!child_rem[0]){
		kill(f->ac_sem.pids[0],SIGINT);
		fprintf(stderr,"the deed is done\n");
	}
}

int main(int argc, char** argv){
	unsigned long fifomax;
	unsigned long rd;
	my_procnum = 0;

	child_rem = err_mmap(0,sizeof (int),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
	*child_rem = NUMPROC-1;

	f = err_mmap(0,sizeof (struct fifo),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
	fifo_init(f);

	int i;
	for (i = 1; i < NUMPROC; i++){
		if (err_fork()) {
			
		}
		else {
			my_procnum = i;
			break;
		}
	}
	if (my_procnum)
		child_proc();
	else
		parent_proc();
}