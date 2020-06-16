#ifndef IVEC_H
#define IVEC_H

typedef struct ivec {
	char** data;
	int size;
	int cap;
} ivec;


ivec* make_ivec();
ivec* partition(ivec* xs, int start, int end);
int length(ivec* xs);
void print_ivec(ivec* xs);
void add_element(ivec* xs, char* item);
void free_ivec(ivec* xs);

#endif
