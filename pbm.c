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