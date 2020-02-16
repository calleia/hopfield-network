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
TRAINING_SET_SIZE	3\n
INPUT_FILENAME	patterns/32x32/clear/3.pbm\n
INPUT_FILENAME	patterns/32x32/clear/8.pbm\n
INPUT_FILENAME	patterns/32x32/clear/9.pbm\n"

echo $settings > settings.tsv

./hopfield

echo weights generated

echo model training finished

# Pattern 3 retrieval

echo pattern 3 retrieval started

settings="EXECUTION_MODE	1\n
N_NEURONS	1024\n
INITIAL_NETWORK_STATE	patterns/32x32/clear/3.pbm\n
RETRIEVED_IMAGE_FILENAME	experiments/spurious-states/output/retrieved-3.pbm"

echo $settings > settings.tsv

./hopfield

echo pattern 3 retrieval finished

# Pattern 8 retrieval

echo pattern 8 retrieval started

settings="EXECUTION_MODE	1\n
N_NEURONS	1024\n
INITIAL_NETWORK_STATE	patterns/32x32/clear/8.pbm\n
RETRIEVED_IMAGE_FILENAME	experiments/spurious-states/output/retrieved-8.pbm"

echo $settings > settings.tsv

./hopfield

echo pattern 8 retrieval finished

# Pattern 9 retrieval

echo pattern 9 retrieval started

settings="EXECUTION_MODE	1\n
N_NEURONS	1024\n
INITIAL_NETWORK_STATE	patterns/32x32/clear/9.pbm\n
RETRIEVED_IMAGE_FILENAME	experiments/spurious-states/output/retrieved-9.pbm"

echo $settings > settings.tsv

./hopfield

echo pattern 9 retrieval finished
