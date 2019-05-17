#ifndef PBM_H_
#define PBM_H_

typedef struct PBMImage { 
    unsigned long width, height;
    char* data;
} PBMImage;

PBMImage* loadPBM(FILE* pFile);

char* load_pbm_image(char* filename);

#endif