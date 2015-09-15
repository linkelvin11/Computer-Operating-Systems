/*
	ECE 460 Computer Operating Systems
	Problem Set 1
	System Calls, Error Checking and Reporting

	Author: Kelvin Lin
*/


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BUFF_DEFAULT 1024	// default buffer size

int readwrite(int fin, int fout, void *buf, size_t buffersize)
{
	int rd = 0, wr = 0, wlen = 0;
	while((rd = read(fin,buf,buffersize)) != 0)
	{
		if (rd < 0)
		{
			perror("ERROR: Unable to read input");
			free(buf);
			return -1;
		}

		// Check for partial writes & write to file
		wlen = 0;
		do
		{
			wr = write(fout,buf+wlen,rd-wlen);
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
	return 0; // exit with no error
}


int main (int argc, char **argv)
{
	int outfile = 1; 			// default to stdout
	int infiles[100]; 			// default to stdin
	int buffersize = BUFF_DEFAULT;
	int bflag = 0, oflag = 0;	// default option flags to false
	int rflag = 0;

	// check for flags
	char opt;
	while((opt = getopt(argc, argv, "+b:o:")) != -1)
	{
		switch(opt)
		{
			case 'b': // buffersize specified
				if (bflag)
				{
					fprintf(stderr,"WARNING: Attempted to initialize two buffersizes\n");
				}
				if ((buffersize = atoi(optarg)) == 0)
				{
					fprintf(stderr,"ERROR: Invalid buffer size\n");
					return -1;
				}
				bflag = 1;
				break;
			case 'o': // output file specified
				if (oflag)
					fprintf(stderr,"WARNING: Attempted to open two output files\n");
				if((outfile = open(optarg,O_WRONLY | O_CREAT, 0666)) == -1)
				{
					perror("ERROR: Could not open output file");
					return -1;
				}
				oflag = 1;
				break;
			case '?':
				fprintf(stderr,"ERROR: The option -%c requires an argument\n",(char)optopt);
				return -1;
			default:
				break;
		}
	}

	// initialize buffer to size
	char* buf;
	if ((buf = malloc(buffersize)) == 0)
	{
		fprintf(stderr, 
				"ERROR: Could not allocate %d bytes of memory to buffer with malloc: %s\n", 
				buffersize, strerror(errno));
		return -1;
	}

	// initialize indexing and buffer variables
	int j = optind;
	int offset = j;

	// if there are no input args, read from stdin
	if ( j >= argc)
		if (readwrite(0,outfile,buf,buffersize) < 0)
			return -1;

	// iterate through non-flag inputs
	for (; j < argc; j++)
	{
		// open the file input from argv
		if (strcmp(argv[j],"-") != 0)
		{
			if ((infiles[j-offset] = open(argv[j],O_RDONLY,0666)) == -1)
			{
				fprintf(stderr,"ERROR: Unable to open input file %d: %s\n",infiles[j-offset],strerror(errno));
				return -1;
			}
		}
		else
			infiles[j-offset] = 0;

		// read from file input and write to ouput
		if (readwrite(infiles[j-offset],outfile,buf,buffersize) < 0)
			return -1;
		
		// close the file if its not stdin
		if ((infiles[j-offset] != 0) &&
			(close(infiles[j-offset]) < 0))
		{
			perror("ERROR: There was an error closing an input file");
			free(buf);
			return -1;
		}
	}

	// free buffer memeory
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