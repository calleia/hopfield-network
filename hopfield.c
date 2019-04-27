#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hopfield.h"
#include "pbm.h"
#include "rng.h"

Network* create_network(unsigned long size) {
	Network* pNetwork = (Network*) malloc(sizeof(Network));
	pNetwork->size = size;
	pNetwork->weights = (float*) calloc(size * size, sizeof(float));

	return pNetwork;
}

int get_index(int x, int y, Network* pNetwork) {
	return pNetwork->size * y + x;
}

Network* add_networks(Network* pFirstNetwork, Network* pSecondNetwork) {
	if (pFirstNetwork == NULL && pSecondNetwork != NULL) {
		return pSecondNetwork;
	} else if (pFirstNetwork != NULL && pSecondNetwork == NULL) {
		return pFirstNetwork;
	}
	
	Network* pNetwork = (Network*) malloc(sizeof(Network));
	pNetwork->size = pFirstNetwork->size;
	pNetwork->weights = (float*) calloc(pNetwork->size * pNetwork->size, sizeof(float));

	for (int j = 0; j < pNetwork->size; j++) {
		for (int i = 0; i < pNetwork->size; i++) {
			pNetwork->weights[get_index(i, j, pNetwork)] = pFirstNetwork->weights[get_index(i, j, pFirstNetwork)] + pSecondNetwork->weights[get_index(i, j, pSecondNetwork)];
		}
	}

	return pNetwork;
}

Pattern* create_pattern(unsigned long width, unsigned long height) {
	Pattern* pPattern = (Pattern*) malloc(sizeof(Pattern));
	pPattern->width = width;
	pPattern->height = height;
	pPattern->data = (char*) malloc(((width * height) + 1) * sizeof(char));

	// Make pPattern->data NULL terminated
	pPattern->data[width * height] = 0;

	return pPattern;
}

Pattern* copy_pattern(Pattern* pPattern) {
	Pattern* pCopiedPattern = create_pattern(pPattern->width, pPattern->height);
	strcpy(pCopiedPattern->data, pPattern->data);

	return pCopiedPattern;
}

Pattern* load_image(char* path) {
	FILE *pFile = fopen(path, "rb");
	PBMImage* pImage = loadPBM(pFile);
	fclose(pFile);

	Pattern* pPattern = create_pattern(pImage->width, pImage->height);
	unsigned long patternSize = pPattern->width * pPattern->height;

	for (int i = 0; i < patternSize; i++) {
		pPattern->data[i] = pImage->data[i] == '1' ? 1 : -1;
	}
	
	return pPattern;
}

Network* load_network(char* path) {
	Network* pNetwork = (Network*) malloc(sizeof(Network));

	FILE* pFile = fopen(path, "rb");

	char* buffer = (char*) malloc(sizeof(char));

	// Read Size
	char* size = NULL;
	size_t sizeLength = 0;

	do {
		fread(buffer, 1, 1, pFile);

		if (*buffer >= '0' && *buffer <= '9') {
			sizeLength++;
			size = (char*) realloc(size, sizeLength + 1);
			size[sizeLength - 1] = *buffer;
		}
	} while (*buffer != '\n');

	// Null terminating string
	size[sizeLength] = 0;

	pNetwork->size = strtoul(size, NULL, 10);

	// Read Data
	float* data = (float*) malloc(sizeof(float) * pNetwork->size * pNetwork->size);
	size_t dataSize = 0;

	char* weightString = NULL;
	size_t weightSize = 0;

	while (fread(buffer, 1, 1, pFile) == 1) {

		if ((*buffer >= '0' && *buffer <= '9') || *buffer == '.' || *buffer == '-') {
			weightSize++;
			weightString = (char*) realloc(weightString, weightSize + 1);
			weightString[weightSize - 1] = *buffer;
			weightString[weightSize] = 0;
		} else {
			if (weightString == NULL) {
				continue;
			}

			size_t index = dataSize;
			dataSize++;

			// Calculate weight indexes (row, column) based on it's position index
			// SOURCE: https://stackoverflow.com/questions/9674179/getting-the-row-and-column-of-a-triangular-matrix-given-the-index
			size_t row = floor(-0.5 + sqrt(0.25 + 2 * index));
			size_t triangularNumber = row * (row + 1) / 2;
			size_t column = index - triangularNumber;

			float weight = strtof(weightString, NULL);
			data[get_index(row, column, pNetwork)] = weight;
			data[get_index(column, row, pNetwork)] = weight;

			free(weightString);
			weightString = NULL;
			weightSize = 0;
		}
	}
	
	fclose(pFile);

	free(buffer);

	pNetwork->weights = data;

	return pNetwork;
}

