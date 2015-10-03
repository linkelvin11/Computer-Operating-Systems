/*
    ECE 460 Computer Operating Systems
    Problem Set 4
    Signals and Pipes

    Author: Kelvin Lin
*/

#include <stdio.h>      // error reporting
#include <stdlib.h>     // exit
#include <errno.h>      // errno
#include <unistd.h>     // pipe
#include <fcntl.h>

int main(int argc, char **argv)
{
    int pflag = 0;
    int gpid = 0;   // grep pid
    int mpid = 0;   // more pid
    char opt;
    int gfd[2];     // grep pipe
    int mfd[2];     // more pipe
    int more_read;
    while((opt = getopt(argc, argv, "+p:")) != -1)
    {
        switch(opt)
        {
            case 'p': // specify pattern
                pflag = 1;
                break;
            case '?':
                fprintf(stderr,"ERROR: The option -%c requires an argument\n",(char)optopt);
                return -1;
            default:
                break;
        }
    }

    pipe(gfd);
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
            close(gfd[1]);
            // dup redirect io
            dup(gfd[0]);
            dup2(mfd[1],1);
            // exec
            exit(0);
        }
        close(gfd[0]);
    }
    

    if ((mpid = fork()) == -1)
    {
        fprintf(stderr,"ERROR: Could not fork into more: %s\n",strerror(errno));
        exit(1);
    }
    if (!mpid) // exec more
    {
        if (!pflag)
        {
            close(gfd[1]);
            close(mfd[0]);
            close(mfd[1]);
            more_read = gfd[0];
        }
        else
        {
            close(mfd[1]);
            more_read = mfd[0];
        }
        // dup redirect io
        dup(more_read);
        // exec
        exit(0);
    }
    close(mfd[0]);

    // dup redirect io
    dup2(gfd[1],1);
    // exec cat
    
    return 0;
}
