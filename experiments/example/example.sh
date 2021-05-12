#!/bin/sh


# Clean & Build

echo "build started"

make clean

make

echo "build finished"


# Model training

echo "model training started"

cp experiments/example/store-patterns.tsv settings.tsv

./hopfield

echo "model training finished (weights generated)"


# Pattern 0 retrieval

echo "pattern 0 retrieval started"

cp experiments/example/retrieve-0.tsv settings.tsv

./hopfield

echo "pattern 0 retrieval finished"


# Pattern 1 retrieval

echo "pattern 1 retrieval started"

cp experiments/example/retrieve-1.tsv settings.tsv

./hopfield

echo "pattern 1 retrieval finished"
