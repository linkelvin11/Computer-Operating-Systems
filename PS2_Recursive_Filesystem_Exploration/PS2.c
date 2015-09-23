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

int rootlen;
char uname[256];
int mtime;

//int lsDir(DIR *dirp)
int lsDir(char *path)
{
	struct dirent *tdir = malloc(sizeof(struct dirent));
	struct stat buf;// = malloc(sizeof(struct stat));
	DIR *tdirp, *odirp;
	char cPath[256];

	tdirp = opendir(path);
	if (tdirp == NULL)
	{
		perror("Cannot list NULL pointer\n");
		//if(buf) free(buf);
		if(tdir) free(tdir);
		return -1;
	}
	int i;
	for(i = 0; i < 2; i++)
	{
		if ( (tdir = readdir(tdirp)) == NULL)
		{
			perror("Could not open file\n");
			//if(buf) free(buf);
			if(tdir) free(tdir);
			return -1;
		} 
	}
	while( (tdir = readdir(tdirp)) )
	{
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
					if(tdir) free(tdir);
					return -1;
				}
				if ((lsDir(cPath)) == -1)
				{
					fprintf(stderr,"could not open directory %s for listing: %s\n",tdir->d_name,strerror(errno));
					//if(buf) free(buf);
					if(tdir) free(tdir);
					return -1;
				}
				break;
			case DT_REG:
				// print stuff about current file
				//printf("%s\n",cPath+rootlen);
				printf("%s\n",cPath+rootlen);
				if(lstat(cPath,&buf) < 0)
				{
					//if(buf) free(buf);
					if(tdir) free(tdir);
					fprintf(stderr,"could not stat file %s: %s",cPath+rootlen,strerror(errno));
					return -1;
				}
				// printf("%04o %d %s %d %s %s",
				// 	buf.st_dev,
				// 	buf.st_ino,
				// 	buf.st_mode,
				// 	buf.st_nlink,
				// 	buf.st_uid,
				// 	buf.st_gid
				// 	);
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
	if(tdir) free(tdir);
	return 0;
}

int main(int argc, char **argv)
{
	char rootdir[256];
	char tmp[256];

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
	//printf("The root path is: \n%s\n",rootdir);
	lsDir(rootdir);
}
