#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "hopfield.h"

#define MAX_FILENAME_LENGTH 1024

void save(char** inputFilenameList, char* outputFilename) {
	Model* pModel;
	Pattern* pInputPattern;
	int index;

	pModel = NULL;
	index = 0;

	while (inputFilenameList[index] != NULL) {
		// Read input images
		pInputPattern = load_image(inputFilenameList[index]);

		// Create empty model in the first iteration
		if (pModel == NULL) {
			pModel = create_model(pInputPattern->size);
		}

		// Save input image on the model (update weights)
		pModel = memorize_patterns(pInputPattern, pModel);

		index++;
	}

	// Save model in a file
	save_full_model(outputFilename, pModel);

	// Free memory
	free(pModel);
	free(pInputPattern);
}

void retrieve(char* input_path, char* output_path, char* model_path) {
	Pattern* pPattern = load_image(input_path);

	Model* pModel = load_full_model(model_path);

	Pattern* pRecovered = retrieve_pattern(pPattern, pModel);

	save_image(output_path, pRecovered);
}

void readInputFile(FILE* pInputFile) {
	char* option;
	char* value;
	char* mode;

	char* modelFilename;
	char* outputFilename;
	char* inputFilename;
	char** inputFilenameList;
	int inputCount;

	option = (char*) malloc(MAX_FILENAME_LENGTH * sizeof(char));
	value = (char*) malloc(MAX_FILENAME_LENGTH * sizeof(char));
	mode = (char*) malloc(MAX_FILENAME_LENGTH * sizeof(char));
	
	modelFilename = (char*) malloc(MAX_FILENAME_LENGTH * sizeof(char));
	outputFilename = (char*) malloc(MAX_FILENAME_LENGTH * sizeof(char));
	inputFilename = (char*) malloc(MAX_FILENAME_LENGTH * sizeof(char));
	inputFilenameList = NULL;
	inputCount = 0;

	// Read pairs of (tab separated) settings from input file
	while (fscanf(pInputFile, "%s %s", option, value) == 2) {

		if (strcmp(option, "MODE") == 0) {
			// Read MODE option value
			strcpy(mode, value);
		} else if (strcmp(option, "MODEL_FILENAME") == 0) {
			// Read MODEL_FILENAME option value
			strcpy(modelFilename, value);
		} else if (strcmp(option, "OUTPUT_FILENAME") == 0) {
			// Read OUTPUT_FILENAME option value
			strcpy(outputFilename, value);
		} else if (strcmp(option, "INPUT_FILENAME") == 0) {
			inputCount++;

			// Read INPUT_FILENAME option value
			inputFilename = (char*) malloc(MAX_FILENAME_LENGTH * sizeof(char));
			strcpy(inputFilename, value);

			// Append INPUT_FILENAME to list of inputs
			inputFilenameList = (char**) realloc(inputFilenameList, (inputCount + 1) * sizeof(char*));
			inputFilenameList[inputCount - 1] = inputFilename;
		} else {
			// Log error & stop execution if input file has ANY invalid option  
			fprintf(stderr, "Error: option %s from input file not recognized.\n", option);
			return;
		}
	}

	// Set the end of the pointer
	inputFilenameList[inputCount] = NULL;

	// Verify execution mode value
	if (strcmp(mode, "save") == 0) {
		save(inputFilenameList, outputFilename);
	} else if (strcmp(mode, "retrieve") == 0) {
		retrieve(inputFilename, outputFilename, modelFilename);
	} else {
		fprintf(stderr, "Error: could not recognize execution mode \'%s\'\n", mode);
	}

	// Free memory
	free(option);
	free(value);
	free(mode);
}

int main(int argc, char** argv) {
	FILE* pInputFile;

	// Verify the number of command line arguments
	if (argc != 2) {
		fprintf(stdout, "Error: wrong number of command line arguments.\n");
		return 0;
	}

	// Open input file
	pInputFile = fopen(argv[1], "r");

	// Verify if file was properly open and read it in case of success
	if (pInputFile == NULL) {
		fprintf(stderr, "Error: could not find input file %s\n", argv[1]);
	} else {
		readInputFile(pInputFile);
	}

	// Close input file
	fclose(pInputFile);

	return 0;
}