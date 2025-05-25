// menu.c
//
// Ce fichier implémente l'interface utilisateur du programme de traitement d'images.
// Il fournit des menus interactifs pour charger, manipuler et sauvegarder des images BMP
// en formats 8 bits (niveaux de gris) et 24 bits (couleur).
//
// Fonctionnalités principales:
// - Chargement et sauvegarde d'images
// - Application de filtres (négatif, luminosité, flou, contour, etc.)
// - Égalisation d'histogramme
// - Interface utilisateur textuelle avec gestion des entrées utilisateur
//
// Auteurs: Thomas Darbo et Matthieu Galland
// Date: Mai 2025

#include "menu.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "utils24.h"
#include "color.h"
#include "egalisation.h"
#include "filters.h"

void clear_screen() {
    system("cls");
}

int menu(t_bmp24 **image24, t_bmp8 **image8, int *selectedDepth) {
    clear_screen();
    printf("\n");
    const bool i8 = *selectedDepth == 8 && *image8;
    const bool i24 = *selectedDepth == 24 && *image24;
    int choix;

    if (i8 || i24) {
        if (i8){
            choix = print_menu("Menu de gestion Image 8 bits", "[!] Image loaded", "Appliquer un filtre","Egaliser l'image", "Sauvegarder l'image", "Decharger l'image",
                                   "Retour au menu principal", NULL);
            switch (choix) {
                case 1: image8_filter_menu(*image8); break;
                case 2: bmp8_equalize(*image8, 256); break;
                case 3: image_save(*image8,*image24,*selectedDepth); break;
                case 4: bmp_8_free(*image8); *image8 = NULL; *selectedDepth = 0; break;
                case 5: *selectedDepth = 0; break;
            }
        } else {
            choix = print_menu("Menu de gestion Image 24 bits", "[!] Image loaded", "Appliquer un filtre","Egaliser l'image", "Sauvegarder l'image", "Decharger l'image",
                                   "Retour au menu principal", NULL);
            switch (choix) {
                case 1: image24_filter_menu(*image24); break;
                case 2: bmp24_equalize(*image24); break;
                case 3: image_save(*image8,*image24,*selectedDepth); break;
                case 4: /*bmp24_free(*image24);*/ *image24 = NULL; *selectedDepth = 0; break;
                case 5: *selectedDepth = 0; break;
            }
        }
    } else {
        choix = print_menu("Menu principal", "[!] Aucune image chargee", "Charger une image", "Credits", "Quitter", NULL);
        switch (choix) {
            case 1:
                printf("Chargement d'une image...\n");
                *selectedDepth = loadImage(image24, image8);
                if (*selectedDepth == 0) {
                    printf("%s%s[ERREUR] Impossible de charger l'image.%s\n", ANSI_BOLD, ANSI_RED, ANSI_RESET);
                }
                break;
            case 2:
                printf("Projet de traitement d'image par Thomas Darbo et Matthieu Galland.\n");
                printf("Appuyez sur Entrée pour revenir au menu...");
                getchar(); getchar();
                break;
            case 3:
                return 1; // Signal to quit
        }
    }
    return 0;
}


int loadImage(t_bmp24 **image24, t_bmp8 **image8) {

    system("cls");
    printf("Outils de chargement d'images : \n");
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
            *image8 = bmp8_loadImage(chemin);
            break;
        case 24:
            printf("[INFO] Chargement de l'image BMP 24 bits...\n");
            *image24 = bmp24_loadImage(chemin);
            break;

        default:
            printf("[ERREUR] Format d'image non pris en charge (%d color bit).\n", cd);
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

    /*switch (choix) {
        case 1: image8_filter_menu(image8, FOO); break;
    }*/

    return 0;
}

void image_save(t_bmp8 *image8, t_bmp24 *image24, int colorDepth) {

    if (colorDepth == 8) {
        if (image8) {
            bmp8_saveImage("../saved_image.bmp", image8);
            colorDepth = 0; clear_screen();
            printf("%s[INFO] Image 8 bits sauvegardee avec succes !%s\n", ANSI_GREEN, ANSI_RESET);
        } else {
            printf("%s[ERREUR] Aucune image 8 bits chargee.%s\n", ANSI_RED, ANSI_RESET);
        }
    } else if (colorDepth == 24) {
        if (image24) {
            bmp24_saveImage(image24, "../saved_image.bmp");
            colorDepth = 0; clear_screen();
            printf("%s[INFO] Image 24 bits sauvegardee avec succes !%s\n", ANSI_GREEN, ANSI_RESET);
        } else {
            printf("%s[ERREUR] Aucune image 24 bits chargee.%s\n", ANSI_RED, ANSI_RESET);
        }
    } else {
        printf("%s[ERREUR] Format d'image inconnu.%s\n", ANSI_RED, ANSI_RESET);
    }

}

