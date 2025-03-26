//
// Created by thoma on 15.03.2025.
//

#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>

typedef struct {
    unsigned char header[54];
    unsigned char colorTable[1024];
    unsigned char *data;

    unsigned int width;
    unsigned int height;
    unsigned int colorDepth;
    unsigned int dataSize;
} t_bmp8;

t_bmp8 bmp8_loadImage(const char * filename);

void bmp8_saveImage(const char * filename, t_bmp8 * img);

void bmp_8_free(t_bmp8 *image);

void bmp_printInfo(t_bmp8 * image);

void bmp8_negative(t_bmp8 * image);

void bmp8_brightness(int modifier, t_bmp8 *image);

void bmp8_threshold(int threshold, t_bmp8 *image);

void bmp8_applyFilter(t_bmp8 * image, float ** kernel, int kernelSize);

#endif //UTILS_H
