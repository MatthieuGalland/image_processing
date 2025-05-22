//
// Created by Thomas on 02/05/2025.
//

#include "menu.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "utils24.h"
#include "color.h"



int menu(t_bmp24 *image24, t_bmp8 *image8, int *selectedDepth) {

    const bool i8 = true;//*selectedDepth == 8;
    const bool i24 = *selectedDepth == 24;

    if (i8 || i24) {
        if (i8) {
            print_menu("Menu de gestion Image 8 bits", "[!] Image loaded", "Appliquer un filtre", "Lire les modifications de l'image", "Sauvegarder l'image", "Decharger l'image", NULL);
        }
    } else {

        printf("\n =========================\n ### Menu principal ###\n");
        printf("Quelle operation voulez-vous effectuer ?\n");
        printf("1. Charger une image\n");
        printf("2. Quitter\n");
        printf("(pour avoir acces a plus d'options, chargez une image)\n\n");
    }


    int choix;
    scanf("%d", &choix);

    if (i8 || i24) {
        if (i8) {
            image_menu(image8, image24, 8);
        }
    } else {
        switch (choix) {
            case 1:
                printf("Chargement d'une image...\n");
                *selectedDepth = loadImage(image24, image8);
                break;
            case 2:
                return 1;
            default:
                printf("Choix invalide. Veuillez reessayer.\n");
                break;
        }
    }
    return 0;
}

int loadImage(t_bmp24 *image24, t_bmp8 *image8) {

    system("cls");
    printf("\n=========================\nOutils de chargement d'images : \n");
    printf("Veuillez choisir le chemin de l'image a charger.\n");
    printf("Appuyez sur 'd' pour utiliser le chemin par defaut (../sources/load.bmp)\n");
    printf("CHEMIN : ");
    char chemin[256];
    scanf(" %s", chemin);
    if (strcmp(chemin, "d") == 0) {
        strcpy(chemin, "../sources/lena_gray.bmp");
    }

    int cd = 0;
    cd = detectImageFormat(chemin);
    switch (cd) {
        case 8:
            printf("[INFO] Chargement de l'image BMP 8 bits...\n");
            image8 = bmp8_loadImage(chemin);
            break;
        case 24:
            printf("[INFO] Chargement de l'image BMP 24 bits...\n");
            image24 = bmp24_loadImage(chemin);
            break;

        default:
            printf("[ERREUR] Format d'image non pris en charge.\n");
            break;
    }
    return cd;
}

int detectImageFormat(const char *filename) {

    printf("[INFO] Detection du format de l'image...\n");

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("[ERREUR] Impossible d'ouvrir le fichier : %s\n", filename);
        return 0;
    }

    unsigned char *header = calloc(54, sizeof(unsigned char));
    fread(header,sizeof(unsigned char),54,file);
    if (header == NULL) {
        printf("[ERREUR] Impossible de lire l'en-tête BMP.\n");
        free(header);
        fclose(file);
        return 0;
    }

    const unsigned int colorDepth = header[28];
    free(header);

    if (colorDepth == 8) {
        printf("[INFO] Format BMP 8 bits detecte.\n");
        return 8;
    }
    if (colorDepth == 24) {
        printf("[INFO] Format BMP 24 bits detecte.\n");
        return 24;
    }

    return 0;
}

int image_menu(t_bmp8 *image8, t_bmp24 *image24, const int colorDepth) {
    system("cls");
    printf("\n=========================\n### Menu image %d bits ###\n",colorDepth);
    printf("    | 1. Appliquer un filtre\n");
    printf("    | 2. Lire les modifications de l'image\n");
    printf("    | 3. Sauvegarder l'image\n");
    printf("    | 4. Décharger l'image\n");

    int choix;
    scanf("%d", &choix);

    switch (choix) {
        case 1: image8_filter_menu(image8); break;
    }

    return 0;
}

int image8_filter_menu(t_bmp8 *image8) {

    printf("\n=========================\n### Menu filtre image 8 bits ###\n");
    printf("    | 1. Filtre Negatif\n");
    printf("    | 2. Filtre Luminosite\n");
    printf("    | 3. Filtre Threshold\n");

    int choix;
    scanf("%d", &choix);

    switch (choix) {
        case 1: {
            bmp8_negative(image8);
            printf("[INFO] Filtre NEGATIF applique !\n");
            break;
        }
        case 2: {
            const int a = input_value(0,255,"LUMINOSITE");
            bmp8_brightness(a, image8);
            printf("[INFO] Filtre LUMINOSITE de %d applique !\n", a);
            break;
        }
        case 3: {
            const int a = input_value(0,255,"THRESHOLD");
            bmp8_brightness(a, image8);
            printf("[INFO] Filtre THRESHOLD de %d applique !\n", a);
            break;
        }
    }
    return 0;
}

int input_value(int min, int max, char *message) {

    printf("\n\nVeuillez inserer une valeur pour %s :\n    | Valeurs acceptees : %d - %d\n", message, min, max);
    int value;
    scanf("%d", &value);
    while (value < min || value > max) {
        printf("\nValeur invalide. Veuillez entrer une valeur entre %d et %d : ", min, max);
        scanf("%d", &value);
    }
    return value;
}

void print_menu(char* title, char* subtitle,char *message,...) {

    printf("|\n");
    printf("|   %s%s%s%s\n",ANSI_BOLD,ANSI_GREEN,title,ANSI_RESET);
    printf("|   %s%s%s%s\n",ANSI_BLINK,ANSI_RED,subtitle,ANSI_RESET);
    printf("|\n");

    va_list args;
    va_start(args, message);

    char* current_message = message;
    int i = 0;
    while (current_message != NULL) {
        printf("|   %d : %s\n",++i, current_message);
        current_message = va_arg(args, char*);
    }
    printf("|\n");
    printf("   Saisissez votre choix : ");

    va_end(args);
}





