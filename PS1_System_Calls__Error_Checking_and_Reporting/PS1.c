#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char **argv)
{
	int outfile = 1; 		// default to stdout
	int infiles[100]; 		// default to stdin
	int buffersize = 262144;// default to 256k
	int maxflags = 5<argc?5:argc;

	// Check for flags
	int i = 1;
	int j = 1;
	for (; i < maxflags; i++) // only need to check up to first 4 arguments for flags
	{
		if (strcmp(argv[i],"-b") == 0) // check for buffer size flag
		{
			i++;
			if (buffersize == 262144) // check if buffersize has already been initialized
			{
				if (i < maxflags)
				{
					j = i + 1;
					buffersize = atoi(argv[i]);
				}
				else
				{
					printf("ERROR: Missing buffersize argument. \n");
					return -1;
				}
			}
			else
			{
				printf("ERROR: Attempted to reinitialize buffer; Buffer has already been initialized. \n");
				return -1; // placeholder -- report an error about buffer size already initialized
			}		
		}
		else if (strcmp(argv[i],"-o") == 0) // check for output file flag
		{
			i++;
			if (outfile == 1) // check if outfile has already been opened
			{
				if (i < maxflags)
				{
					j = i + 1;
					outfile = open(argv[i],O_WRONLY | O_CREAT);
				}
				else
				{
					printf("ERROR: Missing outfile argument. \n");
					return -1;
				}
			}
			else
			{
				printf("ERROR: Attempted to open more than one output file. \n");
				return -1;
			}
		}
	}
	printf("%d %d\n",i,j);

	int offset = j;
	// open input files

	for (; j < argc; j++)
	{
		if (strcmp(argv[j],"-") != 0)
			infiles[j-offset] = open(argv[j],O_RDONLY);
	}




	return 0; // no errors were encountered
}