Network* memorize_pattern(Pattern* pPattern) {
	Network* pNetwork = create_network(pPattern->width * pPattern->height);
	unsigned long patternSize = pPattern->width * pPattern->height;

	for (int i = 0; i < pNetwork->size; i++) {
		for (int j = 0; j < pNetwork->size; j++) {
			if (i != j) {
				pNetwork->weights[get_index(i, j, pNetwork)] = (pPattern->data[i] * pPattern->data[j]) / (float) patternSize;
			} else {
				pNetwork->weights[get_index(i, j, pNetwork)] = 0;
			}
		}
	}

	return pNetwork;
}

void print_network(Network* pNetwork) {
	for (int j = 0; j < pNetwork->size; j++) {
		for (int i = 0; i < pNetwork->size; i++) {
			fprintf(stdout, "%f\t", pNetwork->weights[get_index(i, j, pNetwork)] );
		}
		fprintf(stdout, "\n");
	}
}

void print_pattern(Pattern* pPattern) {
	unsigned long patternSize = pPattern->width * pPattern->height;

	for (int i = 0; i < patternSize; i++) {
		fprintf(stdout, "%d\t", pPattern->data[i]);
	}
}

void save_image(char* path, Pattern* pPattern) {
	FILE *pFile = fopen(path, "w");

	fprintf(pFile, "P1\n%lu %lu\n", pPattern->width, pPattern->height);

	for (int j = 0; j < pPattern->height; j++) {
		for (int i = 0; i < pPattern->width; i++) {
			fprintf(pFile, "%c ", pPattern->data[j * pPattern->height + i] == 1 ? '1' : '0');
		}

		fprintf(pFile, "\n");
	}

	fclose(pFile);
}

void save_network(char* path, Network* pNetwork) {
	FILE* pFile = fopen(path, "w");

	fprintf(pFile, "%lu\n", pNetwork->size);

	// Just save the lower triangular matrix since the weights are symmetrical between units
	for (int j = 0; j < pNetwork->size; j++) {
		for (int i = 0; i <= j; i++) {
			fprintf(pFile, "%.6f\t", pNetwork->weights[get_index(i, j, pNetwork)]);
		}

		fprintf(pFile, "\n");
	}

	fclose(pFile);
}

int sgn(float input) {
	return input < 0 ? -1 : 1;
}

Pattern* retrieve_pattern(Pattern* pPattern, Network* pNetwork) {
	Pattern* pLastPattern = NULL;

	do {
		pLastPattern = copy_pattern(pPattern);

		unsigned long* randomIndexes = get_random_sequence(pNetwork->size);

		for (int i = 0; i < pNetwork->size; i++) {
			float sum = 0;

			for (int j = 0; j < pNetwork->size; j++) {
				sum += pNetwork->weights[get_index(randomIndexes[i], randomIndexes[j], pNetwork)] * pPattern->data[randomIndexes[j]];
			}
			
			pPattern->data[randomIndexes[i]] = sgn(sum);
		}

	} while (compare_patterns(pPattern, pLastPattern) != 0);

	return pPattern;
}

int compare_patterns(Pattern* pFirstPattern, Pattern* pSecondPattern) {
	if (pFirstPattern->width != pSecondPattern->width)
		return 1;

	if (pFirstPattern->height != pSecondPattern->height)
		return 1;

	if (strcmp(pFirstPattern->data, pSecondPattern->data) != 0)
		return 1;

	return 0;
}