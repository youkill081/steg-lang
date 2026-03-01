# Stéganographe

Le but de ce programme est de cacher de la donnée dans des images en les altérant le moins possible.

## Stockage des pixels

Les pixels seront définis de cette manière :

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

La structure `pixel_usage` permettra de suivre l'utilisation de chaque pixel. Voici les valeurs possibles :  
-> HIDE_SEED : Le pixel cache la graine permanente permettant de connaître la clé pseudo-aléatoire.  
-> ZERO_READ : Le pixel peut stocker de la donnée, mais celle-ci n'a jamais été lue ; la donnée est donc stockée dans le canal rouge.  
-> ONE_READ : Le pixel a déjà été lu une fois ; la donnée est donc stockée dans le canal bleu.  
-> TWO_READ : Le pixel a déjà été lu deux fois ; la donnée est donc stockée dans le canal vert.  
-> ALL_READ : Le pixel a été lu trois fois ; il n'y a donc plus de donnée stockée à l'intérieur.

## Lecture/Écriture des pixels

Les pixels seront lus et écrits de la même manière afin qu'ils soient toujours retrouvés dans le même ordre.  
Nous partons ici du principe que tous les pixels ont été placés dans un vecteur de pixels, chaque pixel étant défini dans la struct `pixel` avec `usage` initialisé sur `ZERO_READ`.

Voici la description de cet algorithme étape par étape.

### Étape 1 : Lecture de la graine pseudo-aléatoire (K)

#### Récupération de la taille de la graine

La taille de la graine dépend de la taille de l'image.  
Pour calculer la taille de la graine nous ferons :
$$
taille\_seed = \min\big(\lceil \log_2(nombre\_pixel) \rceil, 32\big)
$$

#### Récupération de la graine

Voici l'algorithme pour retrouver la graine :

##### #1 → Définir `nombre_pixel` comme graine de notre fonction de génération pseudo-aléatoire

##### #2 Récupération des X bits

Nous définissons une variable `uint32 padding` égale à 0.  
Nous allons répéter les opérations suivantes `taille_seed` fois :

1) Génération d'un nombre pseudo-aléatoire N
2) padding += N % nombre_pixel
3) Récupération d'un pixel P = pixels[padding % nombre_pixel]
4) Si l'usage de P == HIDE_SEED : nous faisons padding += 1 et on recommence à l'étape 3  
   Si l'usage de P == ZERO_READ : on continue à l'étape 5
5) Nous récupérons le bit de poids faible de P sur la couleur rouge ; ce bit sera le bit de poids le plus fort pas encore récupéré de la graine.
6) Nous mettons l'usage de P sur HIDE_SEED.
7) Nous retournons à l'étape 1.

À la suite de cet algorithme nous aurons récupéré X bits avec X <= 32.  
Nous avons un cas spécial si taille_seed < 32.

###### Dans le cas où taille_seed < 32

Si taille_seed < 32, nous devons compléter avec des 0 afin d'obtenir une graine sur 32 bits :
$$
\begin{align*}
taille\_seed &= 16 \\
seed &= 1101001101011101 \\
seed\_complète &= 11010011010111010000000000000000
\end{align*}
$$

##### Calcul de la graine finale

Nous avons maintenant un `uint32` complet (seed_complète) que nous appellerons **seed_root**.  
Pour trouver la graine finale nous allons appliquer l'algorithme suivant :
1) Définir **seed_root** comme graine de notre fonction de génération pseudo-aléatoire.
2) Générer **nombre_pixel** nombres aléatoires ; le dernier nombre obtenu sera notre graine finale.


### Inter-étape : Récupération de bits

Maintenant que nous avons la graine finale, nous allons pouvoir facilement récupérer des bits dans notre image.  
Durant cette inter-étape nous allons voir comment récupérer ces pixels.

#### \#1 Définition de la graine

Tout d'abord nous devons définir la graine, qui restera inchangée durant tout le processus.  
Cette graine sera la graine finale précédemment calculée.

#### \#2 Définition d'un padding (P)

Nous définissons un padding qui ne devra jamais être réinitialisé durant toute la lecture.  
Ce padding sera de type `uint32` afin d'avoir un modulo automatique au moment de l'overflow.

#### \#3 Récupération du prochain pixel contenant de la donnée

1) Nous générons un nouveau nombre pseudo-aléatoire et l'additionnons au padding (padding += N).
2) Nous lisons le pixel P = pixels[padding % nombre_pixel].
3) Nous avons maintenant plusieurs cas possibles selon la valeur de l'usage du pixel :  
   usage = HIDE_SEED → nous faisons padding += 1 et on recommence cette étape avec le nouveau pixel récupéré.  
   usage = ALL_READ → nous faisons padding += 1 et on recommence cette étape avec le nouveau pixel récupéré.  
   usage = ZERO_READ → la valeur est enregistrée dans le bit de poids faible de la couleur rouge | nous mettons maintenant l'usage du pixel sur ONE_READ.  
   usage = ONE_READ → la valeur est enregistrée dans le bit de poids faible de la couleur bleue | nous mettons maintenant l'usage du pixel sur TWO_READ.  
   usage = TWO_READ → la valeur est enregistrée dans le bit de poids faible de la couleur verte | nous mettons maintenant l'usage du pixel sur ALL_READ.

### Étape 2 : Checksum, CRC32, Header et Data

#### Checksum

Pour vérifier si notre image a bien été encodée par notre encodeur, les 32 premiers bits seront le checksum suivant :
```
01101101 01101001 01110100 01100101
```
Si les 32 premiers bits lus ne correspondent pas, nous arrêtons toute opération.

#### CRC32

Les 32 prochains bits seront le CRC32 calculé pour le header.

#### Header

Les prochains bits seront utilisés par le header. Le header est le suivant :
```c++
struct header {
  uint8_t version;       // version actuelle du protocole (actuelle : 0b1)
  uint32_t data_size_byte; // nombre de bits de donnée stockés dans l'image
  uint8_t data_type;     // type de donnée stockée dans l'image
  uint32_t data_crc32;   // CRC32 de la donnée stockée dans l'image
}
```
Pour vérifier son intégrité nous devons utiliser le CRC32 précédent.

### Étape 3 : Lecture de la donnée

La lecture de la donnée est maintenant très simple ; nous devons seulement utiliser la fonction décrite durant
l'inter-étape sur le nombre de bits défini dans le header.  
Une fois récupérée, `data_crc32` devra être appliqué sur l'ensemble des bits récupérés pour vérifier leur intégrité.