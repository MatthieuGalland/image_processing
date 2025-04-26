//
// Created by sandw on 09/04/2025.
//
#include "utils24.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


t_pixel ** bmp24_allocateDataPixels (int width, int height) {
    t_pixel** data = (t_pixel**)malloc(sizeof(t_pixel*) * height);
    for (int i=0;i<height;i++) {
        data[i] = (t_pixel*)malloc(sizeof(t_pixel*)*width);
    }
    return data;
}


void bmp24_freeDataPixels (t_pixel ** pixels, int height) {
    for (int i=0;i<height;i++) {
        free(pixels[i]);
    }
    free(pixels);
}

t_bmp24 * bmp24_allocate (int width, int height, int colorDepth) {
    t_bmp24 * image = malloc(sizeof(t_bmp24));
    image->width = width;
    image->height = height;
    image->data = bmp24_allocateDataPixels(width,height);
    return image;
}

void bmp24_free (t_bmp24 * img) {
    int width = img->width;
    int height = img->height;
    bmp24_freeDataPixels(img->data,img->height);
    free(img);
}


void file_rawRead(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    fseek(file, position, SEEK_SET);
    fread(buffer, size, n, file);
}


void file_rawWrite (uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file) {
    fseek(file, position, SEEK_SET);
    fwrite(buffer, size, n, file);
}

void bmp24_readPixelValue(t_bmp24 *image, int x, int y, FILE *file) {
    uint32_t offset = image->header.offset + (image->height - 1 - y) * (image->width * sizeof(t_pixel)) + x * sizeof(t_pixel);
    t_pixel pixel;
    file_rawRead(offset, &pixel, sizeof(t_pixel), 1, file);

    image->data[y][x].red = pixel.blue;
    image->data[y][x].green = pixel.green;
    image->data[y][x].blue = pixel.red;
}

void bmp24_readPixelData(t_bmp24 *image, FILE *file) {
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            bmp24_readPixelValue(image, x, y, file);
        }
    }
}


void bmp24_writePixelValue(t_bmp24 *image, int x, int y, FILE *file) {
    uint32_t offset = image->header.offset + (image->height - 1 - y) * (image->width * sizeof(t_pixel)) + x * sizeof(t_pixel);
    t_pixel pixel;

    // Convertir RGB en BGR pour l'ecriture
    pixel.red = image->data[y][x].blue;
    pixel.green = image->data[y][x].green;
    pixel.blue = image->data[y][x].red;

    file_rawWrite(offset, &pixel, sizeof(t_pixel), 1, file);
}


void bmp24_writePixelData(t_bmp24 *image, FILE *file) {
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            bmp24_writePixelValue(image, x, y, file);
        }
    }
}


t_bmp24 *bmp24_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("[ERREUR] Impossible d'ouvrir le fichier : %s\n", filename);
        return NULL;
    }

    // Allouer de la memoire pour l'image
    int32_t width, height,depth;
    file_rawRead(BITMAP_WIDTH, &width, sizeof(int32_t),1,file);
    file_rawRead(BITMAP_HEIGHT, &height, sizeof(int32_t),1,file);
    file_rawRead(BITMAP_DEPTH, &depth, sizeof(int32_t),1,file);

    t_bmp24* img = bmp24_allocate(width, height, depth);
    if (!img) {
        printf("[ERREUR] Allocation memoire echouee.\n");
        fclose(file);
        return NULL;
    }

    // Lecture du header
    t_bmp_header header;
    file_rawRead(BITMAP_MAGIC, &header,sizeof(t_bmp_header),1,file);
    img->header = header;

    // Debug
    printf("[DEBUG] type = %X\n", img->header.type);
    printf("[DEBUG] size = %u\n", img->header.size);
    printf("[DEBUG] offset = %u\n", img->header.offset);

    // Verification du type BMP
    if (img->header.type != BMP_TYPE) {
        printf("[ERREUR] Fichier non reconnu comme BMP.\n");
        free(img);
        fclose(file);
        return NULL;
    }

    // Lire l'en-tête d'information
    file_rawRead(HEADER_SIZE, &img->header_info, sizeof(t_bmp_info), 1, file);

    // Initialiser les champs principaux
    img->width = img->header_info.width;
    img->height = abs(img->header_info.height);
    img->colorDepth = img->header_info.bits;

    // Verification de la profondeur (24 bits attendus)
    if (img->colorDepth != DEFAULT_DEPTH) {
        printf("[ERREUR] Format BMP non supporte (prof. couleur attendue: 24).\n");
        free(img);
        fclose(file);
        return NULL;
    }

    // Allouer la memoire pour les pixels
    img->data = bmp24_allocateDataPixels(img->width, img->height);
    if (!img->data) {
        printf("[ERREUR] Allocation memoire pour les pixels echouee.\n");
        free(img);
        fclose(file);
        return NULL;
    }

    // Lire les donnees de pixels
    bmp24_readPixelData(img, file);

    fclose(file);
    return img;
}

