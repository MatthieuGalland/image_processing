#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "menu.h"
#include "utils24.h"
#ifdef _WIN32
    #include <windows.h>
#endif

#include <stdio.h>
#include <conio.h>
#include <windows.h>

int main(void) {
    #ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    #endif

    t_bmp24 image24;
    t_bmp8 image8;
    int selectedDepth = 0;

    int a = 0;
    while (a == 0) {
        a = menu(&image24,&image8,&selectedDepth);
    }

    //image8 = bmp8_loadImage("../sources/lena_color.bmp");
/*
    while (1) {
        menu(image24, image8);
    }

    image24 = bmp24_loadImage("../sources/lena_color.bmp");
    bmp24_print_preview(image24);
    bmp24_saveImage(image24, "../sources/lena_color_save.bmp");


    /*
    t_bmp8 image = bmp8_loadImage("D:/DEV/C/projetP1S2/sources/lena_gray.bmp");



    bmp_printInfo(&image);

    float **filter = (float**)malloc(sizeof(float*)*3);
    for (int i = 0; i < 3; ++i) {
        filter[i] = (float*)malloc(sizeof(float)*3);
        for (int j = 0; j < 3; ++j) {
            filter[i][j] = 0.1f;
            printf("%d;%d = %f ",i,j,filter[i][j]);
        }
    }/*

    float **filter2 = (float**)malloc(sizeof(float*)*3);
    for (int i = 0; i < 3; ++i) {
        filter2[i] = (float*)malloc(sizeof(float)*3);
        for (int j = 0; j < 3; ++j) {
            filter2[i][j] = -1;
        }
    }
    filter2[1][1] = 8;

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            printf("%.1f ",filter2[i][j]);
        }
        printf("\n");
    }



    printf("\nFiltre genere!\n");

    bmp8_applyFilter(&image,filter2,3);

    bmp8_saveImage("D:/DEV/C/projetP1S2/sources/save.bmp",&image);

    //bmp_8_free(&image);*/

    //bmp24_applyFilter(image24,filter2,3);

    //bmp24_negative(image24);

    //bmp24_applyFilter(image24,filter,3);
    //bmp24_saveImage(image24, "../sources/save_color.bmp");

    return 0;
}
