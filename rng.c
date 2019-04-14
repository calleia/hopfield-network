#include <time.h>
#include <stdlib.h>

int roll(int max) {
	srand(time(NULL));
	int random = rand();

	return random % max;
}
