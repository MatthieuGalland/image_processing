//
// Created by sandw on 02/05/2025.
//
#include "utils.h"
#include "utils24.h"

#ifndef EGALISATION_H
#define EGALISATION_H

unsigned int * bmp8_computeHistogram(t_bmp8 * img);
unsigned int * bmp8_computeCDF(unsigned int * hist);
void bmp8_equalize(t_bmp8 * img, unsigned int * hist_eq);


#endif //EGALISATION_H
