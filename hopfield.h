#ifndef HOPFIELD_H_
#define HOPFIELD_H_

#include "hash.h"

#define MAX_NETWORK_UPDATES 100

typedef struct Network {
	unsigned long size;
	float* weights;
} Network;

typedef struct Pattern {
	unsigned long width, height;
	char* data;
} Pattern;

Network* add_networks(Network* pFirstNetwork, Network* pSecondNetwork);

Network* create_network(unsigned long size);

Pattern* create_pattern(unsigned long width, unsigned long height);

Pattern* load_image(char* path);

Network* load_network(char* path);

Network* memorize_pattern(Pattern* pPattern);

void print_network(Network* pNetwork);

void print_pattern(Pattern* pPattern);

Pattern* retrieve_pattern(Pattern* pPattern, Network* pNetwork);

void save_image(char* path, Pattern* pPattern);

void save_network(char* path, Network* pNetwork);

int compare_patterns(Pattern* pFirstPattern, Pattern* pSecondPattern);

#endif