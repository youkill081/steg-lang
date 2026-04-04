# Steg

Steg est un langage de haut niveau conçu pour être compilé vers le StegASM. 
Sa particularité est de permettre l'exécution de programmes directement depuis des fichiers images (PNG) via la stéganographie.

# Usage CLI

Le binaire steg (dans le dossier /bin) permet de gérer vos projets:  
**Lancer un script source** : `steg run main.steg`  
**Compiler dans une image** : `steg build main.steg input.png output.png`   
**Lancer une image compilée** : `steg run_img output.png`  

Le lancement d'une image compilé peux être également directement fait avec stegcli : `stegcli output.png`   

# Structure d'un programme

## Block "files"

Ce bloc permet de définir des ressources externes (images, icônes, polices) qui seront intégré dans le binaire final 
par le compilateur pour être utilisées dans le code.

```
files {
    mon_image: "path/to/texture.png"
    mon_icon: "path/to/icon.png"
}
```

Les fichiers sont automatiquement exporté

## Variables et Types

Steg supporte les types primitif signé et non signé ainsi que les booléens :
- uint8, int8 | Codé sur 1 octet
- uint16, int16 | Codé sur 2 octets
- uint32, int32 | Codé sur 4 octets
- bool : true ou false

Il existe également deux type opaque :
- file : type associé à un fichier
- clock : type associ7é à une clock

## Pointeurs et Mémoire

- Déclaration : uint8 @ptr (L'arobase @ indique un pointeur).
- Accès indexé : ptr[i] ; PS: Il est aujourdhui imposible de chainer des indexations. (ptr[i][y] = impossible)
- Déréférencement : #ptr (Récupère directement la valeur pointée).

# Fonctions Builtins

## Entrées / Sorties & Système

| Fonction | Signature          | Description                         |
|----------|--------------------|-------------------------------------|
| print_n  | (uint32) -> void   | Affiche un nombre dans la console.  |
| print_c  | (uint8) -> void    | Affiche un caractère (ASCII).       |
| exit     | (uint8) -> void    | Arrête la VM avec un code.          |
| rand     | () -> uint32       | Génère un nombre aléatoire.         |
| aloc     | (uint32) -> void @ | Alloue de la mémoire dynamiquement. |
| free     | (void @) -> void   | Libère la mémoire allouée.          |


## Gestion de Fenêtre

### Cycle de vie

| Fonction              | Signature               | Description                                                     |
|-----------------------|-------------------------|-----------------------------------------------------------------|
| window_create         | (u32, u32, u8@) -> void | Ouvre une fenêtre (Largeur, Hauteur, Titre).                    |
| window_close          | () -> void              | Ferme la fenêtre et détruit le contexte graphique.              |
| window_should_close   | () -> bool              | Retourne true si l'utilisateur clique sur la croix ou fait ESC. |
| window_set_target_fps | (u32) -> void           | Limite le nombre de rafraîchissements par seconde.              |
| window_set_icon       | (FILE) -> void          | Définit l'icône de la fenêtre via un fichier PNG du bloc files. |

### Gestion du Viewport

| Fonction                 | Signature          | Description                                                       |
|--------------------------|--------------------|-------------------------------------------------------------------|
| window_set_viewport_size | (u32, u32) -> void | Définit une taille de zone de rendu                               |
| window_disable_viewport  | () -> void         | Désactive le viewport personnalisé et revient à la taille totale. |

### Entrées Clavier

Les touche correspondent exactement au code utilisé par Raylib.

| Fonction           | Signature     | Description                                           |
|--------------------|---------------|-------------------------------------------------------|
| window_key_pressed | (u32) -> bool | true uniquement à l'instant où la touche est pressée. |
| window_key_down    | (u32) -> bool | true tant que la touche reste enfoncée.               |

### Rendu & Style

| Fonction            | Signature                | Description                                     |
|---------------------|--------------------------|-------------------------------------------------|
| window_clear        | (u8, u8, u8) -> void     | Efface l'écran avec une couleur RGB.            |
| window_present      | void                     | Envoie les dessins à l'écran.    |
| window_draw_texture | (u32, u32, FILE) -> void | Dessine une image chargée à la position (x, y). |

### Texte et Polices

| Fonction              | Signature               | Description                                                |
|-----------------------|-------------------------|------------------------------------------------------------|
| window_set_font       | (FILE) -> void          | Définit la police active (fichier .ttf ou .fnt).           |
| window_set_text_size  | (u32) -> void           | Définit la taille de la police pour les prochains dessins. |
| window_set_text_color | (u8, u8, u8) -> void    | Définit la couleur du texte (RGB).                         |
| window_draw_text      | (u32, u32, u8@) -> void | Affiche une chaîne de caractères à (x, y).                 |

### Masques de Couleur (Effets)

| Fonction                        | Signature            | Description                                                    |
|---------------------------------|----------------------|----------------------------------------------------------------|
| window_set_texture_color_mask   | (u8, u8, u8) -> void | Applique une teinte RGB sur les prochaines textures dessinées. |
| window_reset_texture_color_mask | () -> void           | Désactive la teinte.                                           |

## Gestion de Fichiers

| Fonction               | Signature              | Description                                       |
|------------------------|------------------------|---------------------------------------------------|
| file_open              | (uint8 @) -> FILE      | Ouvre un fichier existant à partir de son chemin. |
| file_create            | (uint8 @) -> FILE      | Crée un nouveau fichier sur le disque.            |
| file_save              | (FILE) -> void         | Force l'écriture des changements sur le disque.   |
| file_delete            | (FILE) -> void         | Supprime le fichier associé au handle.            |
| file_close             | (FILE) -> void         | Ferme le handle et libère les ressources.         |
| file_reset_cursor      | (FILE) -> void         | Replace le curseur de lecture/écriture au début.  |
| file_clear_data        | (FILE) -> void         | Efface tout le contenu du fichier (tronquage).    |
| file_read_byte         | (FILE) -> uint8        | Lit l'octet (8-bit) à la position actuelle.       |
| file_read_word         | (FILE) -> uint16       | Lit le mot (16-bit) à la position actuelle.       |
| file_append_byte       | (uint8, FILE) -> void  | Ajoute un octet à la fin du fichier.              |
| file_append_word       | (uint16, FILE) -> void | Ajoute un mot (16-bit) à la fin du fichier.       |
| file_is_byte_remaining | (FILE) -> bool         | Vérifie s'il reste au moins 1 octet à lire.       |
| file_is_word_remaining | (FILE) -> bool         | Vérifie s'il reste au moins 2 octets à lire.      |


## Gestion du Temps (Clocks)

| Fonction             | Signature         | Description                                  |
|----------------------|-------------------|----------------------------------------------|
| clock_create         | () -> CLOCK       | Instancie un nouveau chronomètre système.    |
| clock_delete         | (CLOCK) -> void   | Détruit le chronomètre et libère la mémoire. |
| clock_get_elapsed_ms | (CLOCK) -> uint32 | Récupère le temps écoulé en millisecondes.   |
| clock_get_elapsed_s  | (CLOCK) -> uint32 | Récupère le temps écoulé en secondes.        |
| clock_reset          | (CLOCK) -> void   | Remet le compteur du chronomètre à zéro.     |

