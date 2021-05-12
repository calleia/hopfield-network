#!/bin/sh


# Clean & Build

echo "build started"

make clean

make

echo "build finished"


# Model training

echo "model training started"

cp experiments/pictures/store-patterns.tsv settings.tsv

./hopfield

echo "model training finished (weights generated)"


# Pattern cristo-redentor retrieval

echo "pattern cristo-redentor retrieval started"

cp experiments/pictures/retrieve-cristo-redentor.tsv settings.tsv

./hopfield

echo "pattern cristo-redentor retrieval finished"


# Pattern maracana retrieval

echo "pattern maracana retrieval started"

cp experiments/pictures/retrieve-maracana.tsv settings.tsv

./hopfield

echo "pattern maracana retrieval finished"


# Pattern uerj retrieval

echo "pattern uerj retrieval started"

cp experiments/pictures/retrieve-uerj.tsv settings.tsv

./hopfield

echo "pattern uerj retrieval finished"
