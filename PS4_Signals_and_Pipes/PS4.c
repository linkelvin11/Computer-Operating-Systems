/*
    ECE 460 Computer Operating Systems
    Problem Set 4
    Signals and Pipes

    Author: Kelvin Lin
*/

#include <stdio.h>      // error reporting
#include <stdlib.h>     // exit
#include <errno.h>      // errno
#include <unistd.h>     // pipe, exec
#include <fcntl.h>
#include <string.h>

#define PIPE_READ 0
#define PIPE_WRITE 1

void err_exit()
{
    fcloseall();
    exit(1);
}

void err_dup(int ofd, int nfd)  // dup with error reporting
{
    if (dup2(ofd,nfd) == -1)
    {
        fprintf(stderr,"ERROR: could not redirect fd %d to %d: %s\n",ofd,nfd,strerror(errno));
        err_exit();
    }
    return;
}

void err_pipe(int fds[2]) // pipe with error reporting
{
    if (pipe(fds) == -1)
    {
        perror("could not open pipe\n");
        err_exit();
    }
    return;
}

int main(int argc, char **argv)
{
    int pflag = 0;
    int gpid = 0;   // grep pid
    int mpid = 0;   // more pid
    char opt;
    int gfd[2];     // grep pipe
    int mfd[2];     // more pipe
    int more_read;
    char pattern[256];
    while((opt = getopt(argc, argv, "+p:")) != -1)
    {
        switch(opt)
        {
            case 'p': // specify pattern
                pflag = 1;
                snprintf(pattern, 256,"%s",optarg);
                break;
            case '?':
                fprintf(stderr,"ERROR: The option -%c requires an argument\n",(char)optopt);
                return -1;
            default:
                break;
        }
    }
    printf("optind = %d; argv[optind] = %s\n", optind, (argv+optind)[0]);
    err_pipe(gfd);
    err_pipe(mfd);

    if (!pflag)
    {
        fprintf(stderr,"WARNING: pattern not specified. Piping cat directly to more.\n");
        fprintf(stderr,"usage:\n\t./catgrepmore -p pattern infile1 [...infile2...]\n\n");
    }
    else
    {   
        if ((gpid = fork()) == -1)
        {
            fprintf(stderr,"ERROR: Could not fork into grep: %s\n",strerror(errno));
            err_exit();
        }
        if (!gpid)  // exec grep
        {
            close(gfd[PIPE_WRITE]);
            close(mfd[PIPE_READ]);

            err_dup(gfd[PIPE_READ],0);
            err_dup(mfd[PIPE_WRITE],1);

            execlp("grep", "grep", "-e", pattern, NULL);
            err_exit();
        }
    }
    

    if ((mpid = fork()) == -1)
    {
        fprintf(stderr,"ERROR: Could not fork into more: %s\n",strerror(errno));
        err_exit();
        
    }
    else if (!mpid) // exec more
    {
        close(gfd[PIPE_WRITE]);
        if (!pflag)
        {
            close(mfd[PIPE_READ]);
            close(mfd[PIPE_WRITE]);
            more_read = gfd[PIPE_READ];
        }
        else
        {
            close(gfd[PIPE_READ]);
            close(mfd[PIPE_WRITE]);
            more_read = mfd[PIPE_READ];
        }

        err_dup(more_read,0);

        execlp("pg", "pg", NULL);
        err_exit();
    }
    close(gfd[PIPE_READ]);
    close(mfd[PIPE_READ]);

    // dup redirect io
    err_dup(gfd[PIPE_WRITE],1);
    // exec cat

    argv[optind - 1] = "cat";
    execvp("cat", argv+optind-1);

    exit(0);
}
