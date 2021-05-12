#ifndef PBM_H_
#define PBM_H_

int* load_pbm_image(char* filename);

void save_pbm_image(char* filename, int* pixels);

#endif