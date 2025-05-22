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


/**
* @brief Positionne le curseur de fichier a la position position dans le fichier file,
* puis lit n elements de taille size dans buffer.
* @param position La position a partir de laquelle il faut lire dans file.
* @param buffer Le buffer pour stocker les donnees lues.
* @param size La taille de chaque element a lire.
* @param n Le nombre d'elements a lire.
* @param file Le descripteur de fichier dans lequel il faut lire.
* @return void
*/
void file_rawRead(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    fseek(file, position, SEEK_SET);
    fread(buffer, size, n, file);
}

/**
* @brief Positionne le curseur de fichier a la position position dans le fichier file,
* puis ecrit n elements de taille size depuis le buffer.
* @param position La position a partir de laquelle il faut ecrire dans file.
* @param buffer Le buffer contenant les elements a ecrire.
* @param size La taille de chaque element a ecrire.
* @param n Le nombre d'elements a ecrire.
* @param file Le descripteur de fichier dans lequel il faut ecrire.
* @return void
*/
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


t_bmp24* bmp24_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    t_bmp24 *img = (t_bmp24 *)malloc(sizeof(t_bmp24));

    if (!file) {
        printf("[ERREUR] Impossible d'ouvrir le fichier : %s\n", filename);
        free(img);
        return NULL;
    }

    // Allouer de la memoire pour l'image
    if (!img) {
        printf("[ERREUR] Allocation memoire echouee.\n");
        fclose(file);
        return NULL;
    }

    // Lire l'en-tête de fichier
// Lire le header brut
    unsigned char raw_header[14];
    file_rawRead(0, raw_header, 1, 14, file);




    img->header.type = *(uint16_t*)&raw_header[0];
    img->header.size = *(uint32_t*)&raw_header[2];
    img->header.reserved1 = *(uint16_t*)&raw_header[6];
    img->header.reserved2 = *(uint16_t*)&raw_header[8];
    img->header.offset = *(uint32_t*)&raw_header[10];

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

    printf("[DEBUG] Fichier créer/ouvert avec succès.\n");

    // Chaque ligne c 4 octet
    int rowPadding = (4 - (img->width * 3) % 4) % 4;
    int rowSize = img->width * 3 + rowPadding;
    int pixelDataSize = rowSize * img->height;

// Vérification de la validité
    /*
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
    */

    printf("[DEBUG] En-têtes prepares - Taille fichier: %u, Offset donnees: %u\n",
           img->header.size, img->header.offset);

    // Le header est buggé ducoup j'ai tout refait prcsq azé
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

        // Pour finir les lignes et conserver la taille de 4 octets
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

// Filtres
void bmp24_applyFilter(t_bmp24 *image, float **kernel, int kernelSize) {
    if (!image || !kernel || kernelSize <= 0) return;
    int width = image->width, height = image->height;
    int offset = kernelSize / 2;

    // Allocation temporaire et copie des pixels originaux
    t_pixel **tmp = bmp24_allocateDataPixels(width,height);
    if (!tmp) return;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            tmp[y][x] = image->data[y][x];
        }
    }
    //printf("Fin mapping pixels\n");
    // Application du filtre convolution sur chaque canal
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double sumR = 0.0, sumG = 0.0, sumB = 0.0;
            for (int ky = 0; ky < kernelSize; ky++) {
                for (int kx = 0; kx < kernelSize; kx++) {
                    int xx = x + kx - offset;
                    int yy = y + ky - offset;
                    if (xx >= 0 && xx < width && yy >= 0 && yy < height) {
                        t_pixel *p = &tmp[yy][xx];
                        float kval = kernel[ky][kx];
                        sumR += p->red * kval;
                        sumG += p->green * kval;
                        sumB += p->blue * kval;
                    }
                }
            }

            // Clamp et assignation dans l'image originale
            int vR = (int)sumR; if (vR < 0) vR = 0; if (vR > 255) vR = 255;
            int vG = (int)sumG; if (vG < 0) vG = 0; if (vG > 255) vG = 255;
            int vB = (int)sumB; if (vB < 0) vB = 0; if (vB > 255) vB = 255;
            image->data[y][x].red = (unsigned char)vR;
            image->data[y][x].green = (unsigned char)vG;
            image->data[y][x].blue = (unsigned char)vB;
        }
    }
    printf("Filtre applique !\n");
    bmp24_freeDataPixels(tmp,height);
}

