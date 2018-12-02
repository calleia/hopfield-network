#ifndef HOPFIELD_H_
#define HOPFIELD_H_

typedef struct Network {
	unsigned long width, height;
	float* weights;
} Network;

typedef struct Pattern {
	unsigned long width, height, size;
	char* data;
} Pattern;

Network* create_network(Pattern* pPattern);

Pattern* load_image(char* path);

Network* memorize_pattern(Pattern* pPattern);

void print_network(Network* pNetwork);

void print_pattern(Pattern* pPattern);

Pattern* retrieve_pattern(Pattern* pattern, Network* network);

void save_image(char* path, Pattern* pPattern);

#endif