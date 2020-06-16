
#include "ivec.h"
#include <string.h>
#include <stdlib.h> 
#include <stdio.h>
#include <assert.h>

  
ivec* 
make_ivec() 
{
	ivec* xs = malloc(sizeof(ivec));
	xs->cap = 4;
	xs->size = 0;
	xs->data = malloc(sizeof(char*) * xs->cap);
	return xs;
}

ivec* 
partition(ivec* xs, int start, int end)
{
	ivec* ys = make_ivec();
	int ii = start;
	while (ii <= end) {
		add_element(ys, xs->data[ii]);
		ii++;
	}
	return ys;
}

int 
length(ivec* xs) {
	return xs->size;
}


void 
add_element(ivec* xs, char* item) 
{
	if (xs->size >= xs->cap) {
		xs->cap = 2 * xs->cap;
		xs->data = realloc(xs->data, xs->cap * sizeof(char*));
	}

	xs->data[xs->size] = strdup(item);

	xs->size += 1;
}

void 
print_ivec(ivec* xs) 
{
	int ii = 0;
	while (ii < xs->size) {
		printf("%s\n", xs->data[ii]);
		ii++;
	}
/*
	printf("%c", '(');

	int ii = 0;
	while (ii < xs->size) {
		if (ii == xs->size - 1) {
			printf("\"%s\"", xs->data[ii]);
		}
		else {
			printf("\"%s\" ", xs->data[ii]);
		}
		ii++;
	}
	printf("%c", ')');*/
}


void
free_ivec(ivec* xs) 
{

	int ii = 0;
	while (ii < xs->size) {
		if (xs->data[ii] != 0) {
			free(xs->data[ii]);
		}
		ii++;
	}
	free((char *)xs->data);
	free(xs);
}

