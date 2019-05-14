#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "hopfield.h"

#define MAX_FILENAME_LENGTH 1024
#define SETTINGS_FILENAME "settings.tsv"

// Execution modes
#define STORE 0
#define RETRIEVE 1
#define STORE_AND_RETRIEVE 2

int executionMode;
unsigned long nNeurons;
int trainingSetSize;

char** pTrainingSet;

void save(char** inputFilenameList, char* outputFilename) {
	Pattern* load_image();
	Model* create_model();
	Model* memorize_patterns();
	void save_full_model();

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
	Pattern* load_image();
	Pattern* retrieve_pattern();
	Model* load_full_model();
	void save_image();

	Pattern* pPattern;
	Pattern* pRecovered;
	Model* pModel;

	pPattern = load_image(input_path);
	pModel = load_full_model(model_path);
	pRecovered = retrieve_pattern(pPattern, pModel);
	
	save_image(output_path, pRecovered);
}

void readInputFile(FILE* pInputFile) {
	void check_memory_allocation();
	void save();
	void retrieve();

	char* option;
	char* value;
	char* mode;

	char* modelFilename;
	char* outputFilename;
	char* inputFilename;
	char** inputFilenameList;
	int inputCount;

	option = (char*) malloc(MAX_FILENAME_LENGTH * sizeof(char));
	check_memory_allocation(option, "option", "readInputFile()");

	value = (char*) malloc(MAX_FILENAME_LENGTH * sizeof(char));
	check_memory_allocation(value, "value", "readInputFile()");

	mode = (char*) malloc(MAX_FILENAME_LENGTH * sizeof(char));
	check_memory_allocation(mode, "mode", "readInputFile()");
	
	modelFilename = (char*) malloc(MAX_FILENAME_LENGTH * sizeof(char));
	check_memory_allocation(modelFilename, "modelFilename", "readInputFile()");

	outputFilename = (char*) malloc(MAX_FILENAME_LENGTH * sizeof(char));
	check_memory_allocation(outputFilename, "outputFilename", "readInputFile()");

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
			check_memory_allocation(inputFilename, "inputFilename", "readInputFile()");

			strcpy(inputFilename, value);

			// Append INPUT_FILENAME to list of inputs
			inputFilenameList = (char**) realloc(inputFilenameList, (inputCount + 1) * sizeof(char*));
			check_memory_allocation(inputFilenameList, "inputFilenameList", "readInputFile()");
			
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

void input_network_parameters(FILE* pSettingsFile) {
	if (pSettingsFile == NULL) {
		fprintf(stderr, "Error: input settings file could not be found.\n");
		exit(0);
	}

	fscanf(pSettingsFile, "%*s\t%d\n%*s\t%lu", &executionMode, &nNeurons);
}

char* load_pbm_image(char* filename) {
	FILE* pImageFile;

	char* header;
	char* character;
	char* pattern;
	char* pWidthString;
	char* pHeightString;

	size_t patternSize;
	size_t widthLength;
	size_t heightLength;
	size_t width;
	size_t height;

	pImageFile = fopen(filename, "r");

	if (pImageFile == NULL) {
		fprintf(stderr, "Error: could not load image file \'%s\'.\n", filename);
		exit(0);
	}

	// Read Header
	header = (char*) malloc(2 * sizeof(char));
	fread(header, 1, 2, pImageFile);

	if (header[0] != 'P' || header[1] != '1') {
		fprintf(stderr, "Error: could not recognize image file format \'%s\'.\n", filename);
		return NULL;
	}

	character = (char*) malloc(sizeof(char));

	// Skip blanks
	do {
		fread(character, 1, 1, pImageFile);
	} while (*character == ' ' || *character == '\t' || *character == '\n');

	// Skip comments
	while (*character == '#') {
		do {
			fread(character, 1, 1, pImageFile);
		} while (*character != '\n');

		// Skip blanks
		do {
			fread(character, 1, 1, pImageFile);
		} while (*character == ' ' || *character == '\t' || *character == '\n');
	}

	// Read Size
	widthLength = 0;
	pWidthString = NULL;

	do {
		widthLength++;
		pWidthString = (char*) realloc(pWidthString, widthLength + 1);
		pWidthString[widthLength - 1] = *character;
		fread(character, 1, 1, pImageFile);
	} while (*character != ' ' && *character != '\t');

	// Null terminating string
	pWidthString[widthLength] = 0;

	// Skip spaces
	while (*character == ' ' || *character == '\t') {
		fread(character, 1, 1, pImageFile);
	}

	heightLength = 0;
	pHeightString = NULL;

	do {
		heightLength++;
		pHeightString = (char*) realloc(pHeightString, heightLength + 1);
		pHeightString[heightLength - 1] = *character;
		fread(character, 1, 1, pImageFile);
	} while (*character != '\n');

	// Null terminating string
	pHeightString[heightLength] = 0;

	// Read Data
	patternSize = 0;
	pattern = NULL;

	while (fread(character, 1, 1, pImageFile) == 1) {
		if (*character == '0' || *character == '1') {
			patternSize++;
			pattern = (char*) realloc(pattern, patternSize);
			pattern[patternSize - 1] = *character;
		}
	}

	width = strtoul(pWidthString, NULL, 10);
	height = strtoul(pHeightString, NULL, 10);

	// Make sure the parsing worked otherwise return NULL
	if (patternSize == width * height) {
		return pattern;
	} else {
		fprintf(stderr, "Error: wrong data size in image file \'%s\'.\n", filename);
		return NULL;
	}
}

void input_stored_patterns(FILE* pSettingsFile) {
	unsigned long totalSize;
	char* pInputFilename;
	char* pInputPattern;

	fscanf(pSettingsFile, "%*s\t%d", &trainingSetSize);

	totalSize = trainingSetSize * nNeurons;

	pTrainingSet = (char**) malloc(totalSize * sizeof(char*));

	pInputFilename = (char*) malloc(MAX_FILENAME_LENGTH * sizeof(char*));

	int i;
	for (i = 0; i < trainingSetSize; i++) {
		fscanf(pSettingsFile, "%*s %s", pInputFilename);
		pInputPattern = load_pbm_image(pInputFilename);
		pTrainingSet[i] = pInputPattern;
	}
}

int main(int argc, char** argv) {
	FILE* pSettingsFile;
	pSettingsFile = fopen(SETTINGS_FILENAME, "r");

	// Input network parameters
	input_network_parameters(pSettingsFile);

	if (executionMode == STORE || executionMode == STORE_AND_RETRIEVE) {
		// Input stored patterns
		input_stored_patterns(pSettingsFile);
		// TODO: calculate weights
		// TODO: store weights
	}

	if (executionMode == RETRIEVE) {
		// TODO: input weights
	}

	if (executionMode == RETRIEVE || executionMode == STORE_AND_RETRIEVE) {
		// TODO: input initial network state
		// TODO: retrieve stored pattern
		// TODO: output stored pattern
	}

	fclose(pSettingsFile);

	return 0;
}