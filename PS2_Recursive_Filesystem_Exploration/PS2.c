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

int lsDir(DIR *dirp)
{
	struct dirent *tdir = malloc(sizeof (struct dirent));
	int i;
	for(i = 0; i < 2; i++)
	{
		if ( (tdir = readdir(dirp)) == NULL)
		{
			perror("Could not open file\n");
			return -1;
		}
	}
	while( (tdir = readdir(dirp)) != NULL)
	{
		/*
		printf("the current d_type is: %c\n",tdir->d_type);
		printf("DT_DIR = %c \nDT_REG = %c \nDT_BLK = %c \nDT_CHR = %c \n",
				DT_DIR,DT_REG,DT_BLK,DT_CHR);
				*/
		switch(tdir->d_type)
		{
			case DT_DIR:
				// print stuff about current tdir
				printf("Now listing: %s\n",tdir->d_name);
				if (!opendir(tdir->d_name))
				{
					fprintf(stderr,"could not run opendir line 42. d_name = %s: %s\n",tdir->d_name,strerror(errno));
					if(tdir)
						free(tdir);
					return -1;
				}
				if ((lsDir(opendir(tdir->d_name))) == -1)
				{
					fprintf
						(stderr,"could not open directory %s for listing: %s\n",
							tdir->d_name,
								strerror(errno));
					if(tdir)
						free(tdir);
					return -1;
				}
				break;
			case DT_REG:
				// print stuff about current file
				printf("found file: %s\n",tdir->d_name);
				break;
			case DT_BLK:
				break;
			case DT_CHR:
				break;
			default:
				break;
		}
	}
	if(tdir)
		free(tdir);
	return 0;
}

int main(int argc, char **argv)
{
	DIR *tdir;
	if (argc == 1)
		tdir = opendir("."); // list current directory
	else if ((tdir = opendir(argv[1])) == NULL)
	{
		perror("could not open directory");
		return -1; // error report here
	}
	printf("running lsDir on %s\n",argv[1]);
	lsDir(tdir);
}
