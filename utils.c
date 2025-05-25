// utils.c
//
// Ce fichier contient les fonctions de base pour la manipulation d'images BMP 8 bits (niveaux de gris).
// Il fournit les outils essentiels pour le chargement, la sauvegarde et le traitement des images,
// ainsi que diverses fonctions pour appliquer des filtres et des transformations.
//
// Fonctionnalités:
// - Chargement et sauvegarde d'images BMP 8 bits
// - Gestion de la mémoire (allocation et libération)
// - Fonctions de filtrage: négatif, luminosité, seuil
// - Application de filtres de convolution (matrices de kernel)
//
// Auteur: Thomas Darbo et Matthieu Galland
// Date: Main 2025

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

t_bmp8* bmp8_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("[ERREUR] Impossible d'ouvrir le fichier : %s\n", filename);
        return NULL;
    }

    unsigned char *header = (unsigned char*)calloc(54, sizeof(unsigned char));
    if (!header) {
        printf("[ERREUR] Erreur d'allocation pour l'en-tête.\n");
        fclose(file);
        return NULL;
    }

    if (fread(header, sizeof(unsigned char), 54, file) != 54) {
        printf("[ERREUR] Impossible de lire l'en-tête BMP.\n");
        free(header);
        fclose(file);
        return NULL;
    }

    unsigned int width      = *(unsigned int*)&header[18];
    unsigned int height     = *(unsigned int*)&header[22];
    unsigned int dataSize   = *(unsigned int*)&header[34];
    unsigned short colorDepth = *(unsigned short*)&header[28];
    unsigned int offset     = *(unsigned int*)&header[10];

    if (colorDepth != 8) {
        printf("[ERREUR] Format BMP non supporté (%u bits, attendu 8 bits).\n", colorDepth);
        free(header);
        fclose(file);
        return NULL;
    }

    t_bmp8 *image = (t_bmp8*)malloc(sizeof(t_bmp8));
    if (!image) {
        printf("[ERREUR] Échec d'allocation mémoire pour l'image.\n");
        free(header);
        fclose(file);
        return NULL;
    }

    image->width      = width;
    image->height     = height;
    image->dataSize   = dataSize;
    image->colorDepth = colorDepth;

    // Copiez exactement 54 octets, plutôt que sizeof(header)
    memcpy(image->header, header, 54);

    if (fread(image->colorTable, sizeof(unsigned char), 1024, file) != 1024) {
        printf("[ERREUR] Échec de lecture de la table des couleurs.\n");
        free(header);
        free(image);
        fclose(file);
        return NULL;
    }

    image->data = (unsigned char*)malloc(dataSize);
    if (!image->data) {
        printf("[ERREUR] Échec d'allocation mémoire pour les pixels.\n");
        free(header);
        free(image);
        fclose(file);
        return NULL;
    }

    fseek(file, offset, SEEK_SET);
    if (fread(image->data, sizeof(unsigned char), dataSize, file) != dataSize) {
        printf("[ERREUR] Échec de lecture des pixels.\n");
        free(header);
        free(image->data);
        free(image);
        fclose(file);
        return NULL;
    }

    free(header);
    fclose(file);

    return image;
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
}

void bmp8_threshold(int threshold, t_bmp8 *image) {

    for (int i = 0; i < 1024; ++i) {
        const int value = image->colorTable[i];
        int final;
        if (value >= threshold) final = 255;
        else final = 0;

        image->colorTable[i] = final;
    }
}

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



