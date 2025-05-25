// main.c
//
// Programme principal de traitement d'images au format BMP.
// Ce fichier contient le point d'entrée du programme et gère la boucle
// principale d'interaction avec l'utilisateur via le système de menus.
//
// Fonctionnalités:
// - Chargement et manipulation d'images BMP 8 bits (niveaux de gris) et 24 bits (couleur)
// - Interface utilisateur basée sur des menus textuels
// - Gestion de la mémoire des images chargées
//
// Auteurs: Thomas Darbo et Matthieu Galland
// Date: 25 Mai 2025

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "menu.h"
#include "utils24.h"
#include <conio.h>
#ifdef _WIN32
    #include <windows.h>
#endif


int main(void) {
    t_bmp24 *image24 = NULL;
    t_bmp8 *image8 = NULL;
    int selectedDepth = 0;

    while (1) {
        int quit = menu(&image24, &image8, &selectedDepth);
        if (quit) break;
    }

    // if (image24) bmp24_free(image24);
    if (image8) bmp_8_free(image8);

    printf("Au revoir !\n");
    return 0;
}
