#ifndef HOPFIELD_H_
#define HOPFIELD_H_

typedef struct Network {
	unsigned long width, height;
	float* weights;
} Network;

typedef struct Pattern {
	int size;
	char* data;
} Pattern;

void print_pattern(Pattern* pPattern);

#endif