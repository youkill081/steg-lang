# L'assembleur

## Registres et donnée

Cet assembleur auras 8 registres 
R0, R1, R2, R3, R4, R5, R6, R7
Chaqu'un de ces registre stockeras des valeurs de 16 bits non signé 

Nous avons donc une possibilité de 65536 addresses en RAM. 
Chaque mot en mémoire RAM feras 16 bit.

Toutes les opérations faites sur les registres seront non signé.

## Compilation

### Format du fichier binaire

Ici chaque sections du binaire seras décrites dans leurs ordres dans le fichier finale. 

#### Les variables

Les variables serons un moyen pour le développeur d'abstraire des addresse dans le programmes. Au lieu d'écrire une addresse le développeur pourras écrire le nom de la variable.    
Toutes les variables auront une valeurs par défaut, cette valeurs par défaut pourras prendre plusieurs mot dans la mémoire.

Les variables seront définit dans la section .data du fichier assembleur. 

##### encodage des variables

-> uint32 nombre de variable (appelé X)  
-> X fois struct variable

struct variable :   
-> uint16 addresse de la variable  
-> uint16 nombre de mot dans la variable (appelé M)  
-> M fois uint16 ; valeurs initials de chaque mot dans la variable  

#### Codage des instructions

Les instructions seront ensuite écrites les unes après les autres ; elle seront écrite dans la section .text du fichier assembleur.   
Les instruction serons toutes alignées sur 32 bits ; on utilise le codage suivant :

| Opération | RegX(1)  |  flag  |  RegX(2)  |   data1   | 
|:---------:|:--------:|:------:|:---------:|:---------:|
|  8 bits   |  3 bits  | 2 bits |  3 bits   |  16 bits  |
|  bit 1-8  | bit 9-11 | 12-13  | bit 14-16 | bit 17-32 |

Les deux bit de flag permettrons de savoir si data1 et data2 dois être interprété comme une addresse ou comme une valeur.  
Le premier bit serviras à data1 le second à data2. Si le bit est a 0 data devras être interprété comme une valeur, si il est a 1 il seras alors considérés comme une addresse.  

Certaines opération aurons besoin de plus data ou registre pour fonctionner ;
elles seront alors suivis par un second block uint32 sous ce format :

| RegX(3) | RegX(4) | RegX(5) |  RegX(6)  |  RegX(7)  | None  |   data2   |
|:-------:|:-------:|:-------:|:---------:|:---------:|:-----:|:---------:|
| 3 bits  | 3 bits  | 3 bits  |  3 bits   |  3 bits   | 1 bit |  16 bit   |
| bit 1-3 | bit 4-6 | bit 7-9 | bit 10-12 | bit 13-15 |  16   | bit 17-32 |

Les opérations peuvents donc utiliser au maximum 7 registres et 2 datas.  

### Liste des opérandes
 
Toutes les opération auront des signatures fixe ; cet a dire qu'on connaits leurs nombre de registres et data.  
De plus les op-code seront automatiquement calculer lors de la compilation.  

