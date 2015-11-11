#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

#define NUMPROC 64
#define NUMLOOP 1e5
#define MYFIFO_BUFSIZ 4096