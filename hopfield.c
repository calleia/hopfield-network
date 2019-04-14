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

Network* add_networks(Network* pFirstNetwork, Network* pSecondNetwork) {

	if (pFirstNetwork == NULL && pSecondNetwork != NULL) {
		return pSecondNetwork;
	} else if (pFirstNetwork != NULL && pSecondNetwork == NULL) {
		return pFirstNetwork;
	}
	
	Network* pNetwork = (Network*) malloc(sizeof(Network));
	pNetwork->width = pFirstNetwork->width;
	pNetwork->height = pFirstNetwork->height;
	pNetwork->weights = (float*) calloc(pNetwork->width * pNetwork->height, sizeof(float));

	for (int j = 0; j < pNetwork->height; j++) {
		for (int i = 0; i < pNetwork->width; i++) {
			pNetwork->weights[get_index(i, j, pNetwork)] = pFirstNetwork->weights[get_index(i, j, pFirstNetwork)] + pSecondNetwork->weights[get_index(i, j, pSecondNetwork)];
		}
	}

	return pNetwork;
}

Pattern* load_image(char* path) {
	FILE *pFile = fopen(path, "rb");
	PBMImage* pImage = loadPBM(pFile);
	fclose(pFile);

	Pattern* pPattern = (Pattern*) malloc(sizeof(Pattern));
	pPattern->width = pImage->width;
	pPattern->height = pImage->height;
	pPattern->size = pImage->width * pImage->height;
	pPattern->data = (char*) malloc((pPattern->size + 1) * sizeof(char));

	for (int i = 0; i < pPattern->size; i++) {
		pPattern->data[i] = pImage->data[i] == '1' ? 1 : -1;
	}

	// Make pPattern->data NULL terminated
	pPattern->data[pPattern->size] = 0;

	// Calculate hash
	pPattern->signature = get_hash(pPattern->data, FNV1_32A_INIT);

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

	pNetwork->width = strtoul(size, NULL, 10);
	pNetwork->height = pNetwork->width;

	// Read Data
	float* data = (float*) malloc(sizeof(float) * pNetwork->height * pNetwork->width);
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

void save_network(char* path, Network* pNetwork) {
	FILE* pFile = fopen(path, "w");

	fprintf(pFile, "%lu\n", pNetwork->width);

	// Just save the lower triangular matrix since the weights are symmetrical between units
	for (int j = 0; j < pNetwork->height; j++) {
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

Pattern* retrieve_pattern(Pattern* pattern, Network* network) {
	int network_update_count = 0;

	do {

		Hash last_signature = pattern->signature;

		for (int i = 0; i < pattern->size; i++) {
			float sum = 0;

			for (int j = 0; j < pattern->size; ++j) {
				sum += network->weights[get_index(i, j, network)] * pattern->data[j];
			}

			pattern->data[i] = sgn(sum);
		}

		// Make pPattern->data NULL terminated
		pattern->data[pattern->size] = 0;
		
		// Calculate hash
		pattern->signature = get_hash(pattern->data, FNV1_32A_INIT);

		if (pattern->signature != last_signature) {
			network_update_count = 0;
		} else {
			network_update_count++;
		}

	} while (network_update_count < MAX_NETWORK_UPDATES);

	return pattern;
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