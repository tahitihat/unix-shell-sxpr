// Most of file originally written by N Tuck (ch01 starter code)

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include "tokens.h"

static char temp_line[128];

char*
read_line(int fd)
{
	int ii;
	for (ii = 0; ii < 100; ++ii) {
		int rv = read(fd, temp_line + ii, 1);
		if (rv == 0) {
			return 0;
		}
		if (temp_line[ii] == '\n') {
			temp_line[ii] = 0;
			break;
		}
	}
	return temp_line;
}

int
main(int argc, char* argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: ./shell input.sexp\n");
		return 1;
	}

	int input = open(argv[1], O_RDONLY);
	if (input == -1) {
		perror("open failed");
		return 1;
	}

	char* line;
	int rv = 0;

	while ((line = read_line(input))) {
		ivec* toks = tokens(line);
		rv = execute(toks);
		free_ivec(toks);  
	}

	close(input);

	return rv;
}

