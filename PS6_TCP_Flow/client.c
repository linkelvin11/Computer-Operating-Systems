#include "constants.h"

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    portno = atoi(argv[2]);
    sockfd = err_socket(AF_INET, SOCK_STREAM, 0);
    server = err_gethostbyname(argv[1]);

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    err_connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr));

    while(1){
        printf("Please enter the message: ");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) 
             err_exit("error writing to socket");
        bzero(buffer,256);
        n = read(sockfd,buffer,255);
        if (n < 0) 
             err_exit("error reading from socket");
        printf("%s\n",buffer);
    }

    close(sockfd);
    return 0;
}