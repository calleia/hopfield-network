#!/bin/sh


# Clean & Build

echo build started

make clean

make

echo build finished


# Model training

echo "model training started"

cp experiments/spurious-states/store-patterns.tsv settings.tsv

./hopfield

echo weights generated

echo "model training finished"


# Pattern 3 retrieval

echo "pattern 3 retrieval started"

cp experiments/spurious-states/retrieve-3.tsv settings.tsv

./hopfield

echo "pattern 3 retrieval finished"


# Pattern 8 retrieval

echo "pattern 8 retrieval started"

cp experiments/spurious-states/retrieve-8.tsv settings.tsv

./hopfield

echo "pattern 8 retrieval finished"


# Pattern 9 retrieval

echo "pattern 9 retrieval started"

cp experiments/spurious-states/retrieve-9.tsv settings.tsv

./hopfield

echo "pattern 9 retrieval finished"
