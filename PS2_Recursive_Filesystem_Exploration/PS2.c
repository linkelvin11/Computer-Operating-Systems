/*
	ECE 460 Computer Operating Systems
	Problem Set 2
	Recursive Filesystem Exploration

	Author: Kelvin Lin
*/

#include <stdio.h>		// error reporting
#include <stdlib.h>		// malloc
#include <dirent.h> 	// readdir opendir
#include <sys/types.h>	// stat
#include <sys/stat.h>
#include <unistd.h>		// getcwd
#include <errno.h>		// errno
#include <string.h>
#include <limits.h>
#include <pwd.h>		// getpwnam
#include <grp.h>		// getgrgid	
#include <time.h>		// strftime

int rootlen;
char uname[256];
int mtime;
char tmp[256];

int printStat(struct stat buf, int isLink, char * cPath)
{
	char c;
	printf("%04o/%d ",//%s %d %s %s",
		buf.st_dev,
		buf.st_ino);
	if (isLink) c = 'l';
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
						,getgrgid(buf.st_gid)->gr_name );
	printf( " %ld\t",buf.st_size );
	strftime(tmp, 18,"%y-%m-%d %I:%M",localtime(&buf.st_mtime));
	printf( " %s",tmp);
	printf(" %s\n",cPath+rootlen);
}

//int lsDir(DIR *dirp)
int lsDir(char *path)
{
	struct dirent *tdir;
	struct stat buf;// = malloc(sizeof(struct stat));
	DIR *tdirp, *odirp;
	char cPath[256];

	tdirp = opendir(path);
	if (tdirp == NULL)
	{
		perror("Cannot list NULL pointer\n");
		//if(buf) free(buf);
		//if(tdir) free(tdir);
		return -1;
	}	 
	while( tdir = readdir(tdirp) )
	{
		if (tdir == NULL)
		{
			perror("Could not open file\n");
			//if(buf) free(buf);
			//if(tdir) free(tdir);
			return -1;
		}
		if (!strcmp(tdir->d_name,".")||!strcmp(tdir->d_name,".."))
		{
			//printf("skipping %s\n",tdir->d_name);
			continue;
		}
		/*
		printf("the current d_type is: %c\n",tdir->d_type);
		printf("DT_DIR = %c \nDT_REG = %c \nDT_BLK = %c \nDT_CHR = %c \n",
				DT_DIR,DT_REG,DT_BLK,DT_CHR);
				*/
		snprintf(cPath,256,"%s%s%s",path,"/",tdir->d_name);
		switch(tdir->d_type)
		{
			case DT_DIR:
				// print stuff about current tdir
				printf("%s\n",cPath+rootlen);
				lstat(cPath,&buf);
				odirp = opendir(cPath);
				if (odirp == NULL)
				{
					fprintf(stderr,"could not open directory %s: %s\n",cPath+rootlen,strerror(errno));
					//if(buf) free(buf);
					//if(tdir) free(tdir);
					return -1;
				}
				if ((lsDir(cPath)) == -1)
				{
					fprintf(stderr,"could not open directory %s for listing: %s\n",tdir->d_name,strerror(errno));
					//if(buf) free(buf);
					//if(tdir) free(tdir);
					return -1;
				}
				break;
			case DT_REG:
				// print stuff about current file
				//printf("%s\n",cPath+rootlen);
				if(lstat(cPath,&buf) < 0)
				{
					//if(buf) free(buf);
					if(tdir) free(tdir);
					fprintf(stderr,"could not stat file %s: %s",cPath+rootlen,strerror(errno));
					return -1;
				}
				printStat(buf,cPath,0);
				break;
			case DT_BLK:
				break;
			case DT_CHR:
				break;
			default:
				break;
		}
	}
	//if(buf) free(buf);
	//if(tdir) free(tdir);
	return 0;
}

int main(int argc, char **argv)
{
	char rootdir[256];

	char opt;
	while((opt = getopt(argc, argv, "+u:m:")) != -1)
	{
		switch(opt)
		{
			case 'u':
				strcpy(uname,optarg);
				printf("%s\n",uname);
				break;
			case 'm':
				mtime = atoi(optarg);
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


	//printf("running lsDir on %s\n",tdir->d_name);
	realpath(tmp,rootdir);
	rootlen = strlen(rootdir)+1;
	printf("The root path is: \n%s\n",rootdir);
	lsDir(rootdir);
}
