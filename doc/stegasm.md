# L'assembleur

## Registres et données

Cet assembleur aura 8 registres :
R0, R1, R2, R3, R4, R5, R6, R7.
Chacun de ces registres stockera des valeurs de 16 bits non signé.

Nous avons donc une possibilité de 65 536 adresses en RAM.
Chaque mot en mémoire RAM fera 16 bits.

Toutes les opérations faites sur les registres seront non signées.

## Compilation

### Format du fichier binaire

Ici, chaque section du binaire sera décrite dans son ordre dans le fichier final.

#### Les variables

Les variables seront un moyen pour le développeur d'abstraire des adresses dans le programme. Au lieu d'écrire une adresse, le développeur pourra écrire le nom de la variable.
Toutes les variables auront une valeur par défaut ; cette valeur par défaut pourra prendre plusieurs mots dans la mémoire.

Les variables seront définies dans la section `.data` du fichier assembleur.  
Chaqu'une d'entre elles pourras possédé plusieurs mot en mémoire ce terminant automatiquement par un mot vide ('\0').

##### Encodage des variables

-> uint32 nombre de variables (appelé X)  
-> X fois struct variable

struct variable :
-> uint8 variable_flag
-> uint32 adresse de la variable  
-> uint16 nombre de mots dans la variable (appelé M)  
-> M fois <taille_variable> ; valeurs initiales de chaque mot dans la variable

Avec variable_flag :
````c++
enum VariableFlag {
    DATA_UINT8 = 0b01000000, // DB
    DATA_UINT16 = 0b10000000, // DW
    DATA_UINT32 = 0b11000000 // DD
};
````

#### Les fichiers

Les fichiers sont des données binaires stockées directement dans le ByteCode.  
Le développeur pourra réaliser plusieurs opérations dessus à l'aide de descripteurs.  
Ne seront donc pas stockés le nom du fichier ou son chemin, par exemple.  
Seuls le numéro du descripteur et le ByteCode du fichier seront stockés.  

##### Encodage des fichiers

-> uint32 nombre de fichiers  
-> X fois struct fichier

struct fichier :   
-> uint16 descripteur  
-> uint8 nombre de caractères dans l'extension  
-> X fois uint8 ; extension du fichier  
-> uint32 nombre de mots dans le fichier  
-> M fois uint8 ; ByteCode du fichier

#### Les sub_textures

Les sous-textures permettent d'encoder nativement des spritesheets dans le binaire.
Elles définissent des sections d'un fichier (représentant une image) qui seront considérées comme une texture à part entière.  
Donc chaqu'une des subtextures seront utiliser via un descripteur le tout sans stocker des images supplémentaire dans le bytecode.

##### Encodage des sub_textures

-> uint32 nombre de sub_textures  
-> X fois struct subtexture

struct subtexture :   
-> uint16 descripteur du fichier d'origine  
-> uint16 descripteur de la sub_texture  
-> uint16 X -> coordonnée X dans l'image d'origine  
-> uint16 Y -> coordonnée Y dans l'image d'origine  
-> uint16 W -> largeur dans l'image d'origine  
-> uint16 H -> hauteur dans l'image d'origine


#### Les instructions

Les instructions seront ensuite écrites les unes après les autres ; elles seront écrites dans la section `.text` du fichier assembleur.
Les instructions seront toutes alignées sur 32 bits ; on utilise le codage suivant :

| Opération | Handler Number | RegX(1) is address |  RegX(1)   | RegX(2) is address |  RegX(2)   | RegX(3) is address |  RegX(3)   | number_of_registry | data_type  |
|:---------:|:--------------:|:------------------:|:----------:|:------------------:|:----------:|:------------------:|:----------:|:------------------:|------------|
|  8 bits   |     2 bit      |       1 bit        |   5 bits   |       1 bit        |   5 bits   |       1 bit        |   5 bits   |       2 bits       | 2 bits     |
| bits 1-8  |   bits 9-10    |       bit 11       | bits 12-16 |       bit 17       | bits 18-22 |       bit 23       | bits 24-28 |     bits 29-30     | bits 31-32 |

