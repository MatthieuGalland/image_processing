//
// Created by sandw on 09/04/2025.
//
#include "utils24.h"

#include <stdlib.h>


t_pixel ** bmp24_allocateDataPixels (int width, int height) {
    t_pixel** data = (t_pixel**)malloc(sizeof(t_pixel) * height);
    for (int i=0;i<height;i++) {
        data[i] = (t_pixel*)malloc(sizeof(t_pixel)*width);
    }
    return data;
}


void bmp24_freeDataPixels (t_pixel ** pixels, int height) {
    for (int i=0;i<height;i++) {
        free(pixels[i]);
    }
    free(pixels);
}
