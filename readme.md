# Stegnocode

Stegnocode est une chaîne d'outils permettant de cacher du code et de l'exécuter directement depuis des images.
Plus qu'un simple outil de stéganographie, il permet de transformer des images en véritables programmes exécutables.

Le projet est composé de deux éléments principaux :
 - Stegasm : Un langage de programmation complet (inspiré de l'assembleur, avec des fonctionnalités de haut niveau comme la création de fenêtres), conçu pour être compilé en ByteCode.
 - Stegnocode : L'outil de stéganographie sur-mesure permettant d'injecter et d'exécuter ce ByteCode (ou de cacher des textes/fichiers) au sein des pixels d'une image, en altérant celle-ci le moins possible.

## table des matières

<!-- TOC -->
* [Stegnocode](#stegnocode)
  * [table des matières](#table-des-matières)
  * [Informations générales](#informations-générales)
  * [Utilisation](#utilisation)
    * [**Stegnocode** (Stéganographie et Exécution d'images)](#stegnocode-stéganographie-et-exécution-dimages)
      * [Cacher de la donnée (encode) :](#cacher-de-la-donnée-encode-)
        * [Encoder des String](#encoder-des-string)
        * [Encoder des Fichier](#encoder-des-fichier)
        * [Encoder du stegocode](#encoder-du-stegocode)
      * [Décoder de la donnée (decode) :](#décoder-de-la-donnée-decode-)
    * [**Stegasm** (Compilateur et Interpréteur)](#stegasm-compilateur-et-interpréteur)
  * [Compilation](#compilation)
  * [Documentation](#documentation)
  * [Tests](#tests)
<!-- TOC -->

## Informations générales

Le projet est entièrement développé en C++ et s'appuie sur plusieurs bibliothèques externes pour fonctionner (incluses dans le projet (submodules)) :
- **stb** : Utilisée pour la récupération des pixels pour l'algorithme de stéganographie, permet également la sauvegarde des images
- **raylib** : Gère toute la partie graphique, la fenêtre, les inputs... pour les programmes écrits en Stegasm.
- **lsp-framework** : Framework simplifiant la création du serveur LSP de Stegasm.
- **googletest** : Framework utilisé pour les tests

## Utilisation

La compilation du projet génère deux exécutables principaux.
 
### **Stegnocode** (Stéganographie et Exécution d'images)

C'est le binaire principal pour interagir avec les images.

#### Cacher de la donnée (encode) :

```shell
  ./stegnocode encode <string|file|code> <options...> <input_image_path> <output_image_path>
```

##### Encoder des String

```shell
  ./stegnocode encode string "Ma super string" ./mon_image_input ./mon_image_output
```

##### Encoder des Fichier

Tout type de fichier peuvent être encodé dans l'image. Son nom et son extention seras préservé pour le décodage.

```shell
  ./stegnocode encode file ./mon_super_fichier.mp3 ./mon_image_input ./mon_image_output
```

##### Encoder du stegocode

Le code seras compiler à ce moment là puis inséré dans l'image.

```shell
  ./stegnocode encode code ./steg.stegasm ./mon_image_input ./mon_image_output
```

#### Décoder de la donnée (decode) :

Le type de donné sauvegarder dans l'image est également stocker dans l'image. Nous avons donc aucun paramètre particulier à donner.  
Si une image ne contient aucune données encoder, ce seras détecté et cela affichera une erreur.

```shell
stegnocode decode <image_path>
# Ou
stegnocode <image_path>
```

Sur windows, il est également possible de faire "Ouvrir Avec" sur un fichier .png et selectionner l'éxécutable.

### **Stegasm** (Compilateur et Interpréteur)

Ce binaire permet de compiler et d'exécuter un fichier source .stegasm à la volée, sans passer par l'étape de stéganographie.  
Il utilise le même environement de compilation et d'éxécution que Stegnocode, il est donc utilisé pour le développement des programmes Stegasm.

```shell
  ./stegasm <source_file_path>
```


## Compilation

Le projet utilise CMake comme système de build standard.  
Puisque le projet utilise des dépendances externes, il vous devez récupérer les submodules avant de lancer la compilation :  

```shell
  git submodule update --init --recursive
```

Compilation du projet :

```shell
  mkdir build
  cd build
  cmake ..
  make
```

## Documentation

Une documentation du fonctionnement en interne des outils est disponible dans ces différents fichiers :  
- [Stegnographie](doc/steganografer.md) : Explique la logique de stockage des données dans les images. 
- [Stegasm](doc/stegasm.md)  : Détaille l'architecture de la VM, les registres, le format du fichier binaire, la gestion de la mémoire/fichiers/graphismes, et la liste complète des instructions supportées.

## Tests

```shell
    ./stegnocode_tests
```