Avec data_type : 

|    |                 |
|----|-----------------|
| 00 | NO_DATA         |
| 01 | Data is value   |
| 10 | Data is address |

Si data != NO_DATA, alors un second block suit pour stocker la data:

| data      |
|-----------|
| 32 bits   |
| bits 1-32 |


### Liste des opérandes


| Opération                         | Registre(s) et Donnée(s)  |                                                                             Description                                                                             |
|:----------------------------------|:-------------------------:|:-------------------------------------------------------------------------------------------------------------------------------------------------------------------:|
| LOAD                              |         dest, src         |                                                                  Copie le contenu de src dans dest                                                                  |
| STOREA                            |      RegX(1) ; data1      |                                   Enregistre une donnée en RAM depuis un registre RegX(1) vers une adresse **codée en dur** data1                                   |
| STORER                            |     RegX(1) ; RegX(2)     |                         Enregistre une donnée en RAM depuis un registre RegX(1) vers une adresse **stockée dans un autre registre** RegX(2)                         |
| ADD                               |         dest, src         |                                                                              dest+src                                                                               |
| SUB                               |         dest, src         |                                                                              dest-src                                                                               |
| MUL                               |         dest, src         |                                                                              dest*src                                                                               |
| DIV                               |         dest, src         |                                                                              dest/src                                                                               |
| MIN                               |         dest, src         |                                                                           min(dest, src)                                                                            |
| MAX                               |         dest, src         |                                                                           max(dest, src)                                                                            |
| MOD                               |         dest, src         |                                                                             dest % src                                                                              |
| JMP                               |           data1           |                                                              Saute à l'instruction data1 du programme                                                               |
| CMP                               |         dest, src         |                                                     Compare les deux valeurs et stock le résultat dans les flag                                                     |
| JE                                |           data1           |                                                          Saute à l'instruction data1 du programme si A==B                                                           |
| JNE                               |           data1           |                                                          Saute à l'instruction data1 du programme si A!=B                                                           |
| JA                                |           data1           |                                                           Saute à l'instruction data1 du programme si A>B                                                           |
| JLA                               |           data1           |                                      Saute à l'instruction data1 du programme si A>B manière signé (bit de poid fort = signe)                                       |
| JB                                |           data1           |                                                           Saute à l'instruction data1 du programme si A<B                                                           |
| JLB                               |           data1           |                                     Saute à l'instruction data1 du programme si A<B de manière signé (bit de poid fort = signe)                                     |
| EXTEND_8                          |     RegX(1), RegX(2)      |                                        Extend le signe depuis RegX(2) (étant un int8) et sauvegarde le résultat dans RegX(1)                                        |
| EXTEND_16                         |     RegX(1), RegX(2)      |                                       Extend le signe depuis RegX(2) (étant un int16) et sauvegarde le résultat dans RegX(1)                                        |
| TRUNC_8                           |     RegX(1), RegX(2)      |                                             Tronque le signe de regX(2) depuis un int32 vers int8 résultat dans RegX(1)                                             |
| TRUNC_16                          |     RegX(1), RegX(2)      |                                            Tronque le signe de regX(2) depuis un int32 vers int16 résultat dans RegX(1)                                             |
| DISPLAY_N                         |          RegX(1)          |                                                       Affiche le contenu du registre RegX(1) comme un nombre                                                        |
| DISPLAY_C                         |          RegX(1)          |                                                      Affiche le contenu du registre RegX(1) comme un caractère                                                      |
| DISPLAY_B                         |          RegX(1)          |                                                          Affiche le contenu du registre RegX(1) en binaire                                                          |
| HALT                              |                           |                                            Met fin au programme ; la valeur de retour sera la valeur enregistrée dans R0                                            |
| ALOCA                             |      RegX(1), data1       |                                                  Alloue data1 mots et retourne l'adresse dans le registre RegX(1)                                                   |
| ALOCR                             |     RegX(1), RegX(2)      |                                                 Alloue RegX(2) mots et retourne l'adresse dans le registre RegX(1)                                                  |
| FREE                              |          RegX(1)          |                                                            Libère la mémoire allouée à l'adresse RegX(1)                                                            |
 | DEBUG_R                           |                           |                                                                Affiche l'état de tous les registres                                                                 |
