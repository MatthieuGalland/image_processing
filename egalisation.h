#ifndef EGALISATION_H
#define EGALISATION_H

#include "utils.h"
#include "utils24.h"


unsigned int* bmp8_computeHistogram(t_bmp8* img);
unsigned int* bmp8_getCDF(unsigned int* hist);
unsigned int* bmp8_computeCDF(unsigned int* cdf, int N);
void bmp8_equalize(t_bmp8* img, double N);

void bmp24_equalize(t_bmp24* img);

#endif // EGALISATION_H