#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "pbm.h"

#define MAX_FILENAME_LENGTH 1024
#define SETTINGS_FILENAME "settings.tsv"
#define WEIGHTS_FILENAME "weights.hn"

// Execution modes
#define STORE 0
#define RETRIEVE 1
#define STORE_AND_RETRIEVE 2

// Network state comparison values
#define EQUAL_STATES 0
#define DIFFERENT_STATES 1

int executionMode;
unsigned long nNeurons;
int trainingSetSize;

float** w;
int* s;
int** pTrainingSet;

void check_memory_allocation(void* pPointer, char* pPointerName, char* pFunctionName) {
	if (pPointer == NULL) {
		fprintf(stderr, "Error: could not allocate memory for pointer \'%s\' in function \'%s\'.\n", pPointerName, pFunctionName);

		exit(0);
	}
}

int sgn(double value) {
	return value < 0 ? -1 : 1;
}

int compare_states(int* s0, int* s1) {
	unsigned long index;

	for (index = 0; index < nNeurons; index++)
		if (s0[index] != s1[index])
			return DIFFERENT_STATES;

	return EQUAL_STATES;
}

void input_network_parameters(FILE* pSettingsFile) {
	if (pSettingsFile == NULL) {
		fprintf(stderr, "Error: input settings file could not be found.\n");
		exit(0);
	}

	fscanf(pSettingsFile, "%*s\t%d\n%*s\t%lu", &executionMode, &nNeurons);
}

void input_stored_patterns(FILE* pSettingsFile) {
	void check_memory_allocation();
	int* load_pbm_image();

	unsigned long totalSize;
	char* pInputFilename;
	int* pInputPattern;

	fscanf(pSettingsFile, "%*s\t%d", &trainingSetSize);

	totalSize = trainingSetSize * nNeurons;

	pTrainingSet = (int**) malloc(totalSize * sizeof(int*));
	check_memory_allocation(pTrainingSet, "pTrainingSet", "input_stored_patterns()");

	pInputFilename = (char*) malloc(MAX_FILENAME_LENGTH * sizeof(char*));
	check_memory_allocation(pInputFilename, "pInputFilename", "input_stored_patterns()");

	int i;
	for (i = 0; i < trainingSetSize; i++) {
		fscanf(pSettingsFile, "%*s %s", pInputFilename);
		pInputPattern = load_pbm_image(pInputFilename);
		pTrainingSet[i] = pInputPattern;
	}
}

void calculate_weights() {
	void check_memory_allocation();

	unsigned long i;
	unsigned long j;
	unsigned long k;

	w = (float**) malloc(nNeurons * sizeof(float*));
	check_memory_allocation(w, "w", "calculate_weights()");

	for (i = 0; i < nNeurons; i++) {
		w[i] = (float*) calloc(nNeurons, sizeof(float));
		check_memory_allocation(w[i], "w[i]", "calculate_weights()");
	}

	for (k = 0; k < trainingSetSize; k++) {
		for (i = 0; i < nNeurons; i++) {
			for (j = 0; j < nNeurons; j++) {
				if (i != j)
					w[i][j] += pTrainingSet[k][i] * pTrainingSet[k][j];
			}
		}
	}

	for (i = 0; i < nNeurons; i++) {
		for (j = 0; j < nNeurons; j++) {
			w[i][j] = w[i][j] / (float) nNeurons;
		}
	}
}

void store_weights(char* filename) {
	void check_memory_allocation();

	FILE* pFile;
	unsigned long i;
	unsigned long j;

	// Create weights file
	pFile = fopen(filename, "w");
	check_memory_allocation(pFile, "pFile", "store_weights()");

	// Write the network size in the weights file
	fprintf(pFile, "%lu\n", nNeurons);

	// Save the whole square matrix of weights
	for (i = 0; i < nNeurons; i++) {
		for (j = 0; j < nNeurons; j++) {
			// Write current weight in the model file
			fprintf(pFile, "%.6f\t", w[i][j]);
		}

		fprintf(pFile, "\n");
	}

	fclose(pFile);
}

