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

    printf("\n");
    const bool i8 = *selectedDepth == 8;
    const bool i24 = *selectedDepth == 24;
    int choix;



    if (i8 || i24) {
        if (i8){
            choix = print_menu("Menu de gestion Image 8 bits", "[!] Image loaded", "Appliquer un filtre",
                                   "Lire les modifications de l'image", "Sauvegarder l'image", "Decharger l'image",
                                   NULL);
            system("cls");
            switch (choix) {
                case 1: image8_filter_menu(image8); break;
                case 2: /*LOGS*/ break;
                case 3: /*SAVE*/ break;
                case 4: /*BACK*/ break;
            }

        }

    } else {
        choix = print_menu("Menu principal", "[!] Aucune image chargee", "Charger une image","Credits", "Quitter", NULL);
        switch (choix) {
            case 1:
                printf("Chargement d'une image...\n");
                *selectedDepth = loadImage(image24, image8);
                if (*selectedDepth == 0) {
                    printf("%s%s[ERREUR] Impossible de charger l'image.%s\n", ANSI_BOLD, ANSI_RED, ANSI_RESET);
                }
                break;
            case 2:
                /*CREDITS*/break;
            case 3:
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

    switch (choix) {
        case 1: image8_filter_menu(image8); break;
    }

    return 0;
}

int image8_filter_menu(t_bmp8 *image8) {
    const int choix = print_menu("Menu filtre image 8 bits","[!] Image loaded","Filtre negatif","Filtre luminosite","Filtre threshold",NULL);
    switch (choix) {
        case 1: {
            bmp8_negative(image8); system("cls");
            printf("%s[INFO] Filtre NEGATIF applique !\n%s", ANSI_GREEN, ANSI_RESET);
            break;
        }
        case 2: {
            const int a = input_value(0,255,"LUMINOSITE");
            bmp8_brightness(a, image8); system("cls");
            printf("%s[INFO] Filtre LUMINOSITE de %d applique !\n%s", ANSI_GREEN,a,ANSI_RESET);
            break;
        }
        case 3: {
            const int a = input_value(0,255,"THRESHOLD");
            bmp8_brightness(a, image8); system("cls");
            printf("%s[INFO] Filtre THRESHOLD de %d applique !\n%s",ANSI_GREEN, a, ANSI_RESET);
            break;
        }
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


