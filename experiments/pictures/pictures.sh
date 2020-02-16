#!/bin/sh


# Clean & Build

echo "build started"

make clean
make

echo "build finished"


# Model training

echo "model training started"

settings="EXECUTION_MODE	0\n
N_NEURONS	65536\n
TRAINING_SET_SIZE	3\n
INPUT_FILENAME	patterns/pictures/clear/cristo-redentor.pbm\n
INPUT_FILENAME	patterns/pictures/clear/maracana.pbm\n
INPUT_FILENAME	patterns/pictures/clear/uerj.pbm\n"

echo $settings > settings.tsv

./hopfield

echo "model training finished (weights generated)"


# Pattern cristo-redentor retrieval

echo "pattern cristo-redentor retrieval started"

settings="EXECUTION_MODE	1\n
N_NEURONS	65536\n
INITIAL_NETWORK_STATE	patterns/pictures/noisy/cristo-redentor.pbm\n
RETRIEVED_IMAGE_FILENAME	experiments/pictures/output/retrieved-cristo-redentor.pbm"

echo $settings > settings.tsv

./hopfield

echo "pattern cristo-redentor retrieval finished"


# Pattern maracana retrieval

echo "pattern maracana retrieval started"

settings="EXECUTION_MODE	1\n
N_NEURONS	65536\n
INITIAL_NETWORK_STATE	patterns/pictures/noisy/maracana.pbm\n
RETRIEVED_IMAGE_FILENAME	experiments/pictures/output/retrieved-maracana.pbm"

echo $settings > settings.tsv

./hopfield

echo "pattern maracana retrieval finished"


# Pattern uerj retrieval

echo "pattern uerj retrieval started"

settings="EXECUTION_MODE	1\n
N_NEURONS	65536\n
INITIAL_NETWORK_STATE	patterns/pictures/noisy/uerj.pbm\n
RETRIEVED_IMAGE_FILENAME	experiments/pictures/output/retrieved-uerj.pbm"

echo $settings > settings.tsv

./hopfield

echo "pattern uerj retrieval finished"