void input_weights(char* pFilename) {
	void check_memory_allocation();

	FILE* pFile;
	float* pWeight;
	unsigned long* pModelSize;
	unsigned long weightCount;
	unsigned long totalWeightCount;
	unsigned long i;
	unsigned long j;

	w = (float**) malloc(nNeurons * sizeof(float*));
	check_memory_allocation(w, "w", "input_weights()");

	for (i = 0; i < nNeurons; i++) {
		w[i] = (float*) calloc(nNeurons, sizeof(float));
		check_memory_allocation(w[i], "w[i]", "input_weights()");
	}

	pWeight = malloc(sizeof(float));
	check_memory_allocation(pWeight, "pWeight", "input_weights()");

	pModelSize = malloc(sizeof(unsigned long));
	check_memory_allocation(pModelSize, "pModelSize", "input_weights()");

	pFile = fopen(pFilename, "r");
	check_memory_allocation(pFile, "pFile", "input_weights()");

	// Read model size from file (total weight count == size^2)
	if (fscanf(pFile, "%ld", pModelSize) == EOF) {
		fprintf(stderr, "%s\n", "Could not read model size.");
	}

	if (*pModelSize != nNeurons) {
		fprintf(stderr, "%s\n", "Wrong number of weights in model file.");
	}

	weightCount = 0;

	// Read weights from file
	while (fscanf(pFile, "%f", pWeight) != EOF) {
		if (weightCount >= nNeurons * nNeurons) {
			fprintf(stderr, "%s\n", "Model file has an invalid number of weights.");
			break;
		}

		// Calculate weight matrix indexes
		i = weightCount / nNeurons;
		j = weightCount % nNeurons;
		
		w[i][j] = *pWeight;
		weightCount++;
	}

	totalWeightCount = nNeurons * nNeurons;

	if (weightCount != totalWeightCount) {
		fprintf(stderr, "%s\n", "Model file has an invalid number of weights.");
	}

	// Freeing allocated memory
	fclose(pFile);
	free(pWeight);
	free(pModelSize);
}

void input_initial_network_state(FILE* pSettingsFile) {
	void check_memory_allocation();
	int* load_pbm_image();

	char* pInputFilename;

	pInputFilename = (char*) malloc(MAX_FILENAME_LENGTH * sizeof(char*));
	check_memory_allocation(pInputFilename, "pInputFilename", "input_initial_network_state()");

	fscanf(pSettingsFile, "%*s %s", pInputFilename);
	s = load_pbm_image(pInputFilename);

	free(pInputFilename);
}

void retrieve_stored_pattern() {
	void check_memory_allocation();
	int compare_states();

	int* lastS;
	int i;
	int j;
	double h;

	lastS = (int*) malloc(nNeurons * sizeof(int));
	check_memory_allocation(lastS, "lastS", "retrieve_stored_pattern()");

	do {
		// Create a copy of the network state s
		for (i = 0; i < nNeurons; i++)
			lastS[i] = s[i];

		for (i = 0; i < nNeurons; i++) {
			h = 0.0;

			for (j = 0; j < nNeurons; j++) {
				h += w[i][j] * s[j];
			}

			s[i] = sgn(h);
		}
	} while (compare_states(s, lastS) == DIFFERENT_STATES);

	free(lastS);
}

void output_stored_pattern(FILE* pSettingsFile) {
	void check_memory_allocation();
	void save_pbm_image();

	char* pOutputFilename;

	pOutputFilename = (char*) malloc(MAX_FILENAME_LENGTH * sizeof(char));
	check_memory_allocation(pOutputFilename, "pOutputFilename", "output_stored_pattern()");

	fscanf(pSettingsFile, "%*s\t%s", pOutputFilename);
	save_pbm_image(pOutputFilename, s);

	free(pOutputFilename);
}

int main(int argc, char** argv) {
	void check_memory_allocation();
	void input_network_parameters();
	void input_stored_patterns();
	void calculate_weights();
	void store_weights();
	void input_weights();
	void input_initial_network_state();
	void retrieve_stored_pattern();
	void output_stored_pattern();

	FILE* pSettingsFile;
	pSettingsFile = fopen(SETTINGS_FILENAME, "r");

	// Input network parameters
	input_network_parameters(pSettingsFile);

	if (executionMode == STORE || executionMode == STORE_AND_RETRIEVE) {
		// Input stored patterns
		input_stored_patterns(pSettingsFile);

		// Calculate weights
		calculate_weights();

		// Store weights
		store_weights(WEIGHTS_FILENAME);
	}

	if (executionMode == RETRIEVE) {
		// Input weights
		input_weights(WEIGHTS_FILENAME);
	}

	if (executionMode == RETRIEVE || executionMode == STORE_AND_RETRIEVE) {
		// Input initial network state
		input_initial_network_state(pSettingsFile);

		// Retrieve stored pattern
		retrieve_stored_pattern();

		// Output stored pattern
		output_stored_pattern(pSettingsFile);
	}

	fclose(pSettingsFile);

	return 0;
}