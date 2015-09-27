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

void eggzeck(char *command)
{
    int i;
    printf("closing files\n");
    fcloseall();
    exit(0);    // kill the child process
}

int main(int argc, char **argv)
{
    char command[256];
    int pid = 0;
    int stat_loc;
    struct rusage status;
    while(1)    // start shell
    {
        printf("$: ");
        scanf("%s",command);
        pid = fork();
        if (pid)
        {
            printf("i am a parent of %d. Waiting...\n",pid);
            waitpid(pid,&stat_loc,0);
            continue;
        }
        eggzeck(command);
    }

    return 0;
}
