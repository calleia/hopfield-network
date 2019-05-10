#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hopfield.h"
#include "pbm.h"
#include "rng.h"

unsigned long get_index(unsigned long x, unsigned long y, unsigned long size) {
	return size * y + x;
}

int sgn(float input) {
	return input < 0 ? -1 : 1;
}

Model* create_model(unsigned long size) {
	void check_memory_allocation();
	int test_new_model();

	Model* pModel;
	int testNewModelResult;

	// Allocate memory for model struct
	pModel = (Model*) malloc(sizeof(Model));
	check_memory_allocation(pModel, "pModel", "create_model()");

	pModel->size = size;

	pModel->weights = (float*) calloc(size * size, sizeof(float));
	check_memory_allocation(pModel->weights, "pModel->weights", "create_model()");

	// Test if memory allocation was successful
	testNewModelResult = test_new_model(pModel);

	if (testNewModelResult == 1) {
		fprintf(stderr, "Error: an error ocurred while creating a model.\n");
		exit(0);
	}

	// Return new empty model
	return pModel;
}

Model* load_full_model(char* path) {
	Model* create_model();
	void check_memory_allocation();

	FILE* pFile;
	Model* pModel;
	float* pWeight;
	unsigned long* pModelSize;
	unsigned long weightCount;

	pWeight = malloc(sizeof(float));
	check_memory_allocation(pWeight, "pWeight", "load_full_model()");

	pModelSize = malloc(sizeof(unsigned long));
	check_memory_allocation(pModelSize, "pModelSize", "load_full_model()");

	pFile = fopen(path, "r");

	// Read model size from file (total weight count == size^2)
	if (fscanf(pFile, "%ld", pModelSize) == EOF) {
		fprintf(stderr, "%s\n", "Could not read model size.");
	}

	// Create new (empty) model
	pModel = create_model(*pModelSize);

	weightCount = 0;

	// Read weights from file
	while (fscanf(pFile, "%f", pWeight) != EOF) {
		if (weightCount >= pModel->size * pModel->size) {
			fprintf(stderr, "%s\n", "Model file has an invalid number of weights.");
			break;
		}

		pModel->weights[weightCount] = *pWeight;
		weightCount++;
	}

	if (weightCount != pModel->size * pModel->size) {
		fprintf(stderr, "%s\n", "Model file has an invalid number of weights.");
	}

	// Freeing allocated memory
	fclose(pFile);
	free(pWeight);
	free(pModelSize);

	return pModel;
}

void save_full_model(char* path, Model* pModel) {
	unsigned long get_index();

	FILE* pFile;
	unsigned long i;
	unsigned long j;
	unsigned long weightIndex;

	// Create model file
	pFile = fopen(path, "w");

	// Write the model size in the model file
	fprintf(pFile, "%lu\n", pModel->size);

	// Save the whole square matrix of weights
	for (j = 0; j < pModel->size; j++) {
		for (i = 0; i < pModel->size; i++) {
			// Calculate current weight index
			weightIndex = get_index(i, j, pModel->size);

			// Write current weight in the model file
			fprintf(pFile, "%.6f\t", pModel->weights[weightIndex]);
		}

		fprintf(pFile, "\n");
	}

	fclose(pFile);
}

int test_new_model(Model* pModel) {
	unsigned long totalSize;
	unsigned long i;
	
	if (pModel ==  NULL)
		return 1;

	if (pModel->weights == NULL)
		return 1;

	if (pModel->size < 1)
		return 1;

	totalSize = pModel->size * pModel->size;

	for (i = 0; i < totalSize; i++) {
		if (pModel->weights[i] != 0.0)
			return 1;
	}

	return 0;
}

Model* memorize_patterns(Pattern* pPattern, Model* pModel) {
	unsigned long get_index();

	unsigned long weightIndex;
	unsigned long i;
	unsigned long j;
	float weight;

	for (i = 0; i < pPattern->size; i++) {
		for (j = 0; j < pPattern->size; j++) {
			weightIndex = get_index(i, j, pModel->size);
			
			if (i != j) {
				weight = pPattern->data[i] * pPattern->data[j] / (float) pPattern->size;
				pModel->weights[weightIndex] += weight;
			} else {
				pModel->weights[weightIndex] = 0;
			}
		}
	}

	return pModel;
}

