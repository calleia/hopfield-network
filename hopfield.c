#include <stdio.h>
#include <stdlib.h>
#include "hopfield.h"
#include "pbm.h"

Network* create_network(Pattern* pPattern) {
	Network* pNetwork = (Network*) malloc(sizeof(Network));
	pNetwork->width = pPattern->size;
	pNetwork->height = pPattern->size;
	pNetwork->weights = (float*) calloc(pPattern->size * pPattern->size, sizeof(float));

	return pNetwork;
}

int get_index(int x, int y, Network* pNetwork) {
	return pNetwork->width * y + x;
}

Pattern* load_image(char* path) {
	FILE *pFile = fopen(path, "rb");
	PBMImage* pImage = loadPBM(pFile);
	fclose(pFile);

	Pattern* pPattern = (Pattern*) malloc(sizeof(Pattern));
	pPattern->width = pImage->width;
	pPattern->height = pImage->height;
	pPattern->size = pImage->width * pImage->height;
	pPattern->data = (char*) malloc(pPattern->size * sizeof(char));

	for (int i = 0; i < pPattern->size; i++) {
		pPattern->data[i] = pImage->data[i] == '1' ? 1 : -1;
	}

	return pPattern;
}

Network* memorize_pattern(Pattern* pPattern) {
	Network* pNetwork = create_network(pPattern);

	for (int i = 0; i < pNetwork->width; i++) {
		for (int j = 0; j < pNetwork->height; j++) {
			if (i != j) {
				pNetwork->weights[get_index(i, j, pNetwork)] = (pPattern->data[i] * pPattern->data[j]) / (float) pPattern->size;
			} else {
				pNetwork->weights[get_index(i, j, pNetwork)] = 0;
			}
		}
	}

	return pNetwork;
}

void print_network(Network* pNetwork) {
	for (int j = 0; j < pNetwork->height; j++) {
		for (int i = 0; i < pNetwork->width; i++) {
			fprintf(stdout, "%f\t", pNetwork->weights[get_index(i, j, pNetwork)] );
		}
		fprintf(stdout, "\n");
	}
}

void print_pattern(Pattern* pPattern) {
	for (int i = 0; i < pPattern->size; i++) {
		fprintf(stdout, "%d\t", pPattern->data[i]);
	}
}

void save_image(char* path, Pattern* pPattern) {
	FILE *pFile = fopen(path, "w");

	fprintf(pFile, "P1\n%lu %lu\n", pPattern->width, pPattern->height);

	for (int i = 0; i < pPattern->width; i++) {
		for (int j = 0; j < pPattern->height; j++) {
			fprintf(pFile, "%c ", pPattern->data[i * pPattern->width + j] == 1 ? '1' : '0');
		}

		fprintf(pFile, "\n");
	}

	fclose(pFile);
}

int sgn(float input) {
	return input < 0 ? -1 : 1;
}

Pattern* retrieve_pattern(Pattern* pattern, Network* network) {

	Pattern* pMemorized_pattern = (Pattern*) malloc(sizeof(Pattern));
	pMemorized_pattern->width = pattern->width;
	pMemorized_pattern->height = pattern->height;
	pMemorized_pattern->size = pattern->size;
	pMemorized_pattern->data = (char*) malloc(pattern->size * sizeof(char));

	for (int i = 0; i < pattern->size; i++) {
		float sum = 0;

		for (int j = 0; j < pattern->size; ++j) {
			sum += network->weights[get_index(i, j, network)] * pattern->data[j];
		}

		pMemorized_pattern->data[i] = sgn(sum);
	}

	return pMemorized_pattern;
}