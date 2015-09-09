#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main (int argc, char **argv)
{
	if (argc == 1) // if there are no arguments, exit.
		return 0;
	int buff_def = 1024	;	// default buffer size
	int outfile = 1; 		// default to stdout
	int infiles[100]; 		// default to stdin
	int buffersize = buff_def;
	int maxflags = 5<argc?5:argc;

	// Check for flags
	int i = 1;
	int j = 1;
	for (; i < maxflags; i++) // only need to check up to first 4 arguments for flags
	{
		if (strcmp(argv[i],"-b") == 0) // check for buffer size flag
		{
			i++;
			if (buffersize == buff_def) // check if buffersize has already been initialized
			{
				if (i < maxflags ) // check that there are enough input arguments left
				{
					j = i + 1;
					buffersize = atoi(argv[i]);
					if (buffersize == 0)
						perror("ERROR: Entered an invalid buffer size. \n");
				}
				else
				{
					perror("ERROR: Missing buffersize argument. \n");
					return -1;
				}
			}
			else
			{
				perror("ERROR: Attempted to reinitialize buffer; Buffer has already been initialized. \n");
				return -1; // placeholder -- report an error about buffer size already initialized
			}		
		}
		else if (strcmp(argv[i],"-o") == 0) // check for output file flag
		{
			i++;
			if (outfile == 1) // check if outfile has already been opened
			{
				if (i < maxflags) // check that there are enough input arguments left
				{
					j = i + 1;
					outfile = open(argv[i],O_WRONLY | O_CREAT, 0666);
				}
				else
				{
					perror("ERROR: Missing outfile argument. \n");
					return -1;
				}
			}
			else
			{
				perror("ERROR: Attempted to open more than one output file. \n");
				return -1;
			}
		}
	}
	printf("%d %d\n",i,j);

	// open input files
	int offset = j;
	char* buf;
	if ((buf = malloc(buffersize)) == 0)
	{
		fprintf(stderr, "ERROR: Could not allocate %d bytes of memory to buffer with malloc: %s\n", 
				buffersize, strerror(errno));
		return -1;
	}
	int rd = 0;
	int wr = 0;

	for (; j < argc; j++)
	{
		if (strcmp(argv[j],"-") != 0)
			infiles[j-offset] = open(argv[j],O_RDONLY,0666); // remember to check for errors here
		else
			infiles[j-offset] = 0;
		while((rd = read(infiles[j-offset],buf,buffersize)) != 0)
		{
			if (rd < 0)
			{
				perror("ERROR: Error while reading input");
				free(buf);
				return -1;
			}
			printf("rd = %d\n",rd);
			wr = write(outfile,buf,rd);
			
			if (wr < 0)
			{
				perror("ERROR: Unable to write to output");
				free(buf);
				return -1;
			}
		}
	}
	return 0; // no errors were encountered
}
