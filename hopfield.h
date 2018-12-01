#ifndef HOPFIELD_H_
#define HOPFIELD_H_

typedef struct Network {
	unsigned long width, height;
	float* weights;
} Network;

#endif