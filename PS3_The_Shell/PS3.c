/*
    ECE 460 Computer Operating Systems
    Problem Set 3
    Recursive Interactive Shell

    Author: Kelvin Lin
*/

#include <stdio.h>      // error reporting
#include <stdlib.h>     // exit
#include <string.h>     // strtok
#include <sys/resource.h> // getrusage()
#include <sys/wait.h>   // waitpid
#include <unistd.h>     // exec

void eggzeck(char *command)
{
    int i = 1;
    char** prog = malloc(256*sizeof(char));
    prog[255] = NULL;
    char delim[10] = " \t\n";
    fcloseall();

    for (prog[0] = strtok(command,delim); (prog[i] = strtok(NULL,delim)) != NULL && i < 255; i++)
        
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