void bmp24_negative(t_bmp24 *img) {
    int width = img->width;
    int height = img->height;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            img->data[y][x].red = 255 - img->data[y][x].red;
            img->data[y][x].green = 255 - img->data[y][x].green;
            img->data[y][x].blue = 255 - img->data[y][x].blue;
        }
    }
}

void bmp24_grayscale(t_bmp24 * img) {
    int width = img->width;
    int height = img->height;
    int moyenne;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            moyenne = (img->data[y][x].red + img->data[y][x].blue + img->data[y][x].green)/3;
            img->data[y][x].red = moyenne;
            img->data[y][x].blue = moyenne;
            img->data[y][x].green = moyenne;
        }
    }
}

void bmp24_brightness(t_bmp24 * img, int value) {
    int width = img->width;
    int height = img->height;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // On s'assure que la valeur ne dépasse pas 255
            if (img->data[y][x].red+value<=255)
                img->data[y][x].red+=value;
            else
                img->data[y][x].red = 255;

            if (img->data[y][x].green+value<=255)
                img->data[y][x].green+=value;
            else
                img->data[y][x].green = 255;

            if (img->data[y][x].blue+value<=255)
                img->data[y][x].blue+=value;
            else
                img->data[y][x].blue = 255;

        }
    }
}

void bmp24_print_preview(t_bmp24 *image) {
    if (!image || !image->data) {
        printf("Invalid image data\n");
        return;
    }

    // Scale factor to fit image in console
    // Assuming console is roughly 80x25 characters
    int scale_x = image->width / 30 + 1;
    int scale_y = image->height / 10 + 1;

    // Ensure scale factors are at least 1
    if (scale_x < 1) scale_x = 1;
    if (scale_y < 1) scale_y = 1;

    // ASCII characters for different brightness levels (from dark to bright)
    const char *ascii_chars = "#";
    int ascii_len = strlen(ascii_chars);

    printf("Image preview (%dx%d):\n", image->width, image->height);

    // Iterate through the scaled image
    for (int y = 0; y < image->height; y += scale_y) {
        for (int x = 0; x < image->width; x += scale_x) {
            // Calculate average color for this block
            int r_sum = 0, g_sum = 0, b_sum = 0;
            int count = 0;

            for (int dy = 0; dy < scale_y && y + dy < image->height; dy++) {
                for (int dx = 0; dx < scale_x && x + dx < image->width; dx++) {
                    r_sum += image->data[y + dy][x + dx].red;
                    g_sum += image->data[y + dy][x + dx].green;
                    b_sum += image->data[y + dy][x + dx].blue;
                    count++;
                }
            }

            if (count > 0) {
                int r_avg = r_sum / count;
                int g_avg = g_sum / count;
                int b_avg = b_sum / count;

                // Calculate brightness (simple average of RGB)
                int brightness = (r_avg + g_avg + b_avg) / 3;

                // Map brightness to ASCII character
                int char_index = brightness * ascii_len / 256;
                if (char_index >= ascii_len) char_index = ascii_len - 1;

                // Use ANSI escape sequences for color
                printf("\033[38;2;%d;%d;%dm%c\033[0m",
                       r_avg, g_avg, b_avg,
                       ascii_chars[char_index]);
            }
        }
        printf("\n"); // New line after each row
    }

    // Reset colors at the end
    printf("\033[0m\n");
}


