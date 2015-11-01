/*
    Kelvin Lin
    ECE 460 Computer Operating Systems
    Problem Set 5 
*/

#include <stdio.h>
#include <stdlib.h>

// error reporting
#include <errno.h>
#include <string.h>

// file IO
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// mmap
#include <sys/mman.h>

char *testfile = "testfile.txt";

void err_exit(char *error){
    fprintf(stderr,"ERROR: %s: %s\n",error,strerror(errno));
    exit(1);
}

int err_open(const char *pathname, int flags){
    int fd;
    if ((fd = open(pathname,flags)) == -1){
        err_exit("could not open file");
    }
    return fd;
}

int err_write(int fd, const void *buf, size_t count){
    int wr;
    if ((wr = write(fd,buf,count)) == -1){
        err_exit("could not write to file");
    }
    return wr;
}

void *err_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset){
    void *map;
    if ((map = mmap(addr, length, prot, flags, fd, offset)) == -1){
        err_exit("could not map file");
    }
    return map;
}

void err_stat(const char *pathname, struct stat *buf){
    if(stat(pathname, buf) == -1){
        err_exit("could not stat file");
    }
    return;
}

void problem_a(){

    char* buf = "test string";
    void* mapped;
    size_t length;
    struct stat stat_struct;

    err_stat(testfile,&stat_struct);
    length = stat_struct.st_size;

    fprintf(stderr,"Opening file for reading\n");
    int fd = err_open(testfile, O_RDONLY);

    mapped = err_mmap(NULL, length, PROT_READ, MAP_SHARED, fd, 0);
    fprintf(stderr,"about to write to mmap\n");
    sprintf(mapped,"generate signal");

    return;
}

int main(int argc, char** argv){
    // generate test file
    // printf("generating test file\n");
    // err_open(testfile, O_CREAT|O_WRONLY|O_TRUNC);
    problem_a();



    return 0;
}