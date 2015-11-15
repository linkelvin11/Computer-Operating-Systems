#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void empty_func(){
	return;
}

void err_gettime(clockid_t clk_id, struct timespec *tp){
	if (clock_gettime(clk_id, tp) != 0){
		perror("clock_gettime");
		exit(1);
	}
}

int main(int argc, char** argv){
	if (argc < 2){
		fprintf(stderr,"too few arguments\n");
		exit(1);
	}

	struct timespec tns, tne;

	

	switch(argv[1][0]){
		case '0':
			printf("case 0\n");
			int i;
			err_gettime(CLOCK_REALTIME, &tns);
			for (i = 0; i < 1000; i++){}
			err_gettime(CLOCK_REALTIME, &tne);
			printf("%lu\n",tne.tv_nsec-tns.tv_nsec);
			break;
		case '1':
			err_gettime(CLOCK_REALTIME, &tns);
			for (i = 0; i < 1000; i++){empty_func();}
			err_gettime(CLOCK_REALTIME, &tne);
			printf("%lu\n",tne.tv_nsec-tns.tv_nsec);
			break;
		case '2':
			err_gettime(CLOCK_REALTIME, &tns);
			for (i = 0; i < 1000; i++){getuid();}
			err_gettime(CLOCK_REALTIME, &tne);
			printf("%lu\n",tne.tv_nsec-tns.tv_nsec);
			break;
		default:
			break;
	}
	exit(0);
}