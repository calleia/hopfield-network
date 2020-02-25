#include <time.h>
#include <stdlib.h>

// Random number generator seed initialization
void set_random_seed() {
    srand(time(NULL));
}

int get_random_int(int max) {
    // rand() returns a pseudo-random integer value between ​0​ and RAND_MAX (0 and RAND_MAX included).
    int random = rand();

    return random % max;
}

// Generates randomly ordered sequence (Fisher–Yates/Knuth shuffle)
int* get_random_sequence(int size) {
    int i;
    int j;
    int* sequence;

    sequence = malloc(size * sizeof(int));

    for (i = 0; i < size; i++) {
        sequence[i] = i;
    }
    
    for (i = size - 1; i > 0; i--) {
        j = get_random_int(i + 1);
        
        int aux = sequence[i];
        sequence[i] = sequence[j];
        sequence[j] = aux;
    }
    
    return sequence;
}