Pattern* retrieve_pattern(Pattern* pPattern, Model* pModel) {
	Pattern* copy_pattern();
	unsigned long* get_random_sequence();
	unsigned long get_index();
	int compare_patterns();

	Pattern* pLastPattern;
	unsigned long weightIndex;
	unsigned long i;
	unsigned long j;
	int isTheSamePattern;
	float sum;

	do {
		pLastPattern = copy_pattern(pPattern);

		unsigned long* randomIndexes = get_random_sequence(pModel->size);

		for (i = 0; i < pModel->size; i++) {
			sum = 0;

			for (j = 0; j < pModel->size; j++) {
				weightIndex = get_index(randomIndexes[i], randomIndexes[j], pModel->size);
				sum += pModel->weights[weightIndex] * pPattern->data[randomIndexes[j]];
			}
			
			pPattern->data[randomIndexes[i]] = sgn(sum);
		}

		isTheSamePattern = compare_patterns(pPattern, pLastPattern);
	} while (isTheSamePattern != 0);

	return pPattern;
}

Pattern* copy_pattern(Pattern* pPattern) {
	Pattern* create_pattern();

	Pattern* pCopiedPattern;
	pCopiedPattern = create_pattern(pPattern->width, pPattern->height);
	strcpy(pCopiedPattern->data, pPattern->data);

	return pCopiedPattern;
}

Pattern* create_pattern(unsigned long width, unsigned long height) {
	void check_memory_allocation();

	Pattern* pPattern;
	unsigned long totalDataLength;

	pPattern = (Pattern*) malloc(sizeof(Pattern));
	check_memory_allocation(pPattern, "pPattern", "create_pattern()");

	pPattern->size = width * height;
	pPattern->width = width;
	pPattern->height = height;

	totalDataLength = pPattern->size + 1;
	pPattern->data = (char*) malloc(totalDataLength * sizeof(char));
	check_memory_allocation(pPattern->data, "pPattern->data", "create_pattern()");

	// Make pPattern->data NULL terminated
	pPattern->data[pPattern->size] = 0;

	return pPattern;
}

Pattern* load_image(char* path) {
	PBMImage* loadPBM();
	Pattern* create_pattern();

	FILE* pFile;
	PBMImage* pImage;
	Pattern* pPattern;
	unsigned long index;
	char unit;

	pFile = fopen(path, "rb");
	pImage = loadPBM(pFile);
	pPattern = create_pattern(pImage->width, pImage->height);

	for (index = 0; index < pPattern->size; index++) {
		unit = pImage->data[index] == '1' ? 1 : -1;
		pPattern->data[index] = unit;
	}

	fclose(pFile);

	return pPattern;
}

void save_image(char* path, Pattern* pPattern) {
	unsigned long get_index();

	FILE* pFile;
	unsigned long index;
	unsigned long i;
	unsigned long j;
	char binaryPixel;

	// Create PBM image file
	pFile = fopen(path, "w");

	// Write image width & height to PBM file
	fprintf(pFile, "P1\n%lu %lu\n", pPattern->width, pPattern->height);

	// Write image data to PBM file
	for (j = 0; j < pPattern->height; j++) {
		for (i = 0; i < pPattern->width; i++) {
			index = get_index(i, j, pPattern->height);
			binaryPixel = pPattern->data[index] == 1 ? '1' : '0';

			// Write each binary pixel to PBM file
			fprintf(pFile, "%c ", binaryPixel);
		}

		fprintf(pFile, "\n");
	}

	fclose(pFile);
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

void check_memory_allocation(void* pPointer, char* pPointerName, char* pFunctionName) {
	if (pPointer == NULL) {
		fprintf(stderr, "Error: could not allocate memory for pointer \'%s\' in function \'%s\'.\n", pPointerName, pFunctionName);

		exit(0);
	}
}