int image8_filter_menu(t_bmp8 *image8) {
    const int choix = print_menu(
        "Menu filtre image 8 bits", "[!] Image loaded",
        "Filtre negatif",
        "Filtre luminosite",
        "Filtre threshold",
        "Filtre box blur",
        "Filtre gaussian blur",
        "Filtre outline",
        "Filtre emboss",
        "Filtre sharpen",
        NULL);
    switch (choix) {
        case 1:
            bmp8_negative(image8);
            printf("%s[INFO] Filtre NEGATIF applique !\n%s", ANSI_GREEN, ANSI_RESET);
            break;
        case 2: {
            const int a = input_value(0,255,"LUMINOSITE");
            bmp8_brightness(a, image8);
            printf("%s[INFO] Filtre LUMINOSITE de %d applique !\n%s", ANSI_GREEN,a,ANSI_RESET);
            break;
        }
        case 3: {
            const int a = input_value(0,255,"THRESHOLD");
            bmp8_threshold(a, image8);
            printf("%s[INFO] Filtre THRESHOLD de %d applique !\n%s",ANSI_GREEN, a, ANSI_RESET);
            break;
        }
        case 4:
            bmp8_applyFilter(image8, getKernelPtr((const float (*)[3])BOX_BLUR), 3);
            printf("%s[INFO] Filtre BOX BLUR applique !\n%s", ANSI_GREEN, ANSI_RESET);
            break;
        case 5:
            bmp8_applyFilter(image8, getKernelPtr((const float (*)[3])GAUSSIAN_BLUR), 3);
            printf("%s[INFO] Filtre GAUSSIAN BLUR applique !\n%s", ANSI_GREEN, ANSI_RESET);
            break;
        case 6:
            bmp8_applyFilter(image8, getKernelPtr((const float (*)[3])OUTLINE), 3);
            printf("%s[INFO] Filtre OUTLINE applique !\n%s", ANSI_GREEN, ANSI_RESET);
            break;
        case 7:
            bmp8_applyFilter(image8, getKernelPtr((const float (*)[3])EMBOSS), 3);
            printf("%s[INFO] Filtre EMBOSS applique !\n%s", ANSI_GREEN, ANSI_RESET);
            break;
        case 8:
            bmp8_applyFilter(image8, getKernelPtr((const float (*)[3])SHARPEN), 3);
            printf("%s[INFO] Filtre SHARPEN applique !\n%s", ANSI_GREEN, ANSI_RESET);
            break;
        default:
            printf("%s%s%s[ERREUR] Choix invalide. Veuillez reessayer.\n%s", ANSI_BOLD,ANSI_UNDERLINE, ANSI_RED, ANSI_RESET);
            break;
    }
    return 0;
}

int image24_filter_menu(t_bmp24 *image24) {
    const int choix = print_menu(
        "Menu filtre image 24 bits", "[!] Image loaded",
        "Filtre negatif",
        "Filtre luminosite",
        "Filtre box blur",
        "Filtre gaussian blur",
        "Filtre outline",
        "Filtre emboss",
        "Filtre sharpen",
        NULL);
    switch (choix) {
        case 1:
            bmp24_negative(image24);
            printf("%s[INFO] Filtre NEGATIF applique !\n%s", ANSI_GREEN, ANSI_RESET);
            break;
        case 2: {
            const int a = input_value(0,255,"LUMINOSITE");
            bmp24_brightness(image24,a);
            printf("%s[INFO] Filtre LUMINOSITE de %d applique !\n%s", ANSI_GREEN,a,ANSI_RESET);
            break;
        }
        case 3:
            bmp24_applyFilter(image24, getKernelPtr((float (*)[3])BOX_BLUR), 3);
            printf("%s[INFO] Filtre BOX BLUR applique !\n%s", ANSI_GREEN, ANSI_RESET);
            break;
        case 4:
            bmp24_applyFilter(image24, getKernelPtr((float (*)[3])GAUSSIAN_BLUR), 3);
            printf("%s[INFO] Filtre GAUSSIAN BLUR applique !\n%s", ANSI_GREEN, ANSI_RESET);
            break;
        case 5:
            bmp24_applyFilter(image24, getKernelPtr((float (*)[3])OUTLINE), 3);
            printf("%s[INFO] Filtre OUTLINE applique !\n%s", ANSI_GREEN, ANSI_RESET);
            break;
        case 6:
            bmp24_applyFilter(image24, getKernelPtr((float (*)[3])EMBOSS), 3);
            printf("%s[INFO] Filtre EMBOSS applique !\n%s", ANSI_GREEN, ANSI_RESET);
            break;
        case 7:
            bmp24_applyFilter(image24, getKernelPtr((float (*)[3])SHARPEN), 3);
            printf("%s[INFO] Filtre SHARPEN applique !\n%s", ANSI_GREEN, ANSI_RESET);
            break;
        default:
            printf("%s%s%s[ERREUR] Choix invalide. Veuillez reessayer.\n%s", ANSI_BOLD,ANSI_UNDERLINE, ANSI_RED, ANSI_RESET);
            break;
    }
    return 0;
}

