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

Model* create_model(unsigned long size);

Model* load_model(char* path);

Model* load_full_model(char* path);

void save_model(char* path, Model* pModel);

void save_full_model(char* path, Model* pModel);

void print_model(Model* pModel);

int test_new_model(Model* pModel);

Model* memorize_pattern(Pattern* pPattern);

Model* memorize_patterns(Pattern* pPattern, Model* pModel);

Pattern* retrieve_pattern(Pattern* pPattern, Model* pModel);

Pattern* copy_pattern(Pattern* pPattern);

Pattern* create_pattern(unsigned long width, unsigned long height);

Pattern* load_image(char* path);

void save_image(char* path, Pattern* pPattern);

void print_pattern(Pattern* pPattern);

int compare_patterns(Pattern* pFirstPattern, Pattern* pSecondPattern);

#endif