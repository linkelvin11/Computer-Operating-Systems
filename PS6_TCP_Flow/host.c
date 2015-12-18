#include <string.h>
#include "constants.h"

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n,i,j;
    int yes = 1;

    // file descriptor bookkeeping
    int fdmax;
    fd_set master;
    fd_set read_fds;
    int connections[32] = {0}; // length of a fd_set
    
    if (argc < 2) {
        fprintf(stderr,"error, no port provided\n");
        exit(1);
    }

    sockfd = err_socket(AF_INET, SOCK_STREAM, 0);

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
            sizeof(serv_addr)) < 0) 
            err_exit("error on binding");

    // listen only supports up to 5 backlog
    listen(sockfd,5);
    fdmax = sockfd;
    connections[0] = sockfd;
    FD_SET(sockfd, &master);

    fprintf(stderr,"starting server \n");
    int loop = 0;
    for (;;){

        read_fds = master;
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)
            err_exit("error on select");

        fprintf(stderr,"looking through fds\n");
        for(i = 0; i <= 32; i++){
            if (connections[i] && connections[i] < 32) {
                fprintf(stderr,"now on connection fd %d, listener is %d\n",connections[i],sockfd);
                if (!FD_ISSET(connections[i],&read_fds))
                    continue;
                fprintf(stderr,"checking connection...\n");
                if (connections[i] == sockfd){ // handle new connections
                    fprintf(stderr,"found new connection\n");
                    clilen = sizeof(cli_addr);
                    newsockfd = accept(sockfd, 
                                (struct sockaddr *) &cli_addr, 
                                &clilen);
                    if (newsockfd < 0) 
                        err_exit("error on accept");

                    // add new fd to connections list
                    for (j = 0; j < 32; j++){
                        if (!connections[j]){
                            connections[j] = newsockfd;
                            break;
                        }
                    }

                    FD_SET(newsockfd, &master);
                    if (newsockfd > fdmax)
                        fdmax = newsockfd;
                }
                else { // handle each client
                    fprintf(stderr,"handling client\n");
                    bzero(buffer,256);
                    n = err_read(connections[i],buffer,255);
                    if (strcmp(buffer,"LIST\n") == 0){
                        n = err_write(connections[i],"listing all connections...\n",27);
                        for (j = 0; j < 32; j++){
                            if (connections[j] && connections[j] != sockfd && j != i){
                                sprintf(buffer,"client found: %d\n",connections[j]);
                                n = err_write(connections[i],buffer, strlen(buffer));
                            }
                        }
                        n = err_write(connections[i],"done listing connections\n",25);
                    }
                    else{
                        printf("Here is the message: %s\n",buffer);
                        n = err_write(connections[i],"I got your message",18);
                    }
                }
            }
        }// loop through file descriptors
    }

    close(newsockfd);
    close(sockfd);
    return 0; 
}