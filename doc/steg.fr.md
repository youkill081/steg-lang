# Le langage Steg

**Steg** est le langage haut niveau du projet. Il se compile en StegASM via une chaîne complète :

```
Lexer -> Parseur -> AST -> Analyse sémantique -> IR -> Backend Génération StegASM
```

Les fichiers sources portent l'extension `.steg`.

> Pour une introduction à StegASM, le langage bas niveau, consultez la [documentation StegASM](./stegasm.fr.md).

---

## Table des matières

<!-- TOC -->
* [Le langage Steg](#le-langage-steg)
  * [Table des matières](#table-des-matières)
  * [Structure d'un fichier .steg](#structure-dun-fichier-steg)
  * [Types](#types)
    * [Types primitifs](#types-primitifs)
    * [Types opaques](#types-opaques)
    * [Conversions](#conversions)
  * [Variables](#variables)
    * [Variables locales](#variables-locales)
    * [Variables globales](#variables-globales)
    * [Chaînes de caractères](#chaînes-de-caractères)
  * [Fonctions](#fonctions)
    * [Déclaration](#déclaration)
    * [Exportation](#exportation)
  * [Imports et exports](#imports-et-exports)
    * [Importer des symboles](#importer-des-symboles)
    * [Propagation des fichiers](#propagation-des-fichiers)
  * [Structures de contrôle](#structures-de-contrôle)
    * [Conditions](#conditions)
    * [Boucle while](#boucle-while)
    * [Boucle for](#boucle-for)
    * [break et continue](#break-et-continue)
  * [Section files](#section-files)
  * [Type file en détail](#type-file-en-détail)
  * [Fonctions built-in](#fonctions-built-in)
    * [Affichage](#affichage)
    * [Système](#système)
    * [Mathématiques](#mathématiques)
    * [Image](#image)
    * [Fenêtre - Cycle de vie](#fenêtre---cycle-de-vie)
    * [Fenêtre - Rendu](#fenêtre---rendu)
    * [Fenêtre - Clavier](#fenêtre---clavier)
    * [Fenêtre - Souris](#fenêtre---souris)
    * [Fenêtre - Framebuffer](#fenêtre---framebuffer)
    * [Fichiers et vecteurs](#fichiers-et-vecteurs)
    * [Horloge](#horloge)
<!-- TOC -->

---

## Structure d'un fichier .steg

Un fichier `.steg` peut contenir les éléments suivants, dans n'importe quel ordre :

| Élément            | Rôle                                              |
|--------------------|---------------------------------------------------|
| `import { ... }`   | Importation de symboles depuis d'autres fichiers  |
| `files { ... }`    | Fichiers embarqués dans le binaire                |
| Variables globales | Variables accessibles dans tout le fichier        |
| `fn` / `export fn` | Définition de fonctions                           |

Exemple complet d'un fichier `.steg` :

```steg
import { init_gun, update_player_pos } from "./game/player_helper.steg";
 
files {
    GAMEMAPS: "./assets/GAMEMAPS.WL6"
}
 
uint32 num_doors = 0;
 
export fn main() -> void {
    print_str("Hello world !");
}
```

---

## Types

### Types primitifs

| Type     | Taille  | Description                  |
|----------|---------|------------------------------|
| `uint8`  | 8 bits  | Entier non signé sur 8 bits  |
| `uint16` | 16 bits | Entier non signé sur 16 bits |
| `uint32` | 32 bits | Entier non signé sur 32 bits |
| `int`    | 32 bits | Entier signé                 |
| `float`  | 32 bits | Flottant                     |
| `bool`   | 8 bits  | Booléen                      |

### Types opaques

Les types opaques représentent des ressources gérées par l'interpréteur. Ils ne peuvent pas être créés par une valeur littérale - ils sont obtenus et manipulés exclusivement via des fonctions built-in dédiées.

| Type          | Description                                      |
|---------------|--------------------------------------------------|
| `file`        | Fichier sur disque ou vecteur binaire en mémoire |
| `framebuffer` | Surface de dessin pour le rendu graphique        |
| `clock`       | Horloge pour la gestion du temps                 |

### Conversions

Les conversions entre types primitifs (`int`, `float`, `uint8`, `uint16`, `uint32`) sont toutes **implicites**. Il n'existe pas de syntaxe de cast explicite.

```steg
float dist = 5.56;  // uint -> float, implicite
uint32 x = dist;    // float -> uint32, implicite -> x = 5
```

> Les conversions implicites peuvent générer des **hints** si elles peuvent perdre de la donnée (ex : float -> uint). Le compilateur vous en informera.

---

## Variables

### Variables locales

Les variables locales se déclarent à l'intérieur d'une fonction et sont initialisées avec une expression :

```steg
fn exemple() -> void {
    uint32 x = 0;
    float angle = 3.14;
    uint16 door_id = file_read_little_uint16_at(d_map, (y * 64 + x) * 2);
}
```

### Variables globales

Les variables globales se déclarent en dehors de toute fonction. Leur initialisation est limitée à des **valeurs constantes** - les expressions ne sont pas autorisées.

```steg
uint32 num_doors = 0;  // OK
file d_map = 0;        // OK, descripteur invalide avant initialisation
uint32 computed = 5 + 2; // Interdit : expression non constante
```

> ⚠️ Les variables globales ne sont **pas exportées** automatiquement. Pour les exposer, créez une fonction getter explicitement exportée :

```steg
uint32 num_doors = 0;
 
export fn get_num_doors() -> uint32 {
    return num_doors;
}
```

### Chaînes de caractères

Une chaîne de caractères se déclare avec la notation `@` sur un type `uint8` :

```steg
uint8 @my_string = "hello world";
```

> Les tableaux de nombres ne sont pas encore supportés. Seules les chaînes de caractères peuvent utiliser cette notation.

---

## Fonctions

### Déclaration

Une fonction se déclare avec le mot-clé `fn`. Le type de retour est **obligatoire** :

```steg
fn add(int a, int b) -> int {
    return a + b;
}
 
fn main() -> void {
    int result = add(3, 7);
}
```

Le type `void` indique qu'une fonction ne retourne rien.

### Exportation

Par défaut, une fonction n'est pas accessible depuis un autre fichier. Pour la rendre importable, utilisez le mot-clé `export` :

```steg
export fn get_door_openness(int x, int y) -> float {
    uint16 door_id = file_read_little_uint16_at(d_map, (y * 64 + x) * 2);
 
    if (door_id == 65535) { return 0.0; }
    return file_read_little_uint32_at(d_open, door_id * 4) / 10000.0;
}
```

---

## Imports et exports

### Importer des symboles

On importe des symboles nommés depuis un autre fichier `.steg` avec la syntaxe suivante :

```steg
import { nom_fonction, autre_fonction } from "./chemin/vers/fichier.steg";
```

```steg
import { load_level } from "./file_loader.steg";
import { cast_rays, init_renderer, get_zbuffer } from "./renderer.steg";
```

Seules les fonctions marquées `export` dans le fichier source sont importables (voir [Exportation](#exportation)).

### Propagation des fichiers

La section `files { ... }` est **automatiquement exportée** avec le fichier `.steg`. Si un fichier A importe B, et que B déclare des ressources dans `files { ... }`, ces ressources sont disponibles dans A.

Les variables globales, elles, ne se propagent **pas** - elles restent privées au fichier qui les déclare.

---

## Structures de contrôle

### Conditions

Les blocs `if` / `else` suivent une syntaxe classique. Les accolades sont **obligatoires** :

```steg
if (door_id == 65535) {
    return 0.0;
} else {
    return 1.0;
}
```

> ⚠️ `else if` n'est **pas supporté**. Pour des conditions chaînées, imbriquez des `if` dans les blocs `else` :

```steg
if (x == 0) {
    // cas 1
} else {
    if (x == 1) {
        // cas 2
    } else {
        // cas 3
    }
}
```

### Boucle while

```steg
while (!window_should_close()) {
    window_clear(0, 0, 0);
    window_present();
}
```

### Boucle for

```steg
uint32 i = 0;
for (i = 0; i < count; i += 1) {
    file_append_little_uint16(value, out_file);
}
```

> La variable de boucle doit être **déclarée avant** le `for`.

### break et continue

Les mots-clés `break` et `continue` sont disponibles dans les boucles `while` et `for` :

```steg
while (true) {
    if (x > 100) { break; }
    if (x % 2 == 0) { continue; }
    x += 1;
}
```

---

## Section files

La section `files` permet d'**embarquer des fichiers directement dans le binaire** compilé. Elle est l'équivalent direct de la section `.file` en StegASM.

```steg
files {
    GAMEMAPS: "./assets/GAMEMAPS.WL6"
    MAPHEAD:  "./assets/MAPHEAD.WL6"
    VSWAP:    "./assets/VSWAP.WL6"
}
```

Chaque entrée associe un **identifiant** (ex : `GAMEMAPS`) à un chemin de fichier. Ces identifiants sont ensuite utilisables comme arguments des fonctions built-in :

```steg
file header = file_open(MAPHEAD);
```

La section `files` est **automatiquement exportée** : tout fichier qui importe un `.steg` contenant cette section bénéficie également des ressources déclarées.

---

## Type file en détail

Le type `file` est le type opaque le plus polyvalent du langage. Au-delà de la simple lecture de fichiers sur disque, il sert également de **vecteur dynamique en mémoire** - l'équivalent d'un vecteur ou d'un buffer.

Toutes les opérations sur un `file` sont traitées directement par l'interpréteur : pas d'allocation dynamique explicite, pas d'overhead.

**Créer un vecteur vide :**

```steg
file z_buffer = file_create("");
```

**Écrire dans un vecteur :**

```steg
file_clear_data(z_buffer);   // Vide toutes les données
file_reset_cursor(z_buffer); // Replace le curseur au début
 
while (x < 320) {
    float value = dist * cos(deg2rad(angle));
    file_append_uint32(value * 10000.0, z_buffer);
    x += 1;
}
```

**Lire depuis un vecteur :**

```steg
file_seek_cursor(0, in_file);
uint16 w = file_read_little_uint16(in_file);
```

**Passer un `file` en paramètre et le retourner :**

Les types `file` peuvent être passés en argument et retournés par une fonction comme n'importe quel autre type :

```steg
fn rlew_decode(file in_file, uint16 rlew_tag, uint32 in_length) -> file {
    file out_file = file_create("");
    // ...
    return out_file;
}
```

---

## Fonctions built-in

Les fonctions built-in sont directement intégrées au langage. Elles ne nécessitent aucun import et sont compilées directement en instructions StegASM - leur overhead est donc quasi **nul**.

### Affichage

| Signature             | Retour | Description                      |
|-----------------------|--------|----------------------------------|
| `print_n(uint32 v)`   | `void` | Affiche un entier non signé      |
| `print_sn(int v)`     | `void` | Affiche un entier signé          |
| `print_f(float v)`    | `void` | Affiche un flottant              |
| `print_c(uint8 v)`    | `void` | Affiche un caractère             |
| `print_b(uint32 v)`   | `void` | Affiche un entier en binaire     |
| `print_str(uint8 @s)` | `void` | Affiche une chaîne de caractères |

### Système

| Signature           | Retour   | Description                                                                                                                                                                                                                                                          |
|---------------------|----------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `exit(uint8 code)`  | `void`   | Termine l'exécution avec un code de sortie                                                                                                                                                                                                                           |
| `aloc(uint32 size)` | `void @` | Alloue un bloc mémoire                                                                                                                                                                                                                                               |
| `free(void @ptr)`   | `void`   | Libère un bloc mémoire alloué par `aloc`                                                                                                                                                                                                                             |
| `rand()`            | `uint32` | Returnee un entier pseudo-aléatoire                                                                                                                                                                                                                                  |
| `debug_r()`         | `void`   | Affiche l'état des registres                                                                                                                                                                                                                                         |
| `debug_m()`         | `void`   | Affiche l'état de la mémoire                                                                                                                                                                                                                                         |
| `debug_t(uint32 v)` | `void`   | 1er appel : Elle lance le chrono pour l'ID choisi. ;<br/>Appels suivants : Elle calcule et affiche le temps écoulé (le Delta) depuis le dernier passage, puis redémarre le chrono.<br/>Elle permet par éxemple de calcules le temps de chaque itération d'une boucle |

### Mathématiques

| Signature                | Retour  | Description                     |
|--------------------------|---------|---------------------------------|
| `sin(float v)`           | `float` | Sinus                           |
| `cos(float v)`           | `float` | Cosinus                         |
| `ftan(float v)`          | `float` | Tangente                        |
| `fatan(float v)`         | `float` | Arc-tangente                    |
| `sqrt(float v)`          | `float` | Racine carrée                   |
| `fabs(float v)`          | `float` | Valeur absolue                  |
| `fmax(float a, float b)` | `float` | Maximum de deux flottants       |
| `fmin(float a, float b)` | `float` | Minimum de deux flottants       |
| `deg2rad(float v)`       | `float` | Convertit des degrés en radians |
| `rad2deg(float v)`       | `float` | Convertit des radians en degrés |

### Image

| Signature             | Retour     | Description                                                           |
|-----------------------|------------|-----------------------------------------------------------------------|
| `image_map(file f)`   | `uint32 @` | Mappe une image en mémoire, retourne un pointeur vers les pixels RGBA |
| `last_image_size_x()` | `uint32`   | Largeur de la dernière image mappée                                   |
| `last_image_size_y()` | `uint32`   | Hauteur de la dernière image mappée                                   |

### Fenêtre - Cycle de vie

| Signature                                         | Retour  | Description                                                 |
|---------------------------------------------------|---------|-------------------------------------------------------------|
| `window_create(uint32 w, uint32 h, uint8 @title)` | `void`  | Crée et ouvre la fenêtre                                    |
| `window_close()`                                  | `void`  | Ferme la fenêtre                                            |
| `window_should_close()`                           | `bool`  | Vrai si l'utilisateur a demandé la fermeture                |
| `window_toggle_fullscreen()`                      | `void`  | Bascule le mode plein écran                                 |
| `window_set_viewport_size(uint32 w, uint32 h)`    | `void`  | Définit la taille du viewport interne                       |
| `window_disable_viewport()`                       | `void`  | Désactive le viewport interne                               |
| `window_set_target_fps(uint32 fps)`               | `void`  | Cible un nombre d'images par seconde                        |
| `window_get_delta()`                              | `float` | Retourne le delta-time en secondes depuis la dernière frame |
| `window_set_icon(file f)`                         | `void`  | Définit l'icône de la fenêtre                               |

### Fenêtre - Rendu

| Signature                                                  | Retour | Description                               |
|------------------------------------------------------------|--------|-------------------------------------------|
| `window_clear(uint8 r, uint8 g, uint8 b)`                  | `void` | Efface l'écran avec une couleur unie      |
| `window_present()`                                         | `void` | Présente le frame courant à l'écran       |
| `window_draw_text(uint32 x, uint32 y, uint8 @s)`           | `void` | Dessine du texte à la position donnée     |
| `window_draw_texture(uint32 x, uint32 y, file f)`          | `void` | Dessine une texture à la position donnée  |
| `window_set_text_size(uint32 size)`                        | `void` | Définit la taille du texte                |
| `window_set_text_color(uint8 r, uint8 g, uint8 b)`         | `void` | Définit la couleur du texte               |
| `window_set_font(file f)`                                  | `void` | Charge et active une police de caractères |
| `window_set_texture_color_mask(uint8 r, uint8 g, uint8 b)` | `void` | Applique un masque coloré aux textures    |
| `window_reset_texture_color_mask()`                        | `void` | Retire le masque coloré                   |

### Fenêtre - Clavier

| Signature                        | Retour | Description                                            |
|----------------------------------|--------|--------------------------------------------------------|
| `window_key_pressed(uint32 key)` | `bool` | Vrai si la touche vient d'être pressée (front montant) |
| `window_key_down(uint32 key)`    | `bool` | Vrai si la touche est maintenue enfoncée               |

### Fenêtre - Souris

| Signature                                  | Retour   | Description                                           |
|--------------------------------------------|----------|-------------------------------------------------------|
| `window_mouse_x()`                         | `uint32` | Position X du curseur                                 |
| `window_mouse_y()`                         | `uint32` | Position Y du curseur                                 |
| `window_mouse_delta_x()`                   | `int`    | Déplacement X depuis le dernier frame                 |
| `window_mouse_delta_y()`                   | `int`    | Déplacement Y depuis le dernier frame                 |
| `window_mouse_wheel_delta()`               | `int`    | Défilement de la molette depuis le dernier frame      |
| `window_mouse_button_pressed(uint32 btn)`  | `bool`   | Vrai si le bouton vient d'être cliqué (front montant) |
| `window_mouse_button_down(uint32 btn)`     | `bool`   | Vrai si le bouton est maintenu enfoncé                |
| `window_mouse_button_released(uint32 btn)` | `bool`   | Vrai si le bouton vient d'être relâché                |
| `window_hide_cursor()`                     | `void`   | Masque le curseur                                     |
| `window_show_cursor()`                     | `void`   | Affiche le curseur                                    |
| `window_disable_cursor()`                  | `void`   | Désactive et capture le curseur (mode FPS)            |
| `window_enable_cursor()`                   | `void`   | Réactive le curseur                                   |

### Fenêtre - Framebuffer

| Signature                                                     | Retour        | Description                                          |
|---------------------------------------------------------------|---------------|------------------------------------------------------|
| `window_framebuffer_create(uint32 w, uint32 h)`               | `framebuffer` | Crée un framebuffer de la taille donnée              |
| `window_framebuffer_get_address(framebuffer fb)`              | `uint32 @`    | Retourne un pointeur vers les pixels du framebuffer  |
| `window_framebuffer_sync(framebuffer fb)`                     | `void`        | Synchronise les modifications en mémoire vers le GPU |
| `window_framebuffer_draw(framebuffer fb, uint32 x, uint32 y)` | `void`        | Dessine le framebuffer à la position donnée          |

### Fichiers et vecteurs

| Signature                                  | Retour   | Description                                    |
|--------------------------------------------|----------|------------------------------------------------|
| `file_open(uint8 @path)`                   | `file`   | Ouvre un fichier existant sur disque           |
| `file_create(uint8 @path)`                 | `file`   | Crée un nouveau fichier ou vecteur vide (`""`) |
| `file_save(file f)`                        | `void`   | Sauvegarde le fichier sur disque               |
| `file_delete(file f)`                      | `void`   | Supprime le fichier sur disque                 |
| `file_close(file f)`                       | `void`   | Ferme et libère le descripteur                 |
| `file_size(file f)`                        | `uint32` | Retourne la taille en octets                   |
| `file_map(file f)`                         | `void @` | Mappe tout le fichier en mémoire               |
| `file_map_x_from_cursor(file f, uint32 n)` | `void @` | Mappe `n` octets depuis la position du curseur |
| `file_reset_cursor(file f)`                | `void`   | Replace le curseur au début                    |
| `file_seek_cursor(uint32 pos, file f)`     | `void`   | Déplace le curseur à la position absolue `pos` |
| `file_get_cursor(file f)`                  | `uint32` | Retourne la position courante du curseur       |
| `file_clear_data(file f)`                  | `void`   | Efface toutes les données du fichier/vecteur   |
| `file_is_uint8_remaining(file f)`          | `bool`   | Vrai s'il reste au moins 1 octet à lire        |
| `file_is_uint16_remaining(file f)`         | `bool`   | Vrai s'il reste au moins 2 octets à lire       |
| `file_is_uint32_remaining(file f)`         | `bool`   | Vrai s'il reste au moins 4 octets à lire       |

**Lecture séquentielle (depuis le curseur) :**

| Signature                         | Retour   | Description                |
|-----------------------------------|----------|----------------------------|
| `file_read_uint8(file f)`         | `uint8`  | Lit 1 octet                |
| `file_read_uint16(file f)`        | `uint16` | Lit 2 octets big-endian    |
| `file_read_little_uint16(file f)` | `uint16` | Lit 2 octets little-endian |
| `file_read_uint32(file f)`        | `uint32` | Lit 4 octets big-endian    |
| `file_read_little_uint32(file f)` | `uint32` | Lit 4 octets little-endian |

**Lecture absolue (à une position donnée) :**

| Signature                                        | Retour   | Description                        |
|--------------------------------------------------|----------|------------------------------------|
| `file_read_uint8_at(file f, uint32 pos)`         | `uint8`  | Lit 1 octet à `pos`                |
| `file_read_uint16_at(file f, uint32 pos)`        | `uint16` | Lit 2 octets big-endian à `pos`    |
| `file_read_little_uint16_at(file f, uint32 pos)` | `uint16` | Lit 2 octets little-endian à `pos` |
| `file_read_uint32_at(file f, uint32 pos)`        | `uint32` | Lit 4 octets big-endian à `pos`    |
| `file_read_little_uint32_at(file f, uint32 pos)` | `uint32` | Lit 4 octets little-endian à `pos` |

**Écriture séquentielle (append) :**

| Signature                                     | Retour | Description                      |
|-----------------------------------------------|--------|----------------------------------|
| `file_append_uint8(uint8 v, file f)`          | `void` | Ajoute 1 octet en fin de fichier |
| `file_append_uint16(uint16 v, file f)`        | `void` | Ajoute 2 octets big-endian       |
| `file_append_little_uint16(uint16 v, file f)` | `void` | Ajoute 2 octets little-endian    |
| `file_append_uint32(uint32 v, file f)`        | `void` | Ajoute 4 octets big-endian       |
| `file_append_little_uint32(uint32 v, file f)` | `void` | Ajoute 4 octets little-endian    |

**Écriture à la position du curseur :**

| Signature                                    | Retour | Description                            |
|----------------------------------------------|--------|----------------------------------------|
| `file_write_uint8(uint8 v, file f)`          | `void` | Écrit 1 octet à la position du curseur |
| `file_write_uint16(uint16 v, file f)`        | `void` | Écrit 2 octets big-endian              |
| `file_write_little_uint16(uint16 v, file f)` | `void` | Écrit 2 octets little-endian           |
| `file_write_uint32(uint32 v, file f)`        | `void` | Écrit 4 octets big-endian              |
| `file_write_little_uint32(uint32 v, file f)` | `void` | Écrit 4 octets little-endian           |

### Horloge

| Signature                       | Retour   | Description                                |
|---------------------------------|----------|--------------------------------------------|
| `clock_create()`                | `clock`  | Crée une horloge et démarre le chronomètre |
| `clock_delete(clock c)`         | `void`   | Libère l'horloge                           |
| `clock_reset(clock c)`          | `void`   | Remet le chronomètre à zéro                |
| `clock_get_elapsed_ms(clock c)` | `uint32` | Temps écoulé en millisecondes              |
| `clock_get_elapsed_s(clock c)`  | `uint32` | Temps écoulé en secondes                   |