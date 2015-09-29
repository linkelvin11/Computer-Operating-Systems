/*
    ECE 460 Computer Operating Systems
    Problem Set 3
    Recursive Interactive Shell

    Author: Kelvin Lin
*/

#include <stdio.h>      // error reporting
#include <errno.h>      // errno
#include <stdlib.h>     // exit
#include <string.h>     // strtok
#include <sys/resource.h> // getrusage()
#include <sys/wait.h>   // waitpid
#include <unistd.h>     // exec
#include <sys/types.h>  // open
#include <sys/stat.h>
#include <fcntl.h>\

void open_redirect(char *fstream, int io, int flag)
{
    // io = io file descriptor
    // flag = 1 append
    // flag = 0 truncate
    int fd;
    if (!io)
        fd = open(fstream, O_RDONLY);
    else if (flag)
        fd = open(fstream, O_WRONLY|O_CREAT|O_APPEND,0666);
    else
        fd = open(fstream, O_WRONLY|O_CREAT|O_TRUNC,0666);
    if (fd < 0)
    {
        fprintf(stderr,"could not open file %s: %s\n", fstream, strerror(errno));
        exit(0);
    }
    else if (dup2(fd,io) < 0)
    {
        fprintf(stderr,"could not redirect fd %d to fd %d: %s\n",fd, io,strerror(errno));
        exit(0);
    }
    return;
}

int proc_redirect(char *rDirFd)
{
    printf("switching %c\n",rDirFd[0]);
    switch(rDirFd[0])
    {
        case '<':
            printf("lel");
            open_redirect(&rDirFd[1],0,0);
            return 1;
            break;
        case '>':
            if (rDirFd[1] == '>')
                open_redirect(&rDirFd[2],1,1);
            else
                open_redirect(&rDirFd[1],1,0);
            return 1;
            break;
        case '2':
            if (rDirFd[1] == '>')
            {
                if (rDirFd[1] == '>')
                    open_redirect(&rDirFd[2],2,1);
                else
                    open_redirect(&rDirFd[1],2,0);
            }
            return 1;
            break;
        default:
            break;
    }
    return 0;
}

void eggzeck(char *command)
{
    int i = 1;
    char** prog = malloc(256*sizeof(char));
    prog[255] = NULL;
    char delim[10] = " \t\n";
    fcloseall();

    for (prog[0] = strtok(command,delim); (prog[i] = strtok(NULL,delim)) != NULL && i < 255; i++);
    for (i = 1; prog[i] != NULL; i++)
    {
        if (proc_redirect(prog[i]))
            prog[i] = NULL;
    }
        
    execvp(prog[0],prog);
    free(prog);
    exit(0);    // kill the child process
}

int main(int argc, char **argv)
{
    size_t bsize = 256;
    char command[bsize];
    char *cmd = command;
    int pid = 0;
    int stat_loc;
    int i = 0;
    struct rusage status;
    while(1)    // start shell
    {
        printf("$: ");
        getline(&cmd,&bsize,stdin);
        pid = fork();
        if (pid)
        {
            printf("i am a parent of %d. Waiting...\n",pid);
            waitpid(pid,&stat_loc,0);
            printf("i am done waiting. Ready for next input\n");
            continue;
        }
        eggzeck(command);
    }
    return 0;
}
