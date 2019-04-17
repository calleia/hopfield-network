#include <time.h>
#include <stdlib.h>

int flag = 0;

// Generates a random int between (0) and (max - 1)
int roll(int max) {
	if (flag == 0) {
		srand(time(NULL));
		flag++;
	}
	
	int random = rand();

	return random % max;
}

// Generates randomly ordered sequence
unsigned long* get_random_sequence(unsigned long size) {
	unsigned long* sequence = malloc(size * sizeof(unsigned long));

	for (int i = 0; i < size; i++) {
		sequence[i] = i;
	}

	for (int i = 0; i < size; i++) {
		unsigned long aux = sequence[i];
		unsigned long targetIndex = roll(size);
		sequence[i] = sequence[targetIndex];
		sequence[targetIndex] = aux;
	}

	return sequence;
}
