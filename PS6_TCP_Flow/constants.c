#include "constants.h"

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

struct hostent *err_gethostbyname(const char *name){
	struct hostent *ret = gethostbyname(name);
	if (ret == NULL){
		err_exit("could not get hostname");
	}
	return ret;
}

int err_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
	int ret = connect(sockfd,addr,addrlen);
	if (ret < 0){
		err_exit("could not connect");
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