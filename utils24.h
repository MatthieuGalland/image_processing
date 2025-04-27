//
// Created by sandw on 09/04/2025.
//

#ifndef UTILS24_H
#define UTILS24_H
#include <stdint.h>
#include <stdio.h>

// Constantes pour les offsets des champs de l'en-tête BMP
#define BITMAP_MAGIC 0x00 // offset 0
#define BITMAP_SIZE 0x02 // offset 2
#define BITMAP_OFFSET 0x0A // offset 10
#define BITMAP_WIDTH 0x12 // offset 18
#define BITMAP_HEIGHT 0x16 // offset 22
#define BITMAP_DEPTH 0x1C // offset 28
#define BITMAP_SIZE_RAW 0x22 // offset 34
// Constante pour le type de fichier BMP
#define BMP_TYPE 0x4D42 // 'BM' en hexadécimal
// Constantes pour les tailles des champs de l'en-tête BMP
#define HEADER_SIZE 0x0E // 14 octets
#define INFO_SIZE 0x28 // 40 octets
// Constantes pour les valeurs de profondeur de couleur
#define DEFAULT_DEPTH 0x18 // 24



typedef struct {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} t_bmp_header;

typedef struct{
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bits;
    uint32_t compression;
    uint32_t imagesize;
    int32_t xresolution;
    int32_t yresolution;
    uint32_t ncolors;
    uint32_t importantcolors;
} t_bmp_info;

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} t_pixel;


typedef struct {
    t_bmp_header header;
    t_bmp_info header_info;
    int width;
    int height;
    int colorDepth;
    t_pixel **data;
} t_bmp24;


// Prototypes des fonctions
t_pixel ** bmp24_allocateDataPixels (int width, int height);
void bmp24_freeDataPixels (t_pixel ** pixels, int height);
t_bmp24 * bmp24_allocate (int width, int height, int colorDepth);
void bmp24_free (t_bmp24 * img);


void file_rawRead(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file);


void file_rawWrite(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file);


void bmp24_readPixelValue(t_bmp24 *image, int x, int y, FILE *file);


void bmp24_readPixelData(t_bmp24 *image, FILE *file);


void bmp24_writePixelValue(t_bmp24 *image, int x, int y, FILE *file);

void bmp24_writePixelData(t_bmp24 *image, FILE *file);

t_bmp24 *bmp24_loadImage(const char *filename);

void bmp24_saveImage(t_bmp24 *img, const char *filename);

void bmp24_printInfo(t_bmp24 *img);

void bmp24_applyFilter(t_bmp24 *image, float **kernel, int kernelSize);

void bmp24_brightness (t_bmp24 * img, int value);
void bmp24_negative(t_bmp24* img);
void bmp24_grayscale (t_bmp24 * img);



#endif //UTILS24_H
