# L'assembleur

## Registres et donnée

Cet assembleur auras 6 registres 
R0, R1, R2, R3, R4, R5
Chaqu'un des registre seras sur 16 bits non signé. 

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

| Opération | Reg X(1)  | Reg X(2)  |   data1   | 
|:---------:|:---------:|:---------:|:---------:|
|  8 bits   |  4 bits   |  4 bits   |  16 bits  |
|  bit 1-8  | bit 9 -12 | bit 13-16 | bit 17-32 |

Certaines opération aurons besoin de plus data ou registre pour fonctionner ;
elles seront alors suivis par un second block uint32 sous ce format :

| Reg X(3) | Reg X(4) | Reg X(5) | Reg X(6) |   data2   |
|:--------:|:--------:|:--------:|:--------:|:---------:|
|  4 bits  |  4 bits  |  4 bits  |  4 bits  |  16 bit   |
| bit 1-4  | bit 5-8  | bit 9-12 |  13-16   | bit 17-32 |

Les opérations peuvents donc utiliser au maximum 6 registres et 2 datas.  

### Liste des opérandes
 
Toutes les opération auront des signatures fixe ; cet a dire qu'on connaits leurs nombre de registres et data.  

| Opération  | Codage | Registre(s) et Data(s) |                                                   Description                                                    |
|:----------:|:------:|:----------------------:|:----------------------------------------------------------------------------------------------------------------:|
|   LOADA    |  0x1   |    RegX(1) ; data1     |               Charge une donnée depuis une addresse **hardcoder** (data1) dans le registre RegX(1)               |
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

