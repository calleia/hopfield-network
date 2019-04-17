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
