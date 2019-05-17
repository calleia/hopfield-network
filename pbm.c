#include <stdio.h>
#include <stdlib.h>
#include "pbm.h"

PBMImage* loadPBM(FILE* pFile) {

	// Read Header
	char* header = (char*) malloc(2 * sizeof(char));
	fread(header, 1, 2, pFile);

	if (header[0] != 'P' || header[1] != '1') {
		return NULL;
	}

	char* buffer = NULL;
	buffer = (char*) malloc(sizeof(char));

	// Skip blanks
	do {
		fread(buffer, 1, 1, pFile);
	} while (*buffer == ' ' || *buffer == '\n');

	// Skip comments
	while (*buffer == '#') {
		do {
			fread(buffer, 1, 1, pFile);
		} while (*buffer != '\n');

		// Skip blanks
		do {
			fread(buffer, 1, 1, pFile);
		} while (*buffer == ' ' || *buffer == '\n');
	}

	// Read Size
	char* width = NULL;
	size_t widthLength = 0;

	char* height = NULL;
	size_t heightLength = 0;

	do {
		widthLength++;
		width = (char*) realloc(width, widthLength + 1);
		width[widthLength - 1] = *buffer;
		fread(buffer, 1, 1, pFile);
	} while (*buffer != ' ');

	// Null terminating string
	width[widthLength] = 0;

	// Skip spaces
	while (*buffer == ' ') {
		fread(buffer, 1, 1, pFile);
	}

	do {
		heightLength++;
		height = (char*) realloc(height, heightLength + 1);
		height[heightLength - 1] = *buffer;
		fread(buffer, 1, 1, pFile);
	} while (*buffer != '\n');

	// Null terminating string
	height[heightLength] = 0;

	// Read Data
	char* data = NULL;
	size_t dataSize = 0;

	while (fread(buffer, 1, 1, pFile) == 1) {
		if (*buffer == '0' || *buffer == '1') {
			dataSize++;
			data = (char*) realloc(data, dataSize);
			data[dataSize - 1] = *buffer;
		}
	}

	// Create file struct
	PBMImage* pImage = (PBMImage*) malloc(sizeof(PBMImage));

	pImage->width = strtoul(width, NULL, 10);
	pImage->height = strtoul(height, NULL, 10);
	pImage->data = data;

	// Make sure the parsing worked otherwise return NULL
	if (dataSize == pImage->width * pImage->height) {
		return pImage;
	} else {
		return NULL;
	}
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
