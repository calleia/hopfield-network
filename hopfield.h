#ifndef HOPFIELD_H_
#define HOPFIELD_H_

typedef struct Model {
	unsigned long size;
	float* weights;
} Model;

typedef struct Pattern {
	unsigned long size, width, height;
	char* data;
} Pattern;

Model* add_models(Model* pFirstModel, Model* pSecondModel);

Model* create_model(unsigned long size);

Pattern* create_pattern(unsigned long width, unsigned long height);

Pattern* copy_pattern(Pattern* pPattern);

Pattern* load_image(char* path);

Model* load_model(char* path);

Model* memorize_pattern(Pattern* pPattern);

void print_model(Model* pModel);

void print_pattern(Pattern* pPattern);

Pattern* retrieve_pattern(Pattern* pPattern, Model* pModel);

void save_image(char* path, Pattern* pPattern);

void save_model(char* path, Model* pModel);

int compare_patterns(Pattern* pFirstPattern, Pattern* pSecondPattern);

#endif