| DEBUG_M                           |                           |                                                                    Affiche l'état de la mémoire                                                                     |
| CALL                              |           data1           |                                                   Stocke le pointeur d'instruction dans la pile et saute à data1                                                    |
| RET                               |                           |                                                            Dépile la pile et saute à la valeur récupérée                                                            |
| PUSH                              |          RegX(1)          |                                                      Pousse la valeurs contenus dans le RegX(1) dans la stack                                                       |
| POP                               |          RegX(1)          |                                                          Pop la valeur contenus dans la stack dans RegX(1)                                                          |
| RAND                              |          RegX(1)          |                                                                Met une valeur aléatoire dans RegX(1)                                                                |
| WINDOW_CREATE                     |  RegX(1), RegX(2), data1  |                              Crée une fenêtre de taille RegX(1) x RegX(2) avec comme titre data1 (doit pointer vers une chaîne valide)                              |
| WINDOW_CLOSE                      |                           |                                                                          Ferme la fenêtre                                                                           |
| WINDOW_SET_VIEWPORT_SIZE          |     RegX(1), RegX(2)      | Définit la résolution interne (viewport) utilisée pour le rendu. Le contenu sera redimensionné selon le mode pixel perfect, en fonction de la taille de la fenêtre. |
| WINDOW_DISABLE_VIEWPORT           |                           |                                                                         Retire le viewport                                                                          |
| WINDOW_POOL                       |                           |                                                                       Récupère les événements                                                                       |
| WINDOW_SHOULD_CLOSE               |          RegX(1)          |                         Retourne si l'utilisateur a demandé à fermer la fenêtre (ex : clic sur la croix) ; stocke le résultat dans RegX(1)                          |
| WINDOW_CLEAR                      | RegX(1), RegX(2), RegX(3) |                                   Efface la fenêtre avec la couleur R V B spécifiée ; RegX(1)=Rouge ; RegX(2)=Vert ; RegX(3)=Bleu                                   |
| WINDOW_PRESENT                    |                           |                                             Affiche les éléments dessinés sur la fenêtre depuis le dernier WINDOW_CLEAR                                             |
| WINDOW_KEY_PRESSED                |      RegX(1), data1       |                          Retourne dans RegX(1) si la touche data1 a été pressée [Liste des touches ligne 572](../libs/raylib/src/raylib.h)                          |
| WINDOW_KEY_DOWN                   |      RegX(1), data1       |                    Retourne dans RegX(1) si la touche data1 est actuellement enfoncée [Liste des touches ligne 572](../libs/raylib/src/raylib.h)                    |
| WINDOW_SET_TARGET_FPS             |           data1           |                    Définit le nombre de FPS visé ; le programme attendra X ms après l'appel de WINDOW_PRESENT pour correspondre au nombre de FPS                    |
| WINDOW_SET_TEXT_SIZE              |           data1           |                                                 Définit la taille pour les prochains textes dessinés sur la fenêtre                                                 |
| WINDOW_SET_TEXT_COLOR             | RegX(1), RegX(2), RegX(3) |                                                Définit la couleur pour les prochains textes dessinés sur la fenêtre                                                 |
| WINDOW_SET_FONT                   |           data1           |                                                 Définit la font utilisée dans l'application, descripteur dans data1                                                 |
| WINDOW_DRAW_TEXT                  |  RegX(1), RegX(2), data1  |                                       Dessine un texte à l'adresse data1 sur la fenêtre aux coordonnées x=RegX(1), y=RegX(2)                                        |
| WINDOW_DRAW_TEXTURE               |  RegX(1), RegX(2), data1  |                                            Dessine une texture au descripteur data1 aux coordonnées x=RegX(1), y=RegX(2)                                            |
| WINDOW_SET_TEXTURE_COLOR_MASK     | RegX(1), RegX(2), RegX(3) |                                                       Définit le masque de couleur pour dessiner des textures                                                       |
| WINDOW_RESET_TEXTURE_COLOR_MASK   |                           |                                                              Reset le masque de couleurs des textures                                                               |
| WINDOW_SET_ICON                   |           data1           |                                                            Change l'icône vers le descripteur dans data1                                                            |
| FILE_OPEN                         |      RegX(1), data1       |                                                Ouvre le fichier au chemin data1 et place le descripteur dans RegX(1)                                                |
| FILE_CREATE                       |      RegX(1), data1       |                                              Crée un fichier vide au chemin data1 et place le descripteur dans RegX(1)                                              |
| FILE_SAVE                         |          RegX(1)          |                                                   Sauvegarde le fichier sur le disque ; descripteur dans RegX(1)                                                    |
| FILE_DELETE                       |          RegX(1)          |                               Supprime le fichier sur la machine ; le descripteur devient ensuite invalide ; descripteur dans RegX(1)                               |
| FILE_CLOSE                        |          RegX(1)          |                                            Ferme le fichier ; le descripteur devient invalide ; descripteur dans RegX(1)                                            |
| FILE_RESET_CURSOR                 |          RegX(1)          |                                                  Déplace le curseur de lecture au début ; descripteur dans RegX(1)                                                  |
| FILE_CLEAR_DATA                   |          RegX(1)          |                                                 Vide toutes les données dans le fichier ; descripteur dans RegX(1)                                                  |
| FILE_READ_BYTE                    |     RegX(1), RegX(2)      |                                          Lit un octet dans le fichier et le stocke dans RegX(1) ; descripteur dans RegX(2)                                          |
| FILE_READ_WORD                    |     RegX(1), RegX(2)      |                                      Lit un mot (uint16) dans le fichier et le stocke dans RegX(1) ; descripteur dans RegX(2)                                       |
| FILE_APPEND_BYTE                  |     RegX(1), RegX(2)      |                                      Ajoute un octet à la fin du fichier (LSB du registre RegX(1)) ; descripteur dans RegX(2)                                       |
 | FILE_APPEND_WORD                  |     RegX(1), RegX(2)      |                                            Ajoute un mot (uint16) à la fin du fichier RegX(1) ; descripteur dans RegX(2)                                            |
| FILE_IS_BYTE_REMAINING            |     RegX(1), RegX(2)      |                                    Retourne s'il reste des octets à lire dans le fichier dans RegX(1) ; descripteur dans RegX(2)                                    |
| FILE_IS_WORD_REMAINING            |     RegX(1), RegX(2)      |                                Retourne s'il reste des mots (uint16) à lire dans le fichier dans RegX(1) ; descripteur dans RegX(2)                                 |
| CLOCK_CREATE                      |          RegX(1)          |                                                       Crée une clock et retourne son descripteur dans RegX(1)                                                       |
| CLOCK_DELETE                      |          RegX(1)          |                                                              Supprime la clock au descripteur RegX(1)                                                               |
| CLOCK_GET_ELAPSED_MS              |     RegX(1), RegX(2)      |                                                    Met dans RegX(1) le temps écouler en ms dans la clock RegX(2)                                                    |
| CLOCK_GET_ELAPSED_S               |     RegX(1), RegX(2)      |                                                 Met dans RegX(1) le temps écouler en seconde dans la clock RegX(2)                                                  |
| CLOCK_RESET                       |          RegX(1)          |                                                  Remet le temps écoulé à zero dans la clock au descripteur RegX(1)                                                  |