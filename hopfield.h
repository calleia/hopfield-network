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

Network* create_network(Pattern* pPattern);

Network* memorize_pattern(Pattern* pPattern);

void print_network(Network* pNetwork);

void print_pattern(Pattern* pPattern);

Pattern* retrieve_pattern(Pattern* pattern, Network* network);

#endif