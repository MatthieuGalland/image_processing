//
// Created by thoma on 15.03.2025.
//

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


t_bmp8 bmp8_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("[ERREUR] Impossible d'ouvrir le fichier : %s\n", filename);
        exit(EXIT_FAILURE);
    }

    unsigned char header[54];
    if (fread(header, sizeof(unsigned char), 54, file) != 54) {
        printf("[ERREUR] Impossible de lire l'en-tête BMP.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    const unsigned int width = *(unsigned int*)&header[18];
    const unsigned int height = *(unsigned int*)&header[22];
    const unsigned int dataSize = *(unsigned int*)&header[34];
    const unsigned int colorDepth = *(unsigned short*)&header[28]; // short (2 octets)
    const unsigned int offset = *(unsigned int*)&header[10];

    if (colorDepth != 8) {
        printf("[ERREUR] Format BMP non supporte (%u bits, attendu 8 bits).\n", colorDepth);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    t_bmp8 *image = (t_bmp8*)malloc(sizeof(t_bmp8));
    if (!image) {
        printf("[ERREUR] echec d'allocation memoire pour l'image.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    image->width = width;
    image->height = height;
    image->dataSize = dataSize;
    image->colorDepth = colorDepth;

    memcpy(image->header, header, sizeof(header));

    if (fread(image->colorTable, sizeof(unsigned char), 1024, file) != 1024) {
        printf("[ERREUR] echec de lecture de la table des couleurs.\n");
        free(image);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    image->data = (unsigned char*)malloc(dataSize);
    if (!image->data) {
        printf("[ERREUR] echec d'allocation memoire pour les pixels.\n");
        free(image);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fseek(file, offset, SEEK_SET);
    if (fread(image->data, sizeof(unsigned char), dataSize, file) != dataSize) {
        printf("[ERREUR] echec de lecture des pixels.\n");
        free(image->data);
        free(image);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fclose(file);
    return *image;
}


void bmp8_saveImage(const char *filename, t_bmp8 *img) {
    FILE *file = fopen(filename, "wb");

    if (file == NULL) {
        printf("[ERREUR] Impossible de creer le fichier %s\n", filename);
        exit(EXIT_FAILURE);
    }

    unsigned int dataOffset = *(unsigned int*)&img->header[10];

    if (fwrite(img->header, sizeof(unsigned char), sizeof(img->header), file) != sizeof(img->header)) {
        printf("[ERREUR] echec d'ecriture du header\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    if (fwrite(img->colorTable, sizeof(unsigned char), 1024, file) != 1024) {
        printf("[ERREUR] echec d'ecriture de la table de couleurs\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fseek(file, dataOffset, SEEK_SET);

    if (fwrite(img->data, sizeof(unsigned char), img->dataSize, file) != img->dataSize) {
        printf("[ERREUR] echec d'ecriture des pixels\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fclose(file);
}


void bmp_8_free(t_bmp8 *image) {

    if (image == NULL) {
        printf("[ERREUR] Ce pointer est invalide");
        return;
    }

    if (image->data != NULL) {
        free(image->data);
        image->data = NULL;
    }

    free(image->header);
    free(image->colorTable);

    free(image);

    printf("[INFORMATION] Memoire liberee");
}

void bmp_printInfo(t_bmp8 *image) {
    printf("Image Info : \n");
    printf("    Width : %d\n",image->width);
    printf("    Height : %d\n",image->height);
    printf("    ColorDepth : %d\n",image->colorDepth);
    printf("    DataSize : %d\n",image->dataSize);
}

void bmp8_negative(t_bmp8 *image) {
    for (int i = 0; i < 1024; ++i) {
        const int value = image->colorTable[i];
        const int final = 255 - value;
        image->colorTable[i] = final;
    }
    printf("[INFORMATION] Negatif applique !");
}

void bmp8_brightness(int modifier, t_bmp8 *image) {
    for (int i = 0; i < 1024; ++i) {
        const int value = image->colorTable[i];
        int final = value + modifier;

        if (final > 255) {
            final = 255;
        }

        image->colorTable[i] = final;
    }
    printf("[INFORMATION] Brightness de %d applique !",modifier);
}

void bmp8_threshold(int threshold, t_bmp8 *image) {

    for (int i = 0; i < 1024; ++i) {
        const int value = image->colorTable[i];
        int final;
        if (value >= threshold) final = 255;
        else final = 0;

        image->colorTable[i] = final;
    }
    printf("[INFORMATION] Threshold de %d applique !",threshold);
}

#include <stdio.h>
#include <stdlib.h>

void bmp8_applyFilter(t_bmp8 *image, float **kernel, int kernelSize) {
    if (!image || !image->data || !kernel || kernelSize <= 0) {
        printf("Erreur : parametres invalides\n");
        return;
    }

    int halfKernel = kernelSize / 2;

    int **pixels = (int**)malloc(image->height * sizeof(int*));
    if (!pixels) {
        printf("Erreur d'allocation memoire\n");
        return;
    }

    for (int y = 0; y < image->height; ++y) {
        pixels[y] = (int*)malloc(image->width * sizeof(int));
        if (!pixels[y]) {
            printf("Erreur d'allocation memoire\n");
            for (int k = 0; k < y; ++k) {
                free(pixels[k]);
            }
            free(pixels);
            return;
        }
    }

    for (int y = 0; y < image->height; ++y) {
        for (int x = 0; x < image->width; ++x) {
            pixels[y][x] = image->data[y * image->width + x];
        }
    }

    printf("Fin mapping pixels\n");

    int *filteredData = (int*)malloc(image->width * image->height * sizeof(int));
    if (!filteredData) {
        printf("Erreur d'allocation memoire\n");
        for (int y = 0; y < image->height; ++y) {
            free(pixels[y]);
        }
        free(pixels);
        return;
    }

    for (int y = 0; y < image->height; ++y) {
        for (int x = 0; x < image->width; ++x) {
            float sum = 0.0;

            for (int k = 0; k < kernelSize; ++k) {
                for (int l = 0; l < kernelSize; ++l) {
                    int pix_Y = y + k - halfKernel;
                    int pix_X = x + l - halfKernel;

                    if (pix_X >= 0 && pix_X < image->width && pix_Y >= 0 && pix_Y < image->height) {
                        sum += pixels[pix_Y][pix_X] * kernel[k][l];
                    }
                }
            }

            filteredData[y * image->width + x] = (int)sum;
        }
    }

    for (int i = 0; i < image->width * image->height; ++i) {
        image->data[i] = filteredData[i];
    }

    printf("Filtre applique !\n");

    for (int y = 0; y < image->height; ++y) {
        free(pixels[y]);
    }
    free(pixels);
    free(filteredData);
}



