#include <stdio.h>
#include <stdlib.h>
#include "hopfield.h"

void print_pattern(Pattern* pPattern) {
	for (int i = 0; i < pPattern->size; i++) {
		fprintf(stdout, "%d\t", pPattern->data[i]);
	}
}