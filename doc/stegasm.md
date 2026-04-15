# StegASM

StegASM est le langage assembleur bas niveau de la VM **StegVM**. Il mélange
des instructions très bas niveau (déplacement de registres, comparaisons, arithmétique...)
et des instructions haut niveau (fenêtres, fichiers, vecteurs, horloge...).

StegVM consomme StegASM directement sous forme de bytecode. Le processus de transformation est expliqué dans la section [Développeur - Compilation](#compilation).

---

## Table des matières

<!-- TOC -->
* [StegASM](#stegasm)
  * [Table des matières](#table-des-matières)
  * [Documentation Utilisateur](#documentation-utilisateur)
    * [Structure d'un fichier .stegasm](#structure-dun-fichier-stegasm)
      * [Section .text](#section-text)
      * [Section .data](#section-data)
        * [Types de variables](#types-de-variables)
        * [Utilisation des variables](#utilisation-des-variables)
      * [Section .file](#section-file)
      * [Section .sub_textures](#section-sub_textures)
      * [Section .import](#section-import)
    * [Registres](#registres)
    * [Labels et sauts](#labels-et-sauts)
    * [Commentaires](#commentaires)
    * [Liste des opérandes](#liste-des-opérandes)
  * [Documentation Développeur](#documentation-développeur)
    * [Implémentation](#implémentation)
    * [Compilation](#compilation)
      * [Format du fichier binaire](#format-du-fichier-binaire)
        * [Les variables](#les-variables)
        * [Les fichiers](#les-fichiers)
        * [Les sub_textures](#les-sub_textures)
        * [Les instructions](#les-instructions)
    * [Liste des opérandes StegASM](#liste-des-opérandes-stegasm)
      * [Types d'opérandes](#types-dopérandes)
      * [Chargement / Stockage](#chargement--stockage)
      * [Arithmétique entière](#arithmétique-entière)
      * [Arithmétique flottante](#arithmétique-flottante)
        * [Conversion entier <-> flottant](#conversion-entier---flottant)
      * [Comparaison et sauts conditionnels](#comparaison-et-sauts-conditionnels)
      * [Affichage et debug](#affichage-et-debug)
      * [Fenêtre](#fenêtre)
        * [Création et configuration](#création-et-configuration)
        * [Dessin](#dessin)
        * [Framebuffer](#framebuffer)
        * [Entrées clavier](#entrées-clavier)
        * [Entrées souris](#entrées-souris)
      * [Image](#image)
      * [Fichiers](#fichiers)
        * [Gestion des fichiers](#gestion-des-fichiers)
        * [Curseur](#curseur)
        * [Lecture](#lecture)
        * [Écriture et ajout](#écriture-et-ajout)
      * [Horloge](#horloge)
<!-- TOC -->

---

## Documentation Utilisateur

> ⚠️ Coder en StegASM est fastidieux mais tout à fait possible (cf. le [Tetris](../examples/tetris/tetris.stegasm)).  
> Il est fortement conseillé de développer en **Steg** (le langage haut niveau du projet) qui compile directement vers StegASM.  
> Si vous souhaitez tout de même coder en StegASM, lisez attentivement cette documentation et inspirez-vous du Tetris existant.

---

### Structure d'un fichier .stegasm

Un fichier `.stegasm` peut être composé de **5 sections** différentes, chacune optionnelle :

| Section         | Rôle                                         |
|-----------------|----------------------------------------------|
| `.text`         | Code du programme (instructions + labels)    |
| `.data`         | Variables initialisées                       |
| `.file`         | Fichiers encodés directement dans le binaire |
| `.sub_textures` | Sous-textures issues de spritesheets         |
| `.import`       | Import d'autres fichiers `.stegasm`          |

---

#### Section .text

C'est le **coeur** de votre programme. L'exécution débute à la **première instruction** de cette section.  
Elle est composée d'une suite d'**instructions StegASM** et de **labels**.

```asm
section .text
    LOAD_8 R1, 10   ; Charge 10 dans R1
    LOAD_8 R2, 20   ; Charge 20 dans R2
    ADD R3, R1, R2  ; R3 = R1 + R2 (= 30)
    DISPLAY_N R3    ; Affiche 30
    HALT            ; Arrête le programme proprement
```

---

#### Section .data

Cette section contient toutes les **variables initialisées** du programme.  
Chaque variable est une liste de mots terminée automatiquement par un `'\0'` (null terminator).

```asm
section .data
    ma_variable DB 5        ; Variable 8 bits initialisée à 5
    ma_string DB "coucou"   ; Chaîne de caractères
    mix DB "co" 23 47 "a"   ; Un mix des deux
```

En mémoire, cela donne :

| Variable      | Byte 1 | Byte 2 | Byte 3 | Byte 4 | Byte 5 | Byte 6 | Byte 7 |
|---------------|--------|--------|--------|--------|--------|--------|--------|
| `ma_variable` | 5      | `\0`   |        |        |        |        |        |
| `ma_string`   | `'c'`  | `'o'`  | `'u'`  | `'c'`  | `'o'`  | `'u'`  | `'\0'` |
| `mix`         | `'c'`  | `'o'`  | 23     | 47     | `'a'`  | `'\0'` |        |

##### Types de variables

| Type | Taille d'un mot | Déclaration        |
|------|-----------------|--------------------|
| `DB` | 8 bits          | `ma_var DB 42`     |
| `DW` | 16 bits         | `ma_var DW 1000`   |
| `DD` | 32 bits         | `ma_var DD 100000` |

Le type change la **taille de chaque mot** stocké en mémoire. Il est important de choisir le bon type selon l'étendue des valeurs à stocker.

##### Utilisation des variables

Les variables s'utilisent via leur **adresse**. Deux syntaxes existent :

- `[ma_variable]` : charge la **valeur** contenue à l'adresse de `ma_variable`
- `ma_variable` : charge **l'adresse** de `ma_variable` elle-même (utile pour les chaînes et les tableaux)

```asm
section .data
    compteur DW 10
    message DB "hello"

section .text
    LOAD_16 R1, [compteur]  ; R1 = 10 (valeur à l'adresse)
    DISPLAY_N R1            ; Affiche 10

    LOAD_32 R1, message     ; R1 = adresse de "message"
    ADD R1, 1               ; R1 pointe sur le 2e caractère
    DISPLAY_C R1            ; Affiche 'e'
```

---

#### Section .file

La section `.file` permet d'**encoder des fichiers directement dans le binaire** final.  
Chaque fichier est associé à un **descripteur** (identifiant symbolique) utilisable par les instructions de la VM (textures, polices...).

```asm
section .files
    icon "examples/tetris/ressources/icon.png" // Icone d'une application
    tetrisNesFont "examples/tetris/ressources/tetris-nes-front.ttf" // Fichier de font
    backgroundImage "examples/tetris/ressources/background.png" // Image de fond
```

Ces descripteurs peuvent ensuite être passés directement à certaines instructions :

```asm
section .text
    WINDOW_SET_ICON icon
    WINDOW_SET_FONT tetrisNesFont
    WINDOW_DRAW_TEXTURE R0 R1 backgroundImage ; (ici R0 et R1 sont des coordonnées)
```

---

#### Section .sub_textures

La section `.sub_textures` permet de **découper nativement une spritesheet** en sous-textures réutilisables.  
Chaque sous-texture est définie par son fichier d'origine et un rectangle `(X, Y, W, H)`.

```asm
section .files
    spritesheet "./mon_spritesheet.png" ; Image du spritesheet

section .sub_textures
    player_idle spritesheet 0 0 16 16
    player_run spritesheet 16 0 16 16
    enemy spritesheet 0 16 16 16
```

Chaque sous-texture reçoit un **descripteur** indépendant et peut être utilisée comme une texture à part entière dans les instructions `WINDOW_DRAW_TEXTURE`, sans qu'une image supplémentaire soit stockée dans le binaire.

---

#### Section .import

La section `.import` permet de **découper un programme** en plusieurs fichiers `.stegasm` et de les réunir à la compilation.

```asm
section .import
    "examples/tetris/utils/number_in_string.stegasm"
    "examples/tetris/game/lines.stegasm"
    "examples/tetris/game/score.stegasm"
```

> ⚠️ Les chemins d'import sont **relatifs à la position d'éxécution de la commande de compilation**.

---

### Registres

StegVM expose **32 registres** généraux, nommés de `R0` à `R31`.  
Ils sont interchangeables : aucun registre n'a de rôle réservé imposé par la VM.  
Par convention, il est conseillé d'utiliser les registres de cette manière (mais cela n'est pas obligatoire) :   
- R0 -> valeur de retour d'une fonction
- R1-R6 -> Paramètres des fonctions

> Le compilateur Steg utilise les registres de cette manière ! :)

```asm
LOAD_8 R0, 42       ; R0 = 42
LOAD_16 R1, 1000    ; R1 = 1000
ADD R2, R0, R1      ; R2 = R0 + R1
```

> ⚠️ Les registres ne sont pas sauvegardés automatiquement lors d'un `CALL`. Si une fonction modifie un registre que vous utilisez, c'est à vous de le sauvegarder (avec un PUSH/POP par exemple).

---

### Labels et sauts

Un **label** est un identifiant suivi de `:` qui marque une position dans le code.  
Il sert de cible aux instructions de saut (`JMP`, `JE`, `JNE`, `JB`...).

```asm
section .text
    LOAD_8 R0, 0

boucle:
    ADD R0, 1
    CMP R0, 10
    JB boucle       ; Saute à "boucle" tant que R0 < 10

    DISPLAY_N R0    ; Affiche 10
    HALT
```

Les labels fonctionnent également avec `CALL` / `RET` pour structurer le code en **fonctions** :

```asm
section .data
    msg DB "Bonjour !"

section .text
    CALL afficher_bonjour   ; -> Saute à "afficher_bonjour"
    HALT

afficher_bonjour:
    LOAD_32 R0, msg
    DISPLAY_STR R0
    RET                     ; Retourne à l'instruction juste après "CALL 'afficher_bonjour'"
```

---

### Commentaires

Les commentaires commencent par `;` et s'étendent jusqu'à la fin de la ligne.

```asm
LOAD_8 R0, 5 ; Ceci est un commentaire
; Ligne entière en commentaire
```

---

### Liste des opérandes

La liste complète des instructions disponibles (opérandes, types acceptés, comportements) est disponible dans la **[documentation développeur - Liste des opérandes](#liste-des-opérandes-stegasm)**.

---

## Documentation Développeur

---

### Implémentation

StegASM est entièrement développé en **C++ from-scratch**. La seule librairie externe utilisée est [raylib](https://github.com/raysan5/raylib) pour la gestion des fenêtres.  
L'implémentation se trouve dans le dossier [stegasm/src](../stegasm/src).

Les fichiers et dossiers importants :

| Fichier / Dossier                                                   | Rôle                                                 |
|---------------------------------------------------------------------|------------------------------------------------------|
| [`instructions.h`](../stegasm/src/instructions.h)                   | Définition de **toutes** les instructions StegASM    |
| [`interpreter/`](../stegasm/src/interpreter)                        | Implémentation de StegVM                             |
| [`interpreter/Vm.cpp`](../stegasm/src/interpreter/Vm.cpp)           | Boucle principale de la VM                           |
| [`assembler/`](../stegasm/src/assembler)                            | Implémentation de l'assembleur (StegASM -> ByteCode) |
| [`assembler/Assembler.cpp`](../stegasm/src/assembler/Assembler.cpp) | Base de l'assembleur                                 |

---

### Compilation

#### Format du fichier binaire

Le fichier binaire généré par l'assembleur est composé de plusieurs sections écrites **dans l'ordre suivant** :

1. Variables (`.data`)
2. Fichiers (`.file`)
3. Sous-textures (`.sub_textures`)
4. Instructions (`.text`)

---

##### Les variables

Les variables permettent au développeur d'abstraire des adresses mémoire. Chaque variable possède une valeur par défaut composée d'un ou plusieurs mots, terminée automatiquement par un mot vide (`'\0'`).

**Encodage :**

```
uint32 -> nombre de variables (X)
X fois -> struct variable
```

`struct variable` :

```
uint8 -> variable_flag
uint16 -> nombre de mots dans la variable (M)
M fois -> <taille_variable> (valeur initiale de chaque mot)
```

`variable_flag` :

```cpp
enum VariableFlag {
    DATA_UINT8 = 0b01000000, // DB
    DATA_UINT16 = 0b10000000, // DW
    DATA_UINT32 = 0b11000000 // DD
};
```

---

##### Les fichiers

Les fichiers sont des données binaires stockées directement dans le bytecode.  
Seuls le **descripteur** (identifiant numérique), le **contenu brut** du fichier et l'extention du fichier sont encodés - le nom ou le chemin ne sont pas conservés.

**Encodage :**

```
uint32 -> nombre de fichiers (X)
X fois -> struct fichier
```

`struct fichier` :

```
uint16 -> descripteur
uint8 -> nombre de caractères dans l'extension (E)
E fois -> uint8 (extension du fichier)
uint32 -> nombre de mots dans le fichier (M)
M fois -> uint8 (bytecode du fichier)
```

---

##### Les sub_textures

Les sous-textures permettent d'encoder nativement des **spritesheets** dans le binaire.  
Elles définissent des sections d'un fichier image considérées comme une texture indépendante, référençable via un descripteur - sans stocker d'image supplémentaire dans le bytecode.

**Encodage :**

```
uint32 -> nombre de sub_textures (X)
X fois -> struct subtexture
```

`struct subtexture` :

```
uint16 -> descripteur du fichier d'origine
uint16 -> descripteur de la sub_texture (C'est donc un nouveau descripteur)
uint16 -> X (coordonnée X dans l'image d'origine)
uint16 -> Y (coordonnée Y dans l'image d'origine)
uint16 -> W (largeur dans l'image d'origine)
uint16 -> H (hauteur dans l'image d'origine)
```

---

##### Les instructions

Les instructions sont écrites les unes après les autres, dans l'ordre de la section `.text`.  
Chaque instruction est **alignée sur 32 bits** selon l'encodage suivant :

| Champ              | Taille | Position   |
|--------------------|--------|------------|
| Opération          | 8 bits | bits 1–8   |
| Handler Number     | 2 bits | bits 9–10  |
| RegX(1) is address | 1 bit  | bit 11     |
| RegX(1)            | 5 bits | bits 12–16 |
| RegX(2) is address | 1 bit  | bit 17     |
| RegX(2)            | 5 bits | bits 18–22 |
| RegX(3) is address | 1 bit  | bit 23     |
| RegX(3)            | 5 bits | bits 24–28 |
| number_of_registry | 2 bits | bits 29–30 |
| data_type          | 2 bits | bits 31–32 |

**`data_type`** indique si un second bloc de 32 bits suit l'instruction pour encoder une donnée immédiate :

| Valeur | Signification                                         |
|--------|-------------------------------------------------------|
| `00`   | `NO_DATA` - pas de données supplémentaires            |
| `01`   | `Data is value` - la donnée est une valeur immédiate  |
| `10`   | `Data is address` - la donnée est une adresse mémoire |

Si `data_type != NO_DATA`, un **second bloc de 32 bits** suit immédiatement pour stocker la donnée :

```
| data      |
| 32 bits   |
| bits 1–32 |
```

---

### Liste des opérandes StegASM

#### Types d'opérandes

| Type       | Syntaxe                        | Description                                                            |
|------------|--------------------------------|------------------------------------------------------------------------|
| `reg`      | `R0`                           | Valeur directe du registre - ne peut pas être déréférencé              |
| `reg_both` | `R0` ou `[R0]`                 | Valeur du registre **ou** valeur à l'adresse contenue dans le registre |
| `imm`      | `42` ou `[42]` ou `[ma_var]`   | Valeur immédiate ou adresse immédiate                                  |
| `val`      | `R0`, `[R0]`, `42`, `[ma_var]` | Raccourci pour `reg_both` **ou** `imm`                                 |

> `(val)` indique un opérande **optionnel**. Quand il est absent, l'instruction utilise op1 comme destination et source.

---

#### Chargement / Stockage

| Mnémonique  | Opérande 1 | Opérande 2  | Opérande 3 | Description                                                |
|-------------|------------|-------------|------------|------------------------------------------------------------|
| `LOAD_32`   | `reg`      | `val`       | -          | `op1 = op2` (32 bits)                                      |
| `LOAD_16`   | `reg`      | `val`       | -          | `op1 = op2` (16 bits)                                      |
| `LOAD_8`    | `reg`      | `val`       | -          | `op1 = op2` (8 bits)                                       |
| `STORE_32`  | `reg_both` | `reg / imm` | -          | `[op1] = op2` (32 bits)                                    |
| `STORE_16`  | `reg_both` | `reg / imm` | -          | `[op1] = op2` (16 bits)                                    |
| `STORE_8`   | `reg_both` | `reg / imm` | -          | `[op1] = op2` (8 bits)                                     |
| `EXTEND_8`  | `reg`      | `reg_both`  | -          | `op1 = sign_extend_8(op2)` - étend signe de 8 -> 32 bits   |
| `EXTEND_16` | `reg`      | `reg_both`  | -          | `op1 = sign_extend_16(op2)` - étend signe de 16 -> 32 bits |
| `TRUNC_8`   | `reg`      | `reg_both`  | -          | `op1 = op2 & 0xFF` - tronque à 8 bits                      |
| `TRUNC_16`  | `reg`      | `reg_both`  | -          | `op1 = op2 & 0xFFFF` - tronque à 16 bits                   |
| `ALOC`      | `reg`      | `val`       | -          | Alloue `op2` octets en mémoire, adresse du bloc dans `op1` |
| `FREE`      | `reg`      | -           | -          | Libère le bloc alloué à l'adresse `op1`                    |
| `PUSH`      | `reg`      | -           | -          | Empile `op1` sur la pile                                   |
| `POP`       | `reg`      | -           | -          | Dépile la valeur du sommet dans `op1`                      |

---

#### Arithmétique entière

| Mnémonique     | Opérande 1 | Opérande 2 | Opérande 3 | Description                                                                   |
|----------------|------------|------------|------------|-------------------------------------------------------------------------------|
| `ADD`          | `reg`      | `val`      | `(val)`    | `op1 = op1 + op2` ou `op1 = op2 + op3`                                        |
| `SUB`          | `reg`      | `val`      | `(val)`    | `op1 = op1 - op2` ou `op1 = op2 - op3`                                        |
| `MUL`          | `reg`      | `val`      | `(val)`    | `op1 = op1 * op2` ou `op1 = op2 * op3` (non signé)                            |
| `SMUL`         | `reg`      | `val`      | `(val)`    | `op1 = op1 * op2` ou `op1 = op2 * op3` (signé)                                |
| `DIV`          | `reg`      | `val`      | `(val)`    | `op1 = op1 / op2` ou `op1 = op2 / op3` (non signé)                            |
| `SDIV`         | `reg`      | `val`      | `(val)`    | `op1 = op1 / op2` ou `op1 = op2 / op3` (signé)                                |
| `MOD`          | `reg`      | `val`      | `(val)`    | `op1 = op1 % op2` ou `op1 = op2 % op3`                                        |
| `MIN`          | `reg`      | `val`      | `(val)`    | `op1 = min(op1, op2)` ou `op1 = min(op2, op3)`                                |
| `MAX`          | `reg`      | `val`      | `(val)`    | `op1 = max(op1, op2)` ou `op1 = max(op2, op3)`                                |
| `NOT`          | `reg`      | `reg_both` | -          | `op1 = !op2` (NOT logique)                                                    |
| `AND`          | `reg`      | `val`      | `(val)`    | `op1 = op1 & op2` ou `op1 = op2 & op3` (ET bit à bit)                         |
| `OR`           | `reg`      | `val`      | `(val)`    | `op1 = op1 \| op2` ou `op1 = op2 \| op3` (OU bit à bit)                       |
| `XOR`          | `reg`      | `val`      | `(val)`    | `op1 = op1 ^ op2` ou `op1 = op2 ^ op3` (XOR bit à bit)                        |
| `BIT_NOT`      | `reg`      | `val`      | -          | `op1 = ~op2` (NOT bit à bit)                                                  |
| `SHIFT_LEFT`   | `reg`      | `val`      | `(val)`    | `op1 = op1 << op2` ou `op1 = op2 << op3`                                      |
| `SHIFT_RIGHT`  | `reg`      | `val`      | `(val)`    | `op1 = op1 >> op2` ou `op1 = op2 >> op3` (non signé)                          |
| `SSHIFT_RIGHT` | `reg`      | `val`      | `(val)`    | `op1 = op1 >> op2` ou `op1 = op2 >> op3` (signé, propagation du bit de signe) |
| `RAND`         | `reg`      | -          | -          | `op1 = nombre aléatoire 32 bit`                                               |

---

#### Arithmétique flottante

| Mnémonique | Opérande 1 | Opérande 2 | Opérande 3 | Description                                    |
|------------|------------|------------|------------|------------------------------------------------|
| `FADD`     | `reg`      | `val`      | `(val)`    | `op1 = op1 + op2` ou `op1 = op2 + op3`         |
| `FSUB`     | `reg`      | `val`      | `(val)`    | `op1 = op1 - op2` ou `op1 = op2 - op3`         |
| `FMUL`     | `reg`      | `val`      | `(val)`    | `op1 = op1 * op2` ou `op1 = op2 * op3`         |
| `FDIV`     | `reg`      | `val`      | `(val)`    | `op1 = op1 / op2` ou `op1 = op2 / op3`         |
| `FMOD`     | `reg`      | `val`      | `(val)`    | `op1 = op1 % op2` ou `op1 = op2 % op3`         |
| `FMIN`     | `reg`      | `val`      | `(val)`    | `op1 = min(op1, op2)` ou `op1 = min(op2, op3)` |
| `FMAX`     | `reg`      | `val`      | `(val)`    | `op1 = max(op1, op2)` ou `op1 = max(op2, op3)` |
| `FNOT`     | `reg`      | `reg_both` | -          | `op1 = !op2` (NOT logique flottant)            |
| `FSIN`     | `reg`      | `val`      | -          | `op1 = sin(op2)` (radians)                     |
| `FCOS`     | `reg`      | `val`      | -          | `op1 = cos(op2)` (radians)                     |
| `FTAN`     | `reg`      | `val`      | -          | `op1 = tan(op2)` (radians)                     |
| `FATAN`    | `reg`      | `val`      | -          | `op1 = atan(op2)` (radians)                    |
| `FSQRT`    | `reg`      | `val`      | -          | `op1 = sqrt(op2)`                              |
| `FABS`     | `reg`      | `val`      | -          | `op1 = abs(op2)`                               |
| `FDEG2RAD` | `reg`      | `val`      | -          | `op1 = op2 * π / 180`                          |
| `FRAD2DEG` | `reg`      | `val`      | -          | `op1 = op2 * 180 / π`                          |

##### Conversion entier <-> flottant

| Mnémonique | Opérande 1 | Opérande 2 | Opérande 3 | Description                                        |
|------------|------------|------------|------------|----------------------------------------------------|
| `ITOF`     | `reg`      | `reg_both` | -          | `op1 = (float) op2` (entier signé -> flottant)     |
| `UTOF`     | `reg`      | `reg_both` | -          | `op1 = (float) op2` (entier non signé -> flottant) |
| `FTOI`     | `reg`      | `reg_both` | -          | `op1 = (int) op2` (flottant -> entier signé)       |
| `FTOU`     | `reg`      | `reg_both` | -          | `op1 = (uint) op2` (flottant -> entier non signé)  |

---

#### Comparaison et sauts conditionnels

> `CMP`/`FCMP` mettent à jour les flags internes de la VM. Les instructions de saut se basent sur le dernier `CMP`/`FCMP` exécuté.

| Mnémonique | Opérande 1   | Opérande 2 | Opérande 3 | Description                                                   |
|------------|--------------|------------|------------|---------------------------------------------------------------|
| `CMP`      | `reg_both`   | `val`      | -          | Compare `op1` et `op2` (entiers), met à jour les flags        |
| `FCMP`     | `reg_both`   | `val`      | -          | Compare `op1` et `op2` (flottants), met à jour les flags      |
| `JMP`      | `imm`        | -          | -          | Saut inconditionnel vers `op1` (label)                        |
| `JE`       | `imm`        | -          | -          | Saut si `op1 == op2` (dernier CMP)                            |
| `JNE`      | `imm`        | -          | -          | Saut si `op1 != op2`                                          |
| `JA`       | `imm`        | -          | -          | Saut si `op1 > op2` (non signé)                               |
| `JSA`      | `imm`        | -          | -          | Saut si `op1 > op2` (signé)                                   |
| `JB`       | `imm`        | -          | -          | Saut si `op1 < op2` (non signé)                               |
| `JSB`      | `imm`        | -          | -          | Saut si `op1 < op2` (signé)                                   |
| `CALL`     | `imm`        | -          | -          | Appel de fonction à `op1` (label), empile l'adresse de retour |
| `RET`      | -            | -          | -          | Retour de fonction, dépile l'adresse de retour                |
| `HALT`     | `(reg_both)` | -          | -          | Arrête le programme, code de sortie optionnel `op1`           |

---

#### Affichage et debug

> Ces instructions écrivent dans la **sortie standard**

| Mnémonique    | Opérande 1 | Opérande 2 | Opérande 3 | Description                                                                                                                                                                                                                                                          |
|---------------|------------|------------|------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `DISPLAY_N`   | `reg_both` | -          | -          | Affiche `op1` comme entier non signé                                                                                                                                                                                                                                 |
| `DISPLAY_SN`  | `reg_both` | -          | -          | Affiche `op1` comme entier signé                                                                                                                                                                                                                                     |
| `DISPLAY_F`   | `reg_both` | -          | -          | Affiche `op1` comme flottant                                                                                                                                                                                                                                         |
| `DISPLAY_C`   | `reg_both` | -          | -          | Affiche le caractère dont le code ASCII est `op1`                                                                                                                                                                                                                    |
| `DISPLAY_B`   | `reg_both` | -          | -          | Affiche `op1` comme booléen (`0` / `1`)                                                                                                                                                                                                                              |
| `DISPLAY_STR` | `reg_both` | -          | -          | Affiche la chaîne null-terminée à l'adresse `op1`                                                                                                                                                                                                                    |
| `DEBUG_R`     | -          | -          | -          | Affiche le contenu de tous les registres                                                                                                                                                                                                                             |
| `DEBUG_M`     | -          | -          | -          | Affiche le contenu de la mémoire                                                                                                                                                                                                                                     |
| `DEBUG_T`     | `val`      | -          | -          | 1er appel : Elle lance le chrono pour l'ID choisi. ;<br/>Appels suivants : Elle calcule et affiche le temps écoulé (le Delta) depuis le dernier passage, puis redémarre le chrono.<br/>Elle permet par éxemple de calcules le temps de chaque itération d'une boucle |

---

#### Fenêtre

##### Création et configuration

| Mnémonique                 | Opérande 1 | Opérande 2 | Opérande 3 | Description                                                         |
|----------------------------|------------|------------|------------|---------------------------------------------------------------------|
| `WINDOW_CREATE`            | `reg_both` | `reg_both` | `val`      | Crée une fenêtre de taille `op1` x `op2`, titre `op3`               |
| `WINDOW_CLOSE`             | -          | -          | -          | Ferme la fenêtre                                                    |
| `WINDOW_SHOULD_CLOSE`      | `reg`      | -          | -          | `op1 = 1` si fermeture demandée (croix / ALT+F4), sinon `0`         |
| `WINDOW_SET_VIEWPORT_SIZE` | `reg_both` | `reg_both` | -          | Définit la taille du viewport à `op1` x `op2`                       |
| `WINDOW_DISABLE_VIEWPORT`  | `reg_both` | `reg_both` | -          | Supprime le viewport précédement définit                            |
| `WINDOW_TOGGLE_FULLSCREEN` | -          | -          | -          | Bascule le mode plein écran                                         |
| `WINDOW_SET_TARGET_FPS`    | `val`      | -          | -          | Fixe la limite de FPS à `op1`                                       |
| `WINDOW_GET_DELTA`         | `reg`      | -          | -          | `op1 = delta time` depuis la dernière frame (flottant, en secondes) |
| `WINDOW_SET_ICON`          | `val`      | -          | -          | Définit l'icône de la fenêtre (descripteur de fichier image)        |
| `WINDOW_POOL`              | -          | -          | -          | Pompe la file d'événements système                                  |

##### Dessin

| Mnémonique                        | Opérande 1 | Opérande 2 | Opérande 3 | Description                                                                |
|-----------------------------------|------------|------------|------------|----------------------------------------------------------------------------|
| `WINDOW_CLEAR`                    | `reg_both` | `reg_both` | `reg_both` | Efface la fenêtre avec la couleur `(op1, op2, op3)` en RGB                 |
| `WINDOW_PRESENT`                  | -          | -          | -          | Affiche le frame courant (swap buffer)                                     |
| `WINDOW_DRAW_TEXT`                | `reg_both` | `reg_both` | `val`      | Dessine la chaîne `op3` à la position `(op1, op2)`                         |
| `WINDOW_DRAW_TEXTURE`             | `reg_both` | `reg_both` | `val`      | Dessine la texture `op3` à la position `(op1, op2)`                        |
| `WINDOW_SET_TEXT_SIZE`            | `val`      | -          | -          | Définit la taille du texte                                                 |
| `WINDOW_SET_TEXT_COLOR`           | `reg_both` | `reg_both` | `reg_both` | Définit la couleur du texte en RGB `(op1, op2, op3)`                       |
| `WINDOW_SET_FONT`                 | `val`      | -          | -          | Définit la police (descripteur de fichier de police)                       |
| `WINDOW_SET_TEXTURE_COLOR_MASK`   | `reg_both` | `reg_both` | `reg_both` | Applique un masque de couleur RGB `(op1, op2, op3)` aux textures dessinées |
| `WINDOW_RESET_TEXTURE_COLOR_MASK` | -          | -          | -          | Réinitialise le masque de couleur des textures                             |

##### Framebuffer

| Mnémonique                           | Opérande 1 | Opérande 2 | Opérande 3 | Description                                                         |
|--------------------------------------|------------|------------|------------|---------------------------------------------------------------------|
| `WINDOW_TEXTURE_FRAMEBUFFER_CREATE`  | `reg`      | `reg_both` | `reg_both` | Crée un framebuffer de taille `op2` x `op3`, descripteur dans `op1` |
| `WINDOW_TEXTURE_FRAMEBUFFER_ADDRESS` | `reg`      | `reg_both` | -          | `op1 = adresse` des pixels du framebuffer `op2`                     |
| `WINDOW_TEXTURE_FRAMEBUFFER_SYNC`    | `reg_both` | -          | -          | Synchronise le framebuffer `op1` vers la texture GPU                |
| `WINDOW_TEXTURE_FRAMEBUFFER_DRAW`    | `reg_both` | `reg_both` | `reg_both` | Dessine le framebuffer `op3` à la position `(op1, op2)`             |

##### Entrées clavier

| Mnémonique           | Opérande 1 | Opérande 2 | Opérande 3 | Description                                                   |
|----------------------|------------|------------|------------|---------------------------------------------------------------|
| `WINDOW_KEY_PRESSED` | `reg`      | `val`      | -          | `op1 = 1` si la touche `op2` vient d'être pressée (événement) |
| `WINDOW_KEY_DOWN`    | `reg`      | `val`      | -          | `op1 = 1` si la touche `op2` est maintenue enfoncée           |

##### Entrées souris

| Mnémonique                     | Opérande 1 | Opérande 2 | Opérande 3 | Description                                                  |
|--------------------------------|------------|------------|------------|--------------------------------------------------------------|
| `WINDOW_MOUSE_X`               | `reg`      | -          | -          | `op1 = position X` de la souris                              |
| `WINDOW_MOUSE_Y`               | `reg`      | -          | -          | `op1 = position Y` de la souris                              |
| `WINDOW_MOUSE_DELTA_X`         | `reg`      | -          | -          | `op1 = déplacement X` de la souris depuis la dernière frame  |
| `WINDOW_MOUSE_DELTA_Y`         | `reg`      | -          | -          | `op1 = déplacement Y` de la souris depuis la dernière frame  |
| `WINDOW_MOUSE_WHEEL_DELTA`     | `reg`      | -          | -          | `op1 = défilement` de la molette depuis la dernière frame    |
| `WINDOW_MOUSE_BUTTON_PRESSED`  | `reg`      | `val`      | -          | `op1 = 1` si le bouton `op2` vient d'être pressé (événement) |
| `WINDOW_MOUSE_BUTTON_DOWN`     | `reg`      | `val`      | -          | `op1 = 1` si le bouton `op2` est maintenu enfoncé            |
| `WINDOW_MOUSE_BUTTON_RELEASED` | `reg`      | `val`      | -          | `op1 = 1` si le bouton `op2` vient d'être relâché            |
| `WINDOW_HIDE_CURSOR`           | -          | -          | -          | Cache le curseur de la souris                                |
| `WINDOW_SHOW_CURSOR`           | -          | -          | -          | Affiche le curseur de la souris                              |
| `WINDOW_DISABLE_CURSOR`        | -          | -          | -          | Désactive et verrouille le curseur                           |
| `WINDOW_ENABLE_CURSOR`         | -          | -          | -          | Réactive le curseur                                          |

---

#### Image

| Mnémonique         | Opérande 1 | Opérande 2 | Opérande 3 | Description                                                                        |
|--------------------|------------|------------|------------|------------------------------------------------------------------------------------|
| `MAP_IMAGE`        | `reg`      | `val`      | -          | Mappe le fichier image `op2` en mémoire, adresse dans `op1` ; Chaque pixels uint32 |
| `MAP_IMAGE_SIZE_X` | `reg`      | -          | -          | `op1 = largeur` de la dernière image mappée                                        |
| `MAP_IMAGE_SIZE_Y` | `reg`      | -          | -          | `op1 = hauteur` de la dernière image mappée                                        |

---

#### Fichiers

##### Gestion des fichiers

| Mnémonique             | Opérande 1 | Opérande 2 | Opérande 3 | Description                                                               |
|------------------------|------------|------------|------------|---------------------------------------------------------------------------|
| `FILE_OPEN`            | `reg`      | `val`      | -          | Ouvre le fichier au chemin `op2`, descripteur dans `op1`                  |
| `FILE_CREATE`          | `reg`      | `val`      | -          | Crée le fichier au chemin `op2`, descripteur dans `op1`                   |
| `FILE_SAVE`            | `reg`      | -          | -          | Sauvegarde les modifications du fichier `op1` sur le disque               |
| `FILE_DELETE`          | `reg`      | -          | -          | Supprime le fichier `op1`                                                 |
| `FILE_CLOSE`           | `reg`      | -          | -          | Ferme le fichier `op1`                                                    |
| `FILE_GET_SIZE`        | `reg`      | `reg_both` | -          | `op1 = taille` en octets du fichier `op2`                                 |
| `FILE_MAP`             | `reg`      | `reg_both` | -          | Mappe entièrement le fichier `op2` en mémoire, adresse dans `op1`         |
| `FILE_MAP_FROM_CURSOR` | `reg`      | `reg_both` | `reg_both` | Mappe `op3` octets depuis le curseur du fichier `op2`, adresse dans `op1` |
| `FILE_CLEAR_DATA`      | `reg`      | -          | -          | Vide le contenu du fichier `op1`                                          |

##### Curseur

| Mnémonique          | Opérande 1 | Opérande 2 | Opérande 3 | Description                                                |
|---------------------|------------|------------|------------|------------------------------------------------------------|
| `FILE_RESET_CURSOR` | `reg`      | -          | -          | Replace le curseur au début du fichier `op1`               |
| `FILE_SEEK_CURSOR`  | `reg`      | `reg`      | -          | Déplace le curseur du fichier `op1` à l'offset `op2`       |
| `FILE_GET_CURSOR`   | `reg`      | `reg`      | -          | `op1 = position` courante du curseur dans le fichier `op2` |

##### Lecture

| Mnémonique                       | Opérande 1 | Opérande 2 | Opérande 3 | Description                                                         |
|----------------------------------|------------|------------|------------|---------------------------------------------------------------------|
| `FILE_READ_BYTE`                 | `reg`      | `reg`      | -          | `op1 = octet` lu depuis le curseur du fichier `op2`                 |
| `FILE_READ_BYTE_AT`              | `reg`      | `reg_both` | `val`      | `op1 = octet` lu à l'offset `op3` du fichier `op2`                  |
| `FILE_READ_WORD`                 | `reg`      | `reg`      | -          | `op1 = mot 16 bits` lu depuis le curseur (big-endian)               |
| `FILE_READ_WORD_AT`              | `reg`      | `reg_both` | `val`      | `op1 = mot 16 bits` à l'offset `op3` (big-endian)                   |
| `FILE_READ_WORD_LITTLE`          | `reg`      | `reg`      | -          | `op1 = mot 16 bits` lu depuis le curseur (little-endian)            |
| `FILE_READ_WORD_LITTLE_AT`       | `reg`      | `reg_both` | `val`      | `op1 = mot 16 bits` à l'offset `op3` (little-endian)                |
| `FILE_READ_DOUBLEWORD`           | `reg`      | `reg`      | -          | `op1 = double mot 32 bits` lu depuis le curseur (big-endian)        |
| `FILE_READ_DOUBLEWORD_AT`        | `reg`      | `reg_both` | `val`      | `op1 = double mot 32 bits` à l'offset `op3` (big-endian)            |
| `FILE_READ_DOUBLEWORD_LITTLE`    | `reg`      | `reg`      | -          | `op1 = double mot 32 bits` lu depuis le curseur (little-endian)     |
| `FILE_READ_DOUBLEWORD_LITTLE_AT` | `reg`      | `reg_both` | `val`      | `op1 = double mot 32 bits` à l'offset `op3` (little-endian)         |
| `FILE_IS_BYTE_REMAINING`         | `reg`      | `reg`      | -          | `op1 = 1` s'il reste au moins 1 octet à lire dans le fichier `op2`  |
| `FILE_IS_WORD_REMAINING`         | `reg`      | `reg`      | -          | `op1 = 1` s'il reste au moins 2 octets à lire dans le fichier `op2` |
| `FILE_IS_DOUBLEWORD_REMAINING`   | `reg`      | `reg`      | -          | `op1 = 1` s'il reste au moins 4 octets à lire dans le fichier `op2` |

##### Écriture et ajout

| Mnémonique                      | Opérande 1 | Opérande 2 | Opérande 3 | Description                                                                |
|---------------------------------|------------|------------|------------|----------------------------------------------------------------------------|
| `FILE_WRITE_BYTE`               | `reg`      | `reg`      | -          | Écrit l'octet `op2` à la position du curseur dans le fichier `op1`         |
| `FILE_WRITE_WORD`               | `reg`      | `reg`      | -          | Écrit le mot 16 bits `op2` à la position du curseur (big-endian)           |
| `FILE_WRITE_WORD_LITTLE`        | `reg`      | `reg`      | -          | Écrit le mot 16 bits `op2` à la position du curseur (little-endian)        |
| `FILE_WRITE_DOUBLEWORD`         | `reg`      | `reg`      | -          | Écrit le double mot 32 bits `op2` à la position du curseur (big-endian)    |
| `FILE_WRITE_DOUBLEWORD_LITTLE`  | `reg`      | `reg`      | -          | Écrit le double mot 32 bits `op2` à la position du curseur (little-endian) |
| `FILE_APPEND_BYTE`              | `reg`      | `reg`      | -          | Ajoute l'octet `op2` en fin du fichier `op1`                               |
| `FILE_APPEND_WORD`              | `reg`      | `reg`      | -          | Ajoute le mot 16 bits `op2` en fin du fichier (big-endian)                 |
| `FILE_APPEND_WORD_LITTLE`       | `reg`      | `reg`      | -          | Ajoute le mot 16 bits `op2` en fin du fichier (little-endian)              |
| `FILE_APPEND_DOUBLEWORD`        | `reg`      | `reg`      | -          | Ajoute le double mot 32 bits `op2` en fin du fichier (big-endian)          |
| `FILE_APPEND_DOUBLEWORD_LITTLE` | `reg`      | `reg`      | -          | Ajoute le double mot 32 bits `op2` en fin du fichier (little-endian)       |

---

#### Horloge

| Mnémonique             | Opérande 1 | Opérande 2 | Opérande 3 | Description                                                                                    |
|------------------------|------------|------------|------------|------------------------------------------------------------------------------------------------|
| `CLOCK_CREATE`         | `reg`      | -          | -          | Crée une horloge et démarre le chronomètre, descripteur dans `op1`                             |
| `CLOCK_DELETE`         | `reg`      | -          | -          | Supprime l'horloge `op1`                                                                       |
| `CLOCK_GET_ELAPSED_MS` | `reg`      | `reg`      | -          | `op1 = temps` écoulé en millisecondes depuis la création / le dernier reset de l'horloge `op2` |
| `CLOCK_GET_ELAPSED_S`  | `reg`      | `reg`      | -          | `op1 = temps` écoulé en secondes (flottant) de l'horloge `op2`                                 |
| `CLOCK_RESET`          | `reg`      | -          | -          | Remet à zéro l'horloge `op1`                                                                   |