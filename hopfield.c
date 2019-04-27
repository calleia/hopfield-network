#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hopfield.h"
#include "pbm.h"
#include "rng.h"

Model* create_model(unsigned long size) {
	Model* pModel = (Model*) malloc(sizeof(Model));
	pModel->size = size;
	pModel->weights = (float*) calloc(size * size, sizeof(float));

	return pModel;
}

int get_index(int x, int y, Model* pModel) {
	return pModel->size * y + x;
}

Model* add_models(Model* pFirstModel, Model* pSecondModel) {
	if (pFirstModel == NULL && pSecondModel != NULL) {
		return pSecondModel;
	} else if (pFirstModel != NULL && pSecondModel == NULL) {
		return pFirstModel;
	}
	
	Model* pModel = (Model*) malloc(sizeof(Model));
	pModel->size = pFirstModel->size;
	pModel->weights = (float*) calloc(pModel->size * pModel->size, sizeof(float));

	for (int j = 0; j < pModel->size; j++) {
		for (int i = 0; i < pModel->size; i++) {
			pModel->weights[get_index(i, j, pModel)] = pFirstModel->weights[get_index(i, j, pFirstModel)] + pSecondModel->weights[get_index(i, j, pSecondModel)];
		}
	}

	return pModel;
}

Pattern* create_pattern(unsigned long width, unsigned long height) {
	Pattern* pPattern = (Pattern*) malloc(sizeof(Pattern));
	pPattern->size = width * height;
	pPattern->width = width;
	pPattern->height = height;

	pPattern->data = (char*) malloc((pPattern->size + 1) * sizeof(char));

	// Make pPattern->data NULL terminated
	pPattern->data[pPattern->size] = 0;

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

	for (int i = 0; i < pPattern->size; i++) {
		pPattern->data[i] = pImage->data[i] == '1' ? 1 : -1;
	}
	
	return pPattern;
}

Model* load_model(char* path) {
	Model* pModel = (Model*) malloc(sizeof(Model));

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

	pModel->size = strtoul(size, NULL, 10);

	// Read Data
	float* data = (float*) malloc(sizeof(float) * pModel->size * pModel->size);
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
			data[get_index(row, column, pModel)] = weight;
			data[get_index(column, row, pModel)] = weight;

			free(weightString);
			weightString = NULL;
			weightSize = 0;
		}
	}
	
	fclose(pFile);

	free(buffer);

	pModel->weights = data;

	return pModel;
}

Model* memorize_pattern(Pattern* pPattern) {
	Model* pModel = create_model(pPattern->size);

	for (int i = 0; i < pPattern->size; i++) {
		for (int j = 0; j < pPattern->size; j++) {
			if (i != j) {
				pModel->weights[get_index(i, j, pModel)] = (pPattern->data[i] * pPattern->data[j]) / (float) pPattern->size;
			} else {
				pModel->weights[get_index(i, j, pModel)] = 0;
			}
		}
	}

	return pModel;
}

void print_model(Model* pModel) {
	for (int j = 0; j < pModel->size; j++) {
		for (int i = 0; i < pModel->size; i++) {
			fprintf(stdout, "%f\t", pModel->weights[get_index(i, j, pModel)] );
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

	for (int j = 0; j < pPattern->height; j++) {
		for (int i = 0; i < pPattern->width; i++) {
			fprintf(pFile, "%c ", pPattern->data[j * pPattern->height + i] == 1 ? '1' : '0');
		}

		fprintf(pFile, "\n");
	}

	fclose(pFile);
}

void save_model(char* path, Model* pModel) {
	FILE* pFile = fopen(path, "w");

	fprintf(pFile, "%lu\n", pModel->size);

	// Just save the lower triangular matrix since the weights are symmetrical between units
	for (int j = 0; j < pModel->size; j++) {
		for (int i = 0; i <= j; i++) {
			fprintf(pFile, "%.6f\t", pModel->weights[get_index(i, j, pModel)]);
		}

		fprintf(pFile, "\n");
	}

	fclose(pFile);
}

int sgn(float input) {
	return input < 0 ? -1 : 1;
}

Pattern* retrieve_pattern(Pattern* pPattern, Model* pModel) {
	Pattern* pLastPattern = NULL;

	do {
		pLastPattern = copy_pattern(pPattern);

		unsigned long* randomIndexes = get_random_sequence(pModel->size);

		for (int i = 0; i < pModel->size; i++) {
			float sum = 0;

			for (int j = 0; j < pModel->size; j++) {
				sum += pModel->weights[get_index(randomIndexes[i], randomIndexes[j], pModel)] * pPattern->data[randomIndexes[j]];
			}
			
			pPattern->data[randomIndexes[i]] = sgn(sum);
		}

	} while (compare_patterns(pPattern, pLastPattern) != 0);

	return pPattern;
}

int compare_patterns(Pattern* pFirstPattern, Pattern* pSecondPattern) {
	if (pFirstPattern->size != pSecondPattern->size)
		return 1;

	if (pFirstPattern->width != pSecondPattern->width)
		return 1;

	if (pFirstPattern->height != pSecondPattern->height)
		return 1;

	if (strcmp(pFirstPattern->data, pSecondPattern->data) != 0)
		return 1;

	return 0;
}