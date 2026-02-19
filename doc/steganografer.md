# Steganografer

Le but de ce programme est de cacher de la donnée dans des images en l'altérant la moins possible.  

## Stockage des pixel 

Les pixels seron définit de cette manière :  

````c++
enum pixel_usage {
  HIDE_SEED, 
  ZERO_READ,
  ONE_READ,
  TWO_READ,
  ALL_READ
}

struct pixel {
  unsigned char red, blue, green;
  pixel_usage usage;
}
````

La structure de pixel_usage permettra de tracker à quoi sert le pixel, voici les valeurs possibles :  
-> HIDE_SEED : Le pixel cache la crée permanent de connaitre la clée pseudo aléatoire.     
-> ZERO_READ :  Le pixel peux stocker de la donnée, mais la donné n'a jamais été lue, la donnée est donc stocker dans le canal rouge   
-> ONE_READ : Le pixel a déjà été lus une fois, la donnée est donc stocker dans le canal bleu  
-> TWO_READ : Le pixel a déjà été lus deux fois, la donnée est donc stocker dans le canal vert  
-> ALL_READ : Le pixel a été lus trois fois, donc il n'y a plus de donné stocker à l'intérieur  

## Lecture/Écriture des pixels

Les pixels seront lus et écrit de la même manière pour qu'ils  soit toujours retrouver de la même ordres.  
Nous partons ici du principe que tous les pixels ont été mis dans un vecteur de pixel chaque pixel étant définit dans la struct "pixel" avec "usage" définit sur "ZERO_READ"

Voici la description de cet algorithme étape par étape 

### Étape 1 : Lecture de la seed pseudo aléatoire (K)

#### Récupération de la taille de la seed

Premièrement la taille de la seed dépend de la taille de l'image.  
Pour calculer la taille de la seen nous feront :  
$$
taille\_seed = \min\big(\lceil \log_2(nombre\_pixel) \rceil, 32\big)
$$

#### Récupération de la seed

Voici l'algorithme pour retrouver la seed :  

##### #1 -> Définir "nombre_pixel" comme seed à notre fonction de génération pseudo-aléatoire  

##### #2 Récupération des X bits 

Premièrement nous définission une variable uint32 padding qui seras égal à 0  
Nous allons faire "taille_seed" fois ces opération :  
1) Génération d'un nombre pseudo-aléatoire N
2) padding += N%nombre_pixel
3) Récupération d'un pixel P = pixels[padding % nombre_pixel]
4) Si usage de P == HIDE_SEED nous faisons padding += 1 et on recommence à l'étape 3  
   Si usage de P == ZERO_READ on continue à l'étape 5  
5) Nous récupérons le bit de poid faible de P sur la couleur rouge, ce bit seras le bit avec le poid le plus fort pas encore récupérer de la seed  
6) Nous mettons usage de P sur HIDE_SEED
7) Nous retournons à l'étape 1

A la suite de cet algorithme nous aurons récupérer X bit avec X <= 32  
Nous avons un cas spécial si taille_seed < 32

###### Dans le cas ou taille_seed < 32

si taille_seed < 32 nous devons remplir de 0 affins d'obtenir une seed sur 32 bits
$$
\begin{align*}
taille\_seed &= 16 \\
seed &= 1101001101011101 \\
seed\_complète &= 11010011010111010000000000000000
\end{align*}
$$

##### Calcule de la seed finale

Nous avons maintenant un uint32 complet (seed_complète) que nous appeleront **seed_root**.   
Pour trouver la seed finale nous allons faire l'algorithme suivant :  
1) Définir "**seed_root**" comme seed à notre fonction de génération pseudo-aléatoire
2) Générer **nombre_pixel** nombre aléatoire, le dernier nombre obtenus seras notre seed finale


### Inter-étape : Récupération de bit

Maintenant que nous avons la seed finale, nous allons pouvoir facilement récupérer des bit dans notre image.  
Durant cette inter-étape nous allons voir comment récuprer ces pixels en question.  

#### \#1 Définition de la seed  

Tout d'abord nous devons définir la seed qui resteras inchanger durant tous le processus.  
Cette seed seras la seed finale précédement calculé.  

#### \#2 Définition d'un padding (P)

Nous définissons un padding qui devras jamais être réinitialiser durant toutes la lecture.  
Ce padding seras du type uint32 pour avoir un modulo automatique au moment de l'overflow.  

#### \#3 Récupération du prochain pixel contenant de la donnée

1) Nous générons un nouveau nombre pseudo-aléatoire et l'additionnons au padding (padding += N)
2) nous lisons le pixel P = pixels[padding % nombre_pixel]
3) Nous avons maintenant plusieurs cas possible selons la valeurs de l'usage du pixel :  
   usage = HIDE_SEED -> nous faisons padding+=1 et on recommence cette étape avec le nouveau pixel récupéré  
   usage = ALL_READ -> nous faisons padding+=1 et on recommence cette étape avec le nouveau pixel récupéré
   usage = ZERO_READ -> la valeurs est enregistrer dans le bit de poid faible de la oucleur rouge | nous mettons maintenant l'usage du pixel sur ONE_READ  
   usage = ONE_READ -> la valeurs est enregistrer dans le bit de poid faible de la couleur bleu | nous mettons maintenant l'usage du pixel sur TWO_READ  
   usage = TWO_READ -> la valeurs est enregistrer dans le bit de poid faible de la couleur vert | nous mettons maintenant l'usage du pixel sur ALL_READ

### Étape 2 : Checksum,CR32 et Header et DATA 

#### Checksum

Pour vérifier si notre image est bien une image encoder par notre encodeur, les 32 premier bits seront le checksum suivant :  
```
01101101 01101001 01110100 01100101
```
Si les 32 premiers bit lus ne sont pas ça, nous arrêtons toutes opérations

#### CRC32

Les 32 prochains bit seront le CR32 calculer pour le header.

#### Header

Les prochains bits seront utiliser par le header. Le header est le suivant :
```c++
struct header {
  uint8_t version; // current version of protocole (current 0b1)
  uint32_t data_size_byte; // contains number of bits of data stored in image
  uint8_t data_type;  // contains type of data stored in image
  uint32_t data_crc32; // contains CRC32 of data stored in image
}
```
Pour vérifier son intégriter nous devons utiliser le CRC32 précédent

### Étape 3 : Lecture de la donnée

La lecture de la donnée est maintenant très simple nous devons seulement utiliser la fonction décrite durant
l'Inter-étape sur le nombre de bit définit dans le header. Les birs pourrons ensuite être utiliser de la manière
souhaitez!