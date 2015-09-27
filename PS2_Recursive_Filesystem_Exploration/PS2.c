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
int xflag = 0,lflag = 0;
int loopCheck[2][1024];
char loopCheckPath[1024][256];

int isLoop(int dev, int ino, char *path)
{
    int i = 0;
    for (i = 0; i < 1024; i++)
    {
        if (ino == loopCheck[1][i] && dev == loopCheck[0][i])
            return i+1;
        if (loopCheck[0][i] == 0 && loopCheck[1][i] == 0)
        {
            loopCheck[0][i] = dev;
            loopCheck[1][i] = ino;
            snprintf(loopCheckPath[i],256,"%s",path);
            return 0;
        }
    }
    return 0;
}

int isLink(struct stat buf)
{
    return (S_ISLNK(buf.st_mode) ? 1:0);
}

void printStat(struct stat buf,char * cPath, int typeFlag)
{
    if (lflag && !S_ISLNK(buf.st_mode))
        return;
    if (uid >= -1)
    {
        if (uid == -1)
        {
            if (!strcmp(uname,""))
                return;
            if (getpwuid(buf.st_uid) != NULL) strcpy(tmp,getpwuid(buf.st_uid)->pw_name); else sprintf(tmp,"%d",buf.st_uid);
            if (strcmp(uname,tmp))
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
    if (typeFlag)
    {
        printf("%d",typeFlag);
    }
    else
    {
        if      (S_ISLNK(buf.st_mode)) c = 'l';
        else if (S_ISDIR(buf.st_mode)) c = 'd';
        else if (S_ISBLK(buf.st_mode)) c = 'b';
        else if (S_ISCHR(buf.st_mode)) c = 'c';
        else if (S_ISFIFO(buf.st_mode)) c = 'p';
        else c = '-';
        printf( "%c",c );
    }

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

    char uid_str[256];
    char gid_str[256];
    if (getpwuid(buf.st_uid) != NULL) strcpy(uid_str,getpwuid(buf.st_uid)->pw_name); else sprintf(uid_str,"%d",buf.st_uid);
    if (getgrgid(buf.st_gid) != NULL) strcpy(gid_str,getgrgid(buf.st_gid)->gr_name); else sprintf(gid_str,"%d",buf.st_gid);
    
    printf( " %s %s",uid_str
                    ,gid_str);

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
    int loopNum;
    int statres = 0;

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
        snprintf(cPath,256,"%s/%s",path,tdir->d_name);
        statres = lstat(cPath,&buf);
        switch(tdir->d_type)
        {
            case DT_DIR:
                
                if ((loopNum = isLoop(buf.st_dev,buf.st_ino,cPath+rootlen)))
                {
                    loopNum--;
                    printf("Not following looped directory at %s with dev=%04o and ino=%lld, also seen as %s/mnt4\n",
                        cPath+rootlen,buf.st_dev,buf.st_ino,loopCheckPath[loopNum]);
                    continue;
                }
                if (xflag && buf.st_dev != vol)
                {
                    printf("Found new device %04o for file %s, skipping file.\n",buf.st_dev,cPath+rootlen);
                }
                printStat(buf,cPath,0);
                if ((odirp = opendir(cPath)) == NULL)
                {
                    fprintf(stderr,"could not open directory %s: %s\n",cPath+rootlen,strerror(errno));
                    continue;
                }
                if (closedir(odirp) == -1)
                {
                    fprintf(stderr,"could not close directory %s: %s\n",cPath+rootlen,strerror(errno));
                    continue;
                }
                if ((lsDir(cPath)) == -1)
                {
                    return -1;
                }

                break;
            case DT_REG:
            case DT_BLK:
            case DT_CHR:
            default:
                if(statres < 0)
                {
                    fprintf(stderr,"could not stat file %s: %s\n",cPath+rootlen,strerror(errno));
                    continue;
                }
                printStat(buf,cPath,0);
                break;
        }
    }
    if (closedir(tdirp) == -1)
    {
        fprintf(stderr,"could not close directory %s: %s\n",cPath+rootlen,strerror(errno));
    }
    return 0; // return with no errors
}

int getvol(char *path)
{
    struct stat buf;
    strcpy(tmp,path);
    stat(strcat(tmp,"/."),&buf);
    vol = buf.st_dev;
    return 0;
}

int main(int argc, char **argv)
{
    char rootdir[256];
    regex_t regex;
    char opt;
    while((opt = getopt(argc, argv, "+u:m:xl")) != -1)
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
            case 'x':
                xflag = 1;
                break;
            case 'l':
                lflag = 1;
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
