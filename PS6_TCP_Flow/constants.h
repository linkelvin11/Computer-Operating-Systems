/*
	this file contains all error checking versions of commonly
	used functions that need to be shared between the host and
	client code
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <errno.h>

void err_exit(char* buf);
int err_socket(int domain, int type, int protocol);
struct hostent *err_gethostbyname(const char *name);
int err_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int err_read(int fd, void *buf, size_t count);