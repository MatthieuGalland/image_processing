#include <stdio.h>
#include "egalisation.h"

#include <stdlib.h>

unsigned int * bmp8_computeHistogram(t_bmp8 * img) {
    unsigned char* pixels = img->data;
    unsigned int* histogramme = calloc(256,sizeof(unsigned int));
    for(int i=0;i<img->height;i++) {
        for (int j=0;j<img->width;j++) {
            histogramme[*(pixels+i*img->width+j)]++;
        }
    }
    return histogramme;
}

unsigned int * bmp8_computeCDF(unsigned int * hist) {
    for (int i=1;i<255;i++) {
        hist[i]+=hist[i-1];
    }
    return hist;
}