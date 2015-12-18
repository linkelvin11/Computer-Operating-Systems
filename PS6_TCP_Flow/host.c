#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <errno.h>

// we want the recieve buffer to be slightly smaller than the send buffer so the server can also prepend some data
#define RCVLEN 2000
#define SNDLEN 2048

void err_exit(char* buf){
    fprintf(stderr,"%s: %s\n",buf,strerror(errno));
    exit(-1);
}

int err_socket(int domain, int type, int protocol){
    int ret = socket(domain,type,protocol);
    if (ret < 0){
        err_exit("could not create socket");
    }
    return ret;
}

int err_read(int fd, void *buf, size_t count){
    int ret = read(fd,buf,count);
    if (ret < 0){
        err_exit("could not read");
    }
    return ret;
}

int err_write(int fd, const void *buf, size_t count){
    int ret = write(fd,buf,count);
    if (ret < 0){
        err_exit("could not write");
    }
    return ret;
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[RCVLEN];
    char sendbuff[SNDLEN];
    struct sockaddr_in serv_addr, cli_addr;
    int n,i,j,newtargetsockfd;
    int yes = 1;

    // file descriptor bookkeeping
    int fdmax;
    fd_set master;
    fd_set read_fds;
    int connections[32][2] = {0}; // length of a fd_set
    
    if (argc < 2) {
        fprintf(stderr,"error, no port provided\n");
        exit(1);
    }

    // set up listener socket
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
    listen(sockfd,32);
    fdmax = sockfd;
    connections[0][0] = sockfd;
    connections[0][1] = sockfd;
    FD_SET(sockfd, &master);

    fprintf(stderr,"starting server \n");
    int loop = 0;
    for (;;){

        read_fds = master;
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)
            err_exit("error on select");

        for(i = 0; i <= 32; i++){
            if (connections[i][0] && connections[i][0] < 32) {
                if (!FD_ISSET(connections[i][0],&read_fds))
                    continue;
                fprintf(stderr,"checking connection...\n");
                if (connections[i][0] == sockfd){ // handle new connections

                    fprintf(stderr,"found new connection\n");
                    clilen = sizeof(cli_addr);
                    newsockfd = accept(sockfd, 
                                (struct sockaddr *) &cli_addr, 
                                &clilen);
                    if (newsockfd < 0) 
                        err_exit("error on accept");

                    // add new fd to connections list
                    for (j = 0; j < 32; j++){
                        if (!connections[j][0]){
                            connections[j][0] = newsockfd;
                            connections[j][1] = newsockfd;
                            break;
                        }
                    }

                    FD_SET(newsockfd, &master);
                    if (newsockfd > fdmax)
                        fdmax = newsockfd;
                }
                else { // handle each client
                    fprintf(stderr,"handling client %d\n",connections[i][0]);
                    bzero(buffer,256);
                    err_read(connections[i][0],buffer,RCVLEN-1);

                    if (strcmp(buffer,"LIST\n") == 0){ // list all connected clients
                        sprintf(buffer,"you are client %d\n",connections[i][0]);
                        err_write(connections[i][0],buffer,strlen(buffer));
                        err_write(connections[i][0],"listing all connections...\n",27);
                        for (j = 0; j < 32; j++){
                            if (connections[j][0] && connections[j][0] != sockfd){
                                sprintf(buffer,"client found: %d\n",connections[j][0]);
                                err_write(connections[i][0],buffer, strlen(buffer));
                            }
                        }
                        n = err_write(connections[i][0],"done listing connections\n",25);
                    }
                    else if (sscanf(buffer,"CONNECT TO %d\n",&newtargetsockfd) > 0){ // set up forwarding
                        fprintf(stderr,"connecting client %d to client %d\n",connections[i][0],newtargetsockfd);

                        for(j = 0; j < 32; j++){
                            if (connections[j][0] == newtargetsockfd){
                                connections[i][1] = newtargetsockfd;
                                sprintf(buffer,"connected to client %d\n",newtargetsockfd);
                                err_write(connections[i][0],buffer,strlen(buffer));
                                j = 40;
                                break;
                            }
                        }
                        if (j != 40){
                            err_write(connections[i][0],"could not connect to client\n",28);
                        }
                    }
                    else{ // read from one client, and write to another (input/output clients could be the same)
                        printf("Here is the message: %s\n",buffer);
                        err_write(connections[i][0],"message recieved\n",17);

                        sprintf(sendbuff,"recieved message from client %d: %s\n",connections[i][0],buffer);
                        err_write(connections[i][1],sendbuff,strlen(sendbuff));
                    }
                }
            }
        }// loop through file descriptors
    }

    close(newsockfd);
    close(sockfd);
    return 0; 
}