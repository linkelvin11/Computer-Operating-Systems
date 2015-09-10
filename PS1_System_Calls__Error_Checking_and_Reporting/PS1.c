#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BUFF_DEFAULT 1024	// default buffer size

int main (int argc, char **argv)
{
	if (argc == 1) 			// if there are no arguments, exit.
		return 0;	
	int outfile = 1; 		// default to stdout
	int infiles[100]; 		// default to stdin
	int buffersize = BUFF_DEFAULT;
	int maxflags = 5<argc?5:argc;

	// Check for flags
	// consider using getopt (man 3 getopt)
	int i = 1;
	int j = 1;
	for (; i < maxflags; i++) // only need to check up to first 4 arguments for flags
	{
		if (strcmp(argv[i],"-b") == 0) // check for buffer size flag
		{
			i++;
			if (buffersize == BUFF_DEFAULT) // check if buffersize has already been initialized
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
		fprintf(stderr, 
				"ERROR: Could not allocate %d bytes of memory to buffer with malloc: %s\n", 
				buffersize, strerror(errno));
		return -1;
	}
	int rd = 0;		// bytes read
	int wr = 0;		// bytes written
	int wlen = 0;	// progress in buffer

	for (; j < argc; j++)
	{
		// open the file input from argv
		if (strcmp(argv[j],"-") != 0)
			infiles[j-offset] = open(argv[j],O_RDONLY,0666); // remember to check for errors here
		else
			infiles[j-offset] = 0;

		// read from file input and write to ouput
		while((rd = read(infiles[j-offset],buf,buffersize)) != 0)
		{
			if (rd < 0)
			{
				perror("ERROR: Error while reading input");
				free(buf);
				return -1;
			}
			//printf("rd = %d\n",rd);

			// Check for partial writes
			wlen = 0;
			do
			{
				wr = write(outfile,buf+wlen,rd-wlen);
				wlen = wlen + wr;
				if (wr < 0)
				{
					perror("ERROR: Unable to write to output");
					free(buf);
					return -1;
				}	
			}
			while(wlen < rd);
			
		}

		// close the file if its not stdin
		if (infiles[j-offset] != 0 &&
			close(infiles[j-offset]) < 0)
		{
			perror("ERROR: There was an error closing an input file");
			free(buf);
			return -1;
		}
	}

	free(buf);

	// close output file if its not stdout
	if (outfile != 1 &&
		close(outfile) < 0)
	{
		perror("ERROR: There was an error closing the output file");
		return -1;
	}

	// no errors were encountered
	// exit program
	return 0; 
}
