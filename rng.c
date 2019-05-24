#include <time.h>
#include <stdlib.h>
#include <stdio.h>

int flag = 0;

// Generates a random int between (0) and (max - 1)
int get_random_int(int max) {
	if (flag == 0) {
		srand(time(NULL));
		flag++;
	}

	// rand() returns a pseudo-random integer value between ​0​ and RAND_MAX (0 and RAND_MAX included).
	int random = rand();

	double unit = (double) RAND_MAX / (double) max;

	return random / unit;
}


// Generates randomly ordered sequence
unsigned long* get_random_sequence(unsigned long size) {
	int i;
	int j;
	unsigned long randomIndex;
	unsigned long* orderedSequence;
	unsigned long* randomSequence;

	orderedSequence = malloc(size * sizeof(unsigned long));
	randomSequence = malloc(size * sizeof(unsigned long));

	for (i = 0; i < size; i++) {
		orderedSequence[i] = i;
	}

	for (i = size - 1; i >= 0; i--) {
		randomIndex = get_random_int(i + 1);
		randomSequence[size - (i + 1)] = orderedSequence[randomIndex];

		for (j = randomIndex; j < i; j++) {
			orderedSequence[j] = orderedSequence[j + 1];
		}
	}

	return randomSequence;
}
