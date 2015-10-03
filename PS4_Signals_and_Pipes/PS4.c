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
    if (pipe(gfd) == -1)
    {
        fprintf(stderr,"ERROR: could not open pipe: %s\n",strerror(errno));
    }
    pipe(mfd);

    if (!pflag)
    {
        fprintf(stderr,"WARNING: pattern not specified. Piping cat directly to more.\n");
        // pipe cat straight to more.
    }
    else
    {   
        if ((gpid = fork()) == -1)
        {
            fprintf(stderr,"ERROR: Could not fork into grep: %s\n",strerror(errno));
            exit(1);
        }
        if (!gpid)  // exec grep
        {
            printf("execcing grep with pattern: /%s/\n",pattern);
            close(gfd[PIPE_WRITE]);
            close(mfd[PIPE_READ]);
            // dup redirect io
            dup2(gfd[PIPE_READ],0);
            dup2(mfd[PIPE_WRITE],1);
            // exec
            execlp("grep", "grep", "-e", pattern, NULL);
            exit(1);
        }
    }
    

    if ((mpid = fork()) == -1)
    {
        fprintf(stderr,"ERROR: Could not fork into more: %s\n",strerror(errno));
        exit(1);
        
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
        // dup redirect io
        dup2(more_read,0);
        // exec
        printf("execcing more\n");
        execlp("pg", "pg", NULL);
        exit(1);
    }
    close(gfd[PIPE_READ]);
    close(mfd[PIPE_READ]);

    // dup redirect io
    dup2(gfd[PIPE_WRITE],1);
    // exec cat

    argv[optind - 1] = "cat";
    execvp("cat", argv+optind-1);

    exit(0);
}
