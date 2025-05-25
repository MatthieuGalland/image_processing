# 🖼️ Logiciel de Traitement d'Image

[![Langage: C](https://img.shields.io/badge/Langage-C-orange.svg?style=for-the-badge)](https://www.c-language.org/)
![Static Badge](https://img.shields.io/badge/Auteurs-Thomas_Darbo_--_Matthieu_Galland-blue?style=for-the-badge)
![Static Badge](https://img.shields.io/badge/PROJET-Algorithmique_et_structures_de_donnees-red?style=for-the-badge&link=https%3A%2F%2Fmoodle.myefrei.fr%2Fcourse%2Fview.php%3Fid%3D14372)


## Table des Matières

- [Présentation](#présentation)
- [Fonctionnalités](#-fonctionnalités)
- [Installation](#-installation)
- [Utilisation](#-utilisation)
- [Auteurs](#-auteurs)

## Présentation

Ce programme vous permet de charger des images bitmap (8 bits et 24 bits) et d'appliquer divers filtres de traitement d'image. Les images traitées peuvent être sauvegardées au format bitmap. L'outil dispose d'une interface terminal simple et intuitive pour une utilisation facile.

## Fonctionnalités

| Catégorie | Fonctionnalités |
|----------|----------|
| **Support d'Image** | Formats bitmap 8 bits et 24 bits |
| **Filtres de Flou** | Flou gaussien, Flou box |
| **Traitement des Couleurs** | Inversion des couleurs, Ajustement de la luminosité |
| **Traitement d'Histogramme** | Égalisation des histogrammes |
| **Interface** | Interface utilisateur simple basée sur le terminal |
| **Opérations E/S** | Chargement/sauvegarde de fichiers bitmap |

### Détails des Filtres

- **Flou Gaussien**: Lisse les images en utilisant une fonction gaussienne
- **Flou Box**: Simple moyenne des pixels environnants
- **Inversion des Couleurs**: Inverse les valeurs de couleur de chaque pixel
- **Niveaux de Luminosité**: Ajuste la luminosité globale de l'image
- **Égalisation des Histogrammes**: Améliore le contraste en redistribuant les valeurs d'intensité

## Installation

### Prérequis

- Compilateur C/C++
- CMake (optionnel)
- Git

### Étapes d'Installation

1. **Cloner le dépôt**:
   ```bash
   git clone https://github.com/MatthieuGalland/image_processing.git
   ```

2. **Accéder au répertoire du projet**:
   ```bash
   cd image_processing
   ```

3. **Compiler le projet**:

    - **Avec CMake**:
      ```bash
      mkdir build
      cd build
      cmake ..
      cmake --build .
      ```

    - **Avec CLion**:
      ```
      Ouvrir le projet et cliquer sur le marteau "Build" dans la barre d'outils.
      ```

    - **Alternative**:
      Vous pouvez également télécharger directement l'exécutable depuis la dernière release du projet github.

## Utilisation

1. **Lancer le programme**:
   ```bash
   ./image_processing
   ```

2. **Suivre les instructions** affichées dans le terminal pour:
    - Charger une image
    - Appliquer les filtres souhaités
    - Sauvegarder l'image traitée

## Auteurs

- **Matthieu Galland** - [matthieu.galland@efrei.net](mailto:matthieu.galland@efrei.net)
- **Thomas Darbo** - [thomas.darbo@efrei.net](mailto:thomas.darbo@efrei.net)

---
