#include <stdio.h>
#include <stdlib.h>

typedef struct PBMImage { 
    unsigned long width, height;
    char* data;
} PBMImage;