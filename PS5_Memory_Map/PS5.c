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

int err_open(const char *pathname, int flags, mode_t mode){
    int fd;
    if (mode)
        fd = open(pathname,flags,mode);
    else
        fd = open(pathname,flags);
    if (fd == -1){
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

int err_read(int fd, void*buf, size_t count){
    int rd;
    if ((rd = read(fd,buf,count)) == -1){
        err_exit("could not read from file");
    }
    return rd;
}

void *err_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset){
    void *map;
    if ((map = mmap(addr, length, prot, flags, fd, offset)) == MAP_FAILED){
        err_exit("could not map file");
    }
    return map;
}

void err_fstat(int fd, struct stat *buf){
    if(fstat(fd, buf) == -1){
        err_exit("could not stat file");
    }
    return;
}

void err_lseek(int fd, off_t offset, int whence){
    if (lseek(fd,offset,whence) < 0)
        err_exit("could not lseek");
}

void problem_a(){

    char* buf = "test string";
    void* mapped;
    size_t length;
    struct stat stat_struct;

    fprintf(stderr,"Opening file for reading\n");
    int fd = err_open(testfile, O_RDONLY, 0);

    err_fstat(fd,&stat_struct);
    length = stat_struct.st_size;
    fprintf(stderr,"fstat reports the test file size to be %zd\n",length);

    mapped = err_mmap(NULL, length, PROT_READ, MAP_SHARED, fd, 0);
    fprintf(stderr,"about to write to mmap\n");
    sprintf(mapped,"generate signal");

    return;
}

void problem_b(int shared){
    void* mapped;
    int fd;
    size_t length;
    struct stat stat_struct;
    off_t offset = 0;
    int buffersize = 128;
    char* buf = malloc(buffersize*sizeof(char));

    char* writestring = "write to mapped";

    fprintf(stderr,"Opening file\n");
    fd = err_open(testfile, O_RDWR, 0644);

    err_fstat(fd,&stat_struct);
    length = stat_struct.st_size;
    fprintf(stderr,"fstat reports the test file size to be %zd bytes long\n",length);

    if (shared)
        mapped = err_mmap(NULL, length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, offset);
    else
        mapped = err_mmap(NULL, length, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, offset);

    fprintf(stderr,"writing to mapped: %s\n",writestring);
    sprintf(mapped,"%s",writestring);

    err_lseek(fd,offset,SEEK_SET);
    err_read(fd,buf,buffersize);

    fprintf(stderr,"read from mapped: %s\n", buf);

}

int main(int argc, char** argv){
    // generate test file
    // printf("generating test file\n");
    // err_open(testfile, O_CREAT|O_WRONLY|O_TRUNC);
    // problem_a();
    problem_b(1);



    return 0;
}