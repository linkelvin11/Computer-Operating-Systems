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
#include <sys/wait.h>   // wait
#include <sys/types.h>
#include <signal.h>     // signal

#define PIPE_READ 0
#define PIPE_WRITE 1

int tbytes = 0;
int tfiles = 0;

int readwrite(int fin, int fout, void *buf, size_t buffersize)
{
    int rd = 0, wr = 0, wlen = 0;
    while((rd = read(fin,buf,buffersize)) != 0)
    {
        if (rd < 0)
        {
            perror("ERROR: Unable to read input");
            free(buf);
            return -1;
        }

        // Check for partial writes & write to file
        wr = write(fout,buf,rd);
        if (wr < 0)
        {
            perror("ERROR: Unable to write to output");
            free(buf);
            return -1;
        }
        tbytes = tbytes + wr;
    }
    return 0; // exit with no error
}

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
        perror("could not open pipe");
        err_exit();
    }
    return;
}

void err_close(int fd) // close with error reporting
{
    if (close(fd) == -1)
    {
        perror("could not close file");
        err_exit();
    }
}

void sigint_handler()
{
    fprintf(stderr,"SIGINT handler:\n");
    fprintf(stderr,"opened %d files\n", tfiles);
    fprintf(stderr,"wrote a total of %d bytes\n", tbytes);
    exit(1);
}

int main(int argc, char **argv)
{
    int pflag = 0;
    int mflag = 0;
    int gpid = 0;   // grep pid
    int mpid = 0;   // more pid
    char opt;

    int more_read;
    char pattern[256];

    int i;
    int fh;
    int buffersize = 1024;
    char* buf;

    signal(SIGINT,sigint_handler);
    signal(SIGPIPE,SIG_IGN);
    while((opt = getopt(argc, argv, "+mp:")) != -1)
    {
        switch(opt)
        {
            case 'p': // specify pattern
                pflag = 1;
                snprintf(pattern, 256,"%s",optarg);
                break;
            case 'm':
                mflag = 1;
                break;
            case '?':
                fprintf(stderr,"ERROR: The option -%c requires an argument\n",(char)optopt);
                return -1;
            default:
                break;
        }
    }

    for (i = optind; i < argc; i++)
    {
        int gfd[2];     // grep pipe
        int mfd[2];     // more pipe
        err_pipe(gfd);
        err_pipe(mfd);

        if (pflag)
        {  
            if ((gpid = fork()) == -1)
            {
                fprintf(stderr,"ERROR: Could not fork into grep: %s\n",strerror(errno));
                err_exit();
            }
            if (!gpid)  // exec grep
            {
                err_dup(gfd[PIPE_READ],0);
                if (mflag)
                    err_dup(mfd[PIPE_WRITE],1);

                err_close(gfd[PIPE_WRITE]);
                err_close(gfd[PIPE_READ]);
                err_close(mfd[PIPE_WRITE]);
                err_close(mfd[PIPE_READ]);

                execlp("grep", "grep", "-e", pattern, (char *) NULL);
                perror("ERROR: grep failed");
                err_exit();
            }
        }
        
        if (mflag)
        {
            if ((mpid = fork()) == -1)
            {
                fprintf(stderr,"ERROR: Could not fork into more: %s\n",strerror(errno));
                err_exit();
                
            }
            else if (!mpid) // exec more
            {
                if (!pflag)
                {
                    more_read = gfd[PIPE_READ];
                }
                else
                {
                    more_read = mfd[PIPE_READ];
                }

                err_dup(more_read,0);
                err_close(gfd[PIPE_WRITE]);
                err_close(gfd[PIPE_READ]);
                err_close(mfd[PIPE_WRITE]);
                err_close(mfd[PIPE_READ]);

                execlp("less", "less", (char *) NULL);
                perror("ERROR: more failed");
                err_exit();
            }
        }

        err_dup(gfd[PIPE_WRITE],1);
        err_close(gfd[PIPE_READ]);
        err_close(mfd[PIPE_READ]);
        err_close(mfd[PIPE_WRITE]);
        err_close(gfd[PIPE_WRITE]);
        // start writing files
        
        if ((buf = malloc(buffersize)) == 0)
        {
            fprintf(stderr, 
                    "ERROR: Could not allocate %d bytes of memory to buffer with malloc: %s\n", 
                    buffersize, strerror(errno));
            exit(1);
        }
    
        printf("reading %s\n",argv[i]);
        if((fh = open(argv[i],O_RDONLY)) == -1)
        {
            fprintf(stderr,"ERROR: could not open %s for reading: %s", argv[i], strerror(errno));
            exit(1);
        }
        tfiles++;
        readwrite(fh,1,buf,buffersize);
        err_close(fh);
        if (pflag) wait(0);
        if (mflag) wait(0);
    }
    exit(0);
}