| Opération             |  Registre(s) et Data(s)   |                                                         Description                                                          |
|:----------------------|:-------------------------:|:----------------------------------------------------------------------------------------------------------------------------:|
| LOADA                 |      RegX(1) ; data1      |                                   Charge une donnée depuis data1 dans le registre RegX(1)                                    |
| LOADR                 |     RegX(1) ; RegX(2)     |             Charge une donnée depuis une addresse **stocker dans un registre** RegX(2) dans le registre RegX(1)              |
| STOREA                |      RegX(1) ; data1      |                  Charge une donnée en RAM depuis un registre RegX(1) vers une addresse **hardcoder** data1                   |
| STORER                |     RegX(1) ; RegX(2)     |       Charge une donnée en RAM depuis un registre RegX(1) vers une addresse **stocker dans un autre registre** RegX(2)       |
| MOV                   |     RegX(1) ; RegX(2)     |                                     Déplace la valeurs du registre RegX(2) vers RegX(1)                                      |
| ADD                   |     RegX(1) ; RegX(2)     |                              Additionne RegX(1) avec  RegX(2) et stock le résultat dans RegX(1)                              |
| SUB                   |     RegX(1) ; RegX(2)     |                               Soustrait RegX(1) avec RegX(2) et stock le résultat dans RegX(1)                               |
| JMP                   |           data1           |                                           Saute à l'instruction data1 du programme                                           |
| CMPR                  |     RegX(1) ; RegX(2)     |                                             Compare les valeurs de deux registre                                             |
| CMPA                  |      RegX(1) ; data1      |                                   Compare la valeurs dans le registre Reg(1) et dans data1                                   |
| JE                    |           data1           |                                       saute à l'instruction data1 du programme si A==B                                       |
| JNE                   |           data1           |                                       saute à l'instruction data1 du programme si A!=B                                       |
| JA                    |           data1           |                                       saute à l'instruction data1 du programme si A>B                                        |
| JB                    |           data1           |                                       saute à l'instruction data1 du programme si A<B                                        |
| DISPLAY_N             |          RegX(1)          |                                    Affiche le contenu du registre RegX(1) comme un nombre                                    |
| DISPLAY_AN            |          RegX(1)          |                        Affiche la valeur à l'adresse contenu dans le registre RegX(1) comme un nombre                        |
| DISPLAY_C             |          RegX(1)          |                                     Affiche le contenu du registre RegX(1) comme un char                                     |
| DISPLAY_AC            |          RegX(1)          |                        Affiche la valeur à l'addresse contenu dans le registre  RegX(1) comme un char                        |
| DISPLAY_B             |          RegX(1)          |                                   Affiche le contenu du registre RegX(1) comme en binaire                                    |
| DISPLAY_AB            |          RegX(1)          |                       Affiche la valeur à l'addresse contenu dans le registre RegX(1) comme en binaire                       |
| HALT                  |                           |                       Met fin au programme, la valeurs de retours seras la valeurs enregistrer dans R0                       |
| ALOCA                 |      RegX(1), data1       |                              Alloues data1 mot et retourne l'addresse dans le registre RegX(1)                               |
| ALOCR                 |     RegX(1), RegX(2)      |                             Alloues RegX(2) mot et retourne l'addresse dans le registre RegX(1)                              |
| FREE                  |          RegX(1)          |                                         Free la mémoire allouer à l'addresse RegX(1)                                         |
| DEBUG_R               |                           |                                             Affiche l'état de tous les registres                                             |
| DEBUG_M               |                           |                                                 Affiche l'état de la mémoire                                                 |
| ADDA                  |      RegX(1), data1       |                        Additionne RegX(1) avec le contenus de data1 ; stock le résultat dans RegX(1)                         |
| CALL                  |           data1           |                                  Stock l'instruction pointeur dans la stack et jump à data1                                  |
| RET                   |                           |                                          Pop la stack et jump à la valeurs récupéré                                          |
| WINDOW_CREATE         |  RegX(1), RegX(1), data1  |          Crée une fenêtre de taille RegX(1) x RegX(2) avec comme titre data1 (dois pointer vers une string valide)           |
| WINDOW_CLOSE          |                           |                                                       Ferme la fenêtre                                                       |
| WINDOW_POOL           |                           |                                                       Pool les events                                                        |
| WINDOW_SHOULD_CLOSE   |          RegX(1)          |      Retourne si l'utilisateur a demander de fermer la fenêtre (ex: clique sur la croix) stock le résutat dans RegX(1)       |
| WINDOW_CLEAR          | RegX(1), RegX(2), RegX(3) |               Clear la fenêtre avec la couleurs R V B spécifiée ; RegX(1)=Rouge ; RegX(2)=Vert ; RegX(3)=Bleu                |
| WINDOW_PRESENT        |                           |                         Affiche les éléments dessiner sur la fenêtre après le WINDOW_CLEAR précédent                         |
| WINDOW_KEY_PRESSED    |      RegX(1), data1       |      Retourne dans RegX(1) si la touche data1 a été préssée [Liste des touches ligne 572](../libs/raylib/src/raylib.h)       |
| WINDOW_KEY_DOWN       |      RegX(1), data1       | Retourne dans RegX(1) si la touche data1 est actuellement appuyée [Liste des touches ligne 572](../libs/raylib/src/raylib.h) |
| WINDOW_SET_TARGET_FPS |           data1           | Définit le nombre de FPS visé ; le programme va attendre X ms après l'appel de WINDOW_PRESENT pour matcher au nombre de FPS  |
| WINDOW_SET_TEXT_SIZE  |           data1           |                              Définit la taille pour les prochains texte dessiné sur la fenêtre                               |
| WINDOW_SET_TEXT_COLOR | RegX(1), RegX(2), RegX(3) |                              Définit la couleur pour les prochains texte dessiné sur la fenêtre                              |
| WINDOW_DRAW_TEXT      |  RegX(1), RegX(2), data1  |                     Dessine un texte à l'addresse data1 sur la fenêtre au coordonnée x=RegX(1) y=RegX(2)                     |