void bmp24_saveImage(t_bmp24 *img, const char *filename) {
    if (!img || !img->data) {
        printf("[ERREUR] Image invalide ou donnees manquantes.\n");
        return;
    }

    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("[ERREUR] Impossible d'ouvrir le fichier pour ecriture : %s\n", filename);
        return;
    }

    printf("[DEBUG] Fichier %s ouvert avec succès.\n", filename);

    // Chaque ligne a un paddind de 4 octets
    int rowPadding = (4 - (img->width * 3) % 4) % 4;
    int rowSize = img->width * 3 + rowPadding;
    int pixelDataSize = rowSize * img->height;


    img->header.type = BMP_TYPE;
    img->header.size = HEADER_SIZE + INFO_SIZE + pixelDataSize;
    img->header.reserved1 = 0;
    img->header.reserved2 = 0;
    img->header.offset = HEADER_SIZE + INFO_SIZE;

    img->header_info.size = INFO_SIZE;
    img->header_info.width = img->width;
    img->header_info.height = img->height;
    img->header_info.planes = 1;
    img->header_info.bits = DEFAULT_DEPTH;  // 24 bits
    img->header_info.compression = 0;
    img->header_info.imagesize = pixelDataSize;
    img->header_info.xresolution = 0;
    img->header_info.yresolution = 0;
    img->header_info.ncolors = 0;
    img->header_info.importantcolors = 0;

    printf("[DEBUG] En-têtes prepares - Taille fichier: %u, Offset donnees: %u\n",
           img->header.size, img->header.offset);

    // En-tête de fichier BMP
    fwrite(&img->header.type, sizeof(uint16_t), 1, file);
    fwrite(&img->header.size, sizeof(uint32_t), 1, file);
    fwrite(&img->header.reserved1, sizeof(uint16_t), 1, file);
    fwrite(&img->header.reserved2, sizeof(uint16_t), 1, file);
    fwrite(&img->header.offset, sizeof(uint32_t), 1, file);

    // En-tête d'information BMP
    fwrite(&img->header_info.size, sizeof(uint32_t), 1, file);
    fwrite(&img->header_info.width, sizeof(int32_t), 1, file);
    fwrite(&img->header_info.height, sizeof(int32_t), 1, file);
    fwrite(&img->header_info.planes, sizeof(uint16_t), 1, file);
    fwrite(&img->header_info.bits, sizeof(uint16_t), 1, file);
    fwrite(&img->header_info.compression, sizeof(uint32_t), 1, file);
    fwrite(&img->header_info.imagesize, sizeof(uint32_t), 1, file);
    fwrite(&img->header_info.xresolution, sizeof(int32_t), 1, file);
    fwrite(&img->header_info.yresolution, sizeof(int32_t), 1, file);
    fwrite(&img->header_info.ncolors, sizeof(uint32_t), 1, file);
    fwrite(&img->header_info.importantcolors, sizeof(uint32_t), 1, file);

    printf("[DEBUG] En-têtes BMP ecrits avec succès.\n");

    for (int y = img->height - 1; y >= 0; y--) {
        for (int x = 0; x < img->width; x++) {
            // Format BGR pour les fichiers BMP
            unsigned char bgr[3];
            bgr[0] = img->data[y][x].blue;
            bgr[1] = img->data[y][x].green;
            bgr[2] = img->data[y][x].red;
            fwrite(bgr, sizeof(unsigned char), 3, file);
        }

        for (int p = 0; p < rowPadding; p++) {
            fputc(0x00, file);
        }
    }

    printf("[DEBUG] Donnees des pixels ecrites avec succès: %d lignes, %d colonnes, %d octets de padding\n",
           img->height, img->width, rowPadding);

    fclose(file);
    printf("[DEBUG] Fichier %s ferme avec succès. Taille totale: %u octets\n",
           filename, img->header.size);
}


//TESTS
void bmp24_printInfo(t_bmp24 *img) {
    if (!img) {
        printf("[ERREUR] Image NULL\n");
        return;
    }

    printf("=== Infos sur l'image BMP24 ===\n");
    printf("Largeur (width)        : %d px\n", img->width);
    printf("Hauteur (height)       : %d px\n", img->height);
    printf("Profondeur couleur     : %d bits\n", img->colorDepth);
    printf("Taille du fichier      : %u octets\n", img->header.size);
    printf("Offset debut des pixels: %u octets\n", img->header.offset);

    printf("--- Pixels importants ---\n");

    // Premier pixel (en haut a gauche)
    printf("Pixel (0,0) [R,G,B] : (%u, %u, %u)\n",
           img->data[0][0].red, img->data[0][0].green, img->data[0][0].blue);

    // Pixel du milieu
    int midX = img->width / 2;
    int midY = img->height / 2;
    printf("Pixel centre (%d,%d) [R,G,B] : (%u, %u, %u)\n",
           midX, midY,
           img->data[midY][midX].red, img->data[midY][midX].green, img->data[midY][midX].blue);

    printf("==============================\n");
}