int input_value(int min, int max, char *message) {

    printf("\n|    Veuillez inserer une valeur pour %s :\n|      Valeurs acceptees : %d - %d", message, min, max);



    int choice = 0;
    char buffer[256];
    do {
        printf("   Saisissez votre choix (1-%d) : ", 255);
        fflush(stdout);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("\033[1A\033[2K");  // Move up one line and clear it
            printf("   Saisissez votre choix (1-%d) : %s[ERROR] Entrée invalide.%s\n",
                   255, ANSI_RED, ANSI_RESET);
            continue;
        }

        // Remove newline character if present
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }

        int valid = (sscanf(buffer, "%d", &choice) == 1 && choice >= 1 && choice <= 255);

        // Move up one line and clear it
        printf("\033[1A\033[2K");

        if (valid) {
            printf("   Saisissez votre choix (1-%d) : %d %s[VALIDE]%s\n",
                   255, choice, ANSI_GREEN, ANSI_RESET);
        } else {
            printf("   Saisissez votre choix (1-%d) : %s %s[ERROR] Choix invalide (1-%d).\n%s",
                   255, buffer, ANSI_RED, 255,ANSI_RESET);
            choice = 0;
        }

    } while (choice == 0);


    printf("\n\n");
    return choice;
}

int print_menu(char *title, char *subtitle, char *message, ...) {
    printf("|\n");
    printf("|   %s%s%s%s\n", ANSI_BOLD, ANSI_GREEN, title, ANSI_RESET);
    printf("|   %s%s%s%s\n", ANSI_BLINK, ANSI_RED, subtitle, ANSI_RESET);
    printf("|\n");

    va_list args;
    va_start(args, message);

    char *current_message = message;
    int i = 0;
    while (current_message != NULL) {
        printf("|   %d : %s\n", ++i, current_message);
        current_message = va_arg(args, char *);
    }
    printf("|\n");

    int choice = 0;
    char buffer[256];
    do {
        printf("   Saisissez votre choix (1-%d) : ", i);
        fflush(stdout);  // Ensure the prompt is displayed

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("\033[1A\033[2K");  // Move up one line and clear it
            printf("   Saisissez votre choix (1-%d) : %s[ERROR] Entrée invalide.%s\n",
                   i, ANSI_RED, ANSI_RESET);
            continue;
        }

        // Remove newline character if present
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }

        int valid = (sscanf(buffer, "%d", &choice) == 1 && choice >= 1 && choice <= i);

        // Move up one line and clear it
        printf("\033[1A\033[2K");

        if (valid) {
            printf("   Saisissez votre choix (1-%d) : %d %s[VALIDE]%s\n",
                   i, choice, ANSI_GREEN, ANSI_RESET);
        } else {
            printf("   Saisissez votre choix (1-%d) : %s %s[ERROR] Choix invalide (1-%d).%s\n",
                   i, buffer, ANSI_RED,i, ANSI_RESET);
            choice = 0;
        }

    } while (choice == 0);

    va_end(args);
    return choice;
}

float** getKernelPtr(const float arr[3][3]) {
    static float row0[3], row1[3], row2[3];
    static float* rows[3];
    for(int i=0;i<3;i++) {
        row0[i]=arr[0][i]; row1[i]=arr[1][i]; row2[i]=arr[2][i];
    }
    rows[0]=row0; rows[1]=row1; rows[2]=row2;
    return rows;
}


