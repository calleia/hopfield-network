#include <stdio.h>
#include <stdlib.h>
#include "pbm.h"

unsigned long width;
unsigned long height;

int* load_pbm_image(char* filename) {
	FILE* pImageFile;

	char* header;
	char* character;
	char* pWidthString;
	char* pHeightString;
	int* pattern;

	size_t patternSize;
	size_t widthLength;
	size_t heightLength;
	size_t index;

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

	// Convert width & height
	width = strtoul(pWidthString, NULL, 10);
	height = strtoul(pHeightString, NULL, 10);

	// Read Data
	patternSize = 0;
	pattern = (int*) malloc(width * height * sizeof(int));

	while (fread(character, 1, 1, pImageFile) == 1) {
		// Converts 0 or 1 to -1 or +1
		if (*character == '0') {
			pattern[patternSize] = -1;
			patternSize++;
		}
		else if (*character == '1') {
			pattern[patternSize] = 1;
			patternSize++;
		}
	}

	// Make sure the parsing worked otherwise return NULL
	if (patternSize == width * height) {
		return pattern;
	} else {
		fprintf(stderr, "Error: wrong data size in image file \'%s\'.\n", filename);
		return NULL;
	}
}

void save_pbm_image(char* filename, int* pixels) {
	FILE* pFile;
	unsigned long index;
	unsigned long i;
	unsigned long j;
	int binaryPixel;

	// Create PBM image file
	pFile = fopen(filename, "w");

	// Write image width & height to PBM file
	fprintf(pFile, "P1\n%lu %lu\n", width, height);

	// Write image data to PBM file
	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			index = j * width + i;

			binaryPixel = pixels[index] == 1 ? 1 : 0;

			// Write each binary pixel to PBM file
			fprintf(pFile, "%d ", binaryPixel);
		}

		fprintf(pFile, "\n");
	}

	fclose(pFile);
}
