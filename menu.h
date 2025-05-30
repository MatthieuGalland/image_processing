//
// Created by Thomas on 02/05/2025.
//

#ifndef MENU_H
#define MENU_H
#include "utils.h"
#include "utils24.h"

int menu(t_bmp24 **image24, t_bmp8 **image8, int *selectedDepth);

int loadImage(t_bmp24 **image24, t_bmp8 **image8);

int detectImageFormat(const char *filename);

int image_menu(t_bmp8 *image8, t_bmp24 *image24, int colorDepth);

void image_save(t_bmp8 *image8, t_bmp24 *image24, int colorDepth);

int image8_filter_menu(t_bmp8 *image8);
int image24_filter_menu(t_bmp24 *image24);

int input_value(int min, int max, char* message);

int print_menu(char *title, char *subtitle, char *message, ...);

float** getKernelPtr(const float arr[3][3]);

#endif //MENU_H
