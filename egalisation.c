// egalisation.c
// 
// Ce fichier implémente des algorithmes d'égalisation d'histogramme pour le traitement d'images.
// Il contient des fonctions pour calculer les histogrammes, les fonctions de distribution
// cumulatives (CDFs), et effectuer l'égalisation d'histogramme sur des images BMP en niveaux
// de gris 8 bits et en couleur 24 bits.
// L'égalisation des images couleur est réalisée dans l'espace colorimétrique YUV, en égalisant
// uniquement la composante de luminance (Y) tout en préservant la chrominance.

#include <stdio.h>
#include "egalisation.h"
#include "utils.h"
#include <stdlib.h>
#include <math.h>

unsigned int* bmp8_computeHistogram(t_bmp8* img) {
    unsigned char* pixels = img->data;
    unsigned int* histogramme = calloc(256, sizeof(unsigned int));
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            histogramme[*(pixels + i * img->width + j)]++;
        }
    }
    return histogramme;
}

unsigned int* bmp8_getCDF(unsigned int* hist) {
    unsigned int* cdf = calloc(256, sizeof(unsigned int));
    cdf[0] = hist[0];
    for (int i = 1; i <= 255; i++) {
        cdf[i] = cdf[i-1] + hist[i];
    }
    return cdf;
}

unsigned int* bmp8_computeCDF(unsigned int* cdf, int N) {
    unsigned int* hist_eq = calloc(256, sizeof(unsigned int));
    int cdf_min;
    int i = 0;
    while (cdf[i] == 0)
        i++;
    cdf_min = cdf[i];
    for (i = 0; i <= 255; i++) {
        hist_eq[i] = round(((double)(cdf[i] - cdf_min) / (N - cdf_min)) * 255);
    }
    return hist_eq;
}

void bmp8_equalize(t_bmp8* img, double N) {
    unsigned int* hist = bmp8_computeHistogram(img);
    unsigned int* cdf = bmp8_getCDF(hist);
    int nb_pixels = img->width * img->height;
    unsigned int* hist_eq = bmp8_computeCDF(cdf, nb_pixels);
    
    for (int i = 0; i < img->dataSize; i++) {
        img->data[i] = hist_eq[img->data[i]];
    }
    

    free(hist);
    free(cdf);
    free(hist_eq);
}

void bmp24_equalize(t_bmp24* img) {
    int width = img->width;
    int height = img->height;
    int nb_pixels = width * height;

    // Allocation des tableaux 2D
    int** Y = malloc(height * sizeof(int*));
    float** U = malloc(height * sizeof(float*));
    float** V = malloc(height * sizeof(float*));
    unsigned int* histogramme = calloc(256, sizeof(unsigned int)); // 256 niveaux de gris

    for (int i = 0; i < height; i++) {
        Y[i] = malloc(width * sizeof(int));
        U[i] = malloc(width * sizeof(float));
        V[i] = malloc(width * sizeof(float));
    }

    // conversion -> YUV et remplir l'histogramme de Y
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            t_pixel pixel = img->data[i][j];

            float y = 0.299 * pixel.red + 0.587 * pixel.green + 0.114 * pixel.blue;
            y = fmin(fmax(y, 0), 255);
            Y[i][j] = (int)round(y);

            U[i][j] = -0.14713 * pixel.red - 0.28886 * pixel.green + 0.436 * pixel.blue;
            V[i][j] =  0.615   * pixel.red - 0.51499 * pixel.green - 0.10001 * pixel.blue;

            histogramme[Y[i][j]]++;
        }
    }
    
    // égalisation de Y
    unsigned int* cdf = bmp8_getCDF(histogramme);
    unsigned int* hist_eq = bmp8_computeCDF(cdf, nb_pixels);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            Y[i][j] = hist_eq[Y[i][j]];
        }
    }
    
    // conversion YUV -> RGB
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            float y = Y[i][j];
            float u = U[i][j];
            float v = V[i][j];

            int r = round(y + 1.13983 * v);
            int g = round(y - 0.39465 * u - 0.58060 * v);
            int b = round(y + 2.03211 * u);

            img->data[i][j].red   = fmin(fmax(r, 0), 255);
            img->data[i][j].green = fmin(fmax(g, 0), 255);
            img->data[i][j].blue  = fmin(fmax(b, 0), 255);
        }
    }
    
    // Free allocated memory
    free(cdf);
    free(hist_eq);
    free(histogramme);
    
    for (int i = 0; i < height; i++) {
        free(Y[i]);
        free(U[i]);
        free(V[i]);
    }
    free(Y);
    free(U);
    free(V);
}