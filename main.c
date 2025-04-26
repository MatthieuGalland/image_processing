#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "utils24.h"

int main(void) {

    t_bmp24 *image24 = bmp24_loadImage("C:/Users/sandw/CLionProjects/image_processing2/sources/lena_color.bmp");

    // bmp24_printInfo(image24);
    bmp24_saveImage(image24, "C:/Users/sandw/CLionProjects/image_processing2/sources/save_color.bmp");
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
    }

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



    return 0;
}
