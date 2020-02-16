#!/bin/sh

# Clean & Build

echo build started

make clean
make

echo build finished

# Model training

echo model training started

settings="EXECUTION_MODE	0\n
N_NEURONS	1024\n
TRAINING_SET_SIZE	2\n
INPUT_FILENAME	patterns/32x32/clear/0.pbm\n
INPUT_FILENAME	patterns/32x32/clear/1.pbm\n"

echo $settings > settings.tsv

./hopfield

echo weights generated

echo model training finished

# Pattern 0 retrieval

echo pattern 0 retrieval started

settings="EXECUTION_MODE	1\n
N_NEURONS	1024\n
INITIAL_NETWORK_STATE	patterns/32x32/noisy/0.pbm\n
RETRIEVED_IMAGE_FILENAME	experiments/example/output/retrieved-0.pbm"

echo $settings > settings.tsv

./hopfield

echo pattern 0 retrieval finished

# Pattern 1 retrieval

echo pattern 1 retrieval started

settings="EXECUTION_MODE	1\n
N_NEURONS	1024\n
INITIAL_NETWORK_STATE	patterns/32x32/noisy/1.pbm\n
RETRIEVED_IMAGE_FILENAME	experiments/example/output/retrieved-1.pbm"

echo $settings > settings.tsv

./hopfield

echo pattern 1 retrieval finished

