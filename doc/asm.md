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

| Opération | Reg X(1) |  flag  | Reg X(2)  |   data1   | 
|:---------:|:--------:|:------:|:---------:|:---------:|
|  8 bits   |  3 bits  | 2 bits |  3 bits   |  16 bits  |
|  bit 1-8  | bit 9-11 | 12-13  | bit 14-16 | bit 17-32 |

Les deux bit de flag permettrons de savoir si data1 et data2 dois être interprété comme une addresse ou comme une valeur.  
Le premier bit serviras à data1 le second à data2. Si le bit est a 0 data devras être interprété comme une valeur, si il est a 1 il seras alors considérés comme une addresse.  

Certaines opération aurons besoin de plus data ou registre pour fonctionner ;
elles seront alors suivis par un second block uint32 sous ce format :

| Reg X(3) | Reg X(4) | Reg X(5) | Reg X(6)  | Reg X(7)  | None  |   data2   |
|:--------:|:--------:|:--------:|:---------:|:---------:|:-----:|:---------:|
|  3 bits  |  3 bits  |  3 bits  |  3 bits   |  3 bits   | 1 bit |  16 bit   |
| bit 1-3  | bit 4-6  | bit 7-9  | bit 10-12 | bit 13-15 |  16   | bit 17-32 |

Les opérations peuvents donc utiliser au maximum 7 registres et 2 datas.  

### Liste des opérandes
 
Toutes les opération auront des signatures fixe ; cet a dire qu'on connaits leurs nombre de registres et data.  

| Opération  | Codage | Registre(s) et Data(s) |                                                   Description                                                    |
|:----------:|:------:|:----------------------:|:----------------------------------------------------------------------------------------------------------------:|
|   LOADA    |  0x1   |    RegX(1) ; data1     |                             Charge une donnée depuis data1 dans le registre RegX(1)                              |
|   LOADR    |  0x2   |   RegX(1) ; RegX(2)    |       Charge une donnée depuis une addresse **stocker dans un registre** RegX(2) dans le registre RegX(1)        |
|   STOREA   |  0x3   |    RegX(1) ; data1     |            Charge une donnée en RAM depuis un registre RegX(1) vers une addresse **hardcoder** data1             |
|   STORER   |  0x4   |   RegX(1) ; RegX(2)    | Charge une donnée en RAM depuis un registre RegX(1) vers une addresse **stocker dans un autre registre** RegX(2) |
|    MOV     |  0x5   |   RegX(1) ; RegX(2)    |                               Déplace la valeurs du registre RegX(1) vers RegX(2)                                |
|    ADD     |  0x6   |   RegX(1) ; RegX(2)    |                        Additionne RegX(1) avec  RegX(2) et stock le résultat dans RegX(1)                        |
|    SUB     |  0x7   |   RegX(1) ; RegX(2)    |                         Soustrait RegX(1) avec RegX(2) et stock le résultat dans RegX(1)                         |
|    JMP     |  0x8   |         data1          |                                     Saute à l'instruction data1 du programme                                     |
|    CMPR    |  0x9   |   RegX(1) ; RegX(2)    |                                       Compare les valeurs de deux registre                                       |
|    CMPA    |  0xA   |    RegX(1) ; data1     |                             Compare la valeurs dans le registre Reg(1) et dans data1                             |
|     JE     |  0xB   |         data1          |                                 saute à l'instruction data1 du programme si A==B                                 |
|    JNE     |  0xC   |         data1          |                                 saute à l'instruction data1 du programme si A!=B                                 |
|     JA     |  0xD   |         data1          |                                 saute à l'instruction data1 du programme si A>B                                  |
|     JB     |  0xE   |         data1          |                                 saute à l'instruction data1 du programme si A<B                                  |
| DISPLAY_N  |  0xF   |        RegX(1)         |                              Affiche le contenu du registre RegX(1) comme un nombre                              |
| DISPLAY_AN |  0x11  |        RegX(1)         |                  Affiche la valeur à l'adresse contenu dans le registre RegX(1) comme un nombre                  |
| DISPLAY_C  |  0x12  |        RegX(1)         |                               Affiche le contenu du registre RegX(1) comme un char                               |
| DISPLAY_AC |  0x13  |        RegX(1)         |                  Affiche la valeur à l'addresse contenu dans le registre  RegX(1) comme un char                  |
| DISPLAY_B  |  0x14  |        RegX(1)         |                             Affiche le contenu du registre RegX(1) comme en binaire                              |
| DISPLAY_AB |  0x15  |        RegX(1)         |                 Affiche la valeur à l'addresse contenu dans le registre RegX(1) comme en binaire                 |
|    HALT    |  0x16  |                        |                 Met fin au programme, la valeurs de retours seras la valeurs enregistrer dans R0                 |

