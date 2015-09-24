/*
    ECE 460 Computer Operating Systems
    Problem Set 2
    Recursive Filesystem Exploration

    Author: Kelvin Lin
*/

#include <stdio.h>      // error reporting
#include <stdlib.h>     // malloc
#include <dirent.h>     // readdir opendir
#include <sys/types.h>  // stat
#include <sys/stat.h>
#include <unistd.h>     // getcwd
#include <errno.h>      // errno
#include <string.h>
#include <limits.h>     // realpath
#include <pwd.h>        // getpwnam
#include <grp.h>        // getgrgid    
#include <time.h>       // strftime
#include <fcntl.h>      // readlink
#include <regex.h>

int rootlen;
int vol;
char uname[256] = "";
int mtime = 0, curtime = 0;
int uid = -2;
char tmp[256];

void printStat(struct stat buf,char * cPath)
{
    if (uid >= -1)
    {
        if (uid == -1)
        {
            if (!strcmp(uname,""))
                return;
            if (strcmp(uname,getpwuid(buf.st_uid)->pw_name))
                return;
        }
        else if (uid != buf.st_uid)
            return;
    }
    if (!curtime)
    {
    	if (mtime > 0)
        {    
            if((curtime - buf.st_mtime) < mtime)
                return;
    	}
        else if ((curtime + mtime) > buf.st_mtime)
            return;
    }
    char c;
    printf("%04o/%lld ",buf.st_dev,
                        buf.st_ino);
    if      (S_ISLNK(buf.st_mode)) c = 'l';
    else if (S_ISDIR(buf.st_mode)) c = 'd';
    else if (S_ISBLK(buf.st_mode)) c = 'b';
    else if (S_ISCHR(buf.st_mode)) c = 'c';
    else if (S_ISFIFO(buf.st_mode)) c = 'p';
    else c = '-';
    printf( "%c",c );

    printf( (buf.st_mode & S_IRUSR) ? "r" : "-");
    printf( (buf.st_mode & S_IWUSR) ? "w" : "-");
    printf( (buf.st_mode & S_IXUSR) ? "x" : "-");
    printf( (buf.st_mode & S_IRGRP) ? "r" : "-");
    printf( (buf.st_mode & S_IWGRP) ? "w" : "-");
    printf( (buf.st_mode & S_IXGRP) ? "x" : "-");
    printf( (buf.st_mode & S_IROTH) ? "r" : "-");
    printf( (buf.st_mode & S_IWOTH) ? "w" : "-");
    printf( (buf.st_mode & S_IXOTH) ? "x" : "-");
    printf( " %d",buf.st_nlink );
    printf( " %s %s",getpwuid(buf.st_uid)->pw_name
                    ,getgrgid(buf.st_gid)->gr_name);

    if (S_ISBLK(buf.st_mode)||S_ISCHR(buf.st_mode))
        printf( " 0x%x",buf.st_dev );
    else printf( " %ld\t",buf.st_size );

    strftime(tmp, 18,"%y-%m-%d %I:%M",localtime(&buf.st_mtime));
    printf( " %s",tmp);

    printf(" %s",cPath+rootlen);

    if      (S_ISLNK(buf.st_mode))
    {
        readlink(cPath,tmp,256);
        printf(" -> %s",tmp);
    }
    printf("\n");
    return;
}

int lsDir(char *path)
{
    struct dirent *tdir;
    struct stat buf;
    DIR *tdirp, *odirp;
    char cPath[256];

    tdirp = opendir(path);
    if (tdirp == NULL)
    {
        perror("Cannot list NULL pointer\n");
        return -1;
    }     
    while( tdir = readdir(tdirp) )
    {
        if (!strcmp(tdir->d_name,".")||!strcmp(tdir->d_name,".."))
            continue;
        snprintf(cPath,256,"%s%s%s",path,"/",tdir->d_name);
        switch(tdir->d_type)
        {
            case DT_DIR:
                lstat(cPath,&buf);
                printStat(buf,cPath);
                if ((odirp = opendir(cPath)) == NULL)
                {
                    fprintf(stderr,"could not open directory %s: %s\n",cPath+rootlen,strerror(errno));
                    return -1;
                }
                if (buf.st_dev != vol)
                    printf("not in my house!\n");
                if ((lsDir(cPath)) == -1)
                {
                    fprintf(stderr,"could not open directory %s for listing: %s\n",tdir->d_name,strerror(errno));
                    return -1;
                }
                break;
            case DT_REG:
            case DT_BLK:
            case DT_CHR:
            default:
                if(lstat(cPath,&buf) < 0)
                {
                    fprintf(stderr,"could not stat file %s: %s",cPath+rootlen,strerror(errno));
                    return -1;
                }
                printStat(buf,cPath);
                break;
        }
    }
    return 0; // return with no errors
}

int getvol(char *path)
{
    struct stat buf;
    stat(strcat(path,"/."),&buf);
    vol = buf.st_dev;
    printf("volume: %d\n",vol);
    return 0;
}

int main(int argc, char **argv)
{
    char rootdir[256];
    regex_t regex;
    char opt;
    while((opt = getopt(argc, argv, "+u:m:")) != -1)
    {
        switch(opt)
        {
            case 'u':
                uid = -1;
                strcpy(tmp,optarg);
                if (regcomp(&regex,"^[[:digit:]]+$",0))
                    uid = atoi(tmp);
                else
                    strcpy(uname,tmp);
                break;
            case 'm':
                if ((mtime = atoi(optarg))==0)
                    curtime = 0;
                else curtime = (int)time(NULL);
                break;
            case '?':
                return -1;
            default:
                break;    
        }
    }
    if (argc <= optind)
    {
        strcpy(tmp,".");
    }
    else
    {
        strcpy(tmp,argv[optind]);
    }

    realpath(tmp,rootdir);
    rootlen = strlen(rootdir)+1;
    if (!getvol(rootdir))
        lsDir(rootdir);
    return 0;
}
