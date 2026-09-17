# Optimisation de la couverture par antennes

## Présentation

Ce projet consiste à **modéliser et résoudre un problème d'optimisation du positionnement d'antennes** à l'aide d'**IBM ILOG CPLEX Optimization Studio** et du langage **OPL (Optimization Programming Language)**.

L'objectif est de déterminer **quelles antennes installer et à quels emplacements**, afin de couvrir l'ensemble des points d'un territoire tout en **minimisant le coût total d'installation**.

Le modèle prend notamment en compte :

* les points du territoire ;
* les positions possibles des antennes ;
* différents types d'antennes ;
* la portée de chaque type d'antenne ;
* le prix de chaque antenne ;
* la visibilité entre les points ;
* les distances entre les points ;
* les contraintes de couverture ;
* les variables de décision booléennes ;
* la minimisation du coût total.

---

## Problématique

Le territoire est représenté par un ensemble de points.

Chaque point possède des coordonnées cartésiennes :

```text
(x, y)
```

Les antennes peuvent être installées sur ces différents points.

Plusieurs types d'antennes sont disponibles. Chaque type est caractérisé par :

* une **portée** ;
* un **prix**.

Une antenne installée sur un point `pa` peut couvrir un point `p` si :

1. le point `pa` est visible depuis `p` ;
2. la distance entre `p` et `pa` est inférieure ou égale à la portée de l'antenne.

Le problème consiste donc à trouver une configuration permettant de **couvrir tous les points requis au coût minimal**.

---

## Modélisation des données

### Points

Les points sont représentés par un tuple OPL :

```opl
tuple Tpoint {
    int x;
    int y;
};

{Tpoint} points = ...;
```

Chaque élément de `points` représente ainsi une position possible avec ses coordonnées.

---

### Types d'antennes

Les différents types d'antennes sont définis par :

```opl
tuple Tantenne {
    float portee;
    int prix;
};

{Tantenne} antennes = ...;
```

Chaque type possède donc deux caractéristiques :

| Attribut | Signification                |
| -------- | ---------------------------- |
| `portee` | Portée maximale de l'antenne |
| `prix`   | Coût d'installation          |

---

## Matrice de visibilité

Le modèle utilise une matrice de visibilité :

```opl
int visibilite[points][points] = ...;
```

La valeur :

```text
visibilite[p1][p2] = 1
```

indique que les deux points sont visibles l'un depuis l'autre.

Cette matrice permet notamment de représenter les contraintes liées aux **obstacles présents sur le territoire**.

Ainsi, une antenne peut être suffisamment proche d'un point sans pour autant pouvoir le couvrir si la visibilité entre les deux positions n'est pas possible.

---

## Calcul des distances

Une matrice contenant les distances entre toutes les paires de points est construite :

```opl
float Distances[points][points];
```

Les distances euclidiennes sont calculées avant la résolution :

```opl
execute {
    for(var p1 in points)
        for(var p2 in points)
            Distances[p1][p2] =
                Opl.sqrt(
                    (p1.x-p2.x)*(p1.x-p2.x)
                    + (p1.y-p2.y)*(p1.y-p2.y)
                );
}
```

On obtient ainsi une matrice de distances :

```text
             P1      P2      P3
        ┌────────────────────────
P1      │   0      d12     d13
P2      │  d21      0      d23
P3      │  d31     d32      0
```

Cette matrice est ensuite utilisée pour déterminer si la portée d'une antenne est suffisante pour couvrir un point.

---

## Variables de décision

La décision principale du modèle est représentée par une matrice de variables booléennes :

```opl
dvar boolean SELECT[antennes][points];
```

`SELECT[a][pa]` indique si une antenne de type `a` doit être installée au point `pa`.

Ainsi :

```text
SELECT[a][pa] = 1
```

signifie :

> Une antenne du type `a` est installée au point `pa`.

Et :

```text
SELECT[a][pa] = 0
```

signifie qu'aucune antenne de ce type n'est installée à cet emplacement.

C'est **CPLEX qui détermine les valeurs de ces variables** lors de l'optimisation.

---

## Fonction objectif

Le modèle cherche à minimiser le coût total des antennes installées :

```opl
minimize
    sum(a in antennes, p in points)
        a.prix * SELECT[a][p];
```

Mathématiquement :

```text
min Σ prix(a) × SELECT[a][p]
```

Une antenne n'est donc comptabilisée dans le coût que lorsque sa variable `SELECT` vaut `1`.

---

## Contraintes de couverture

Chaque point du territoire doit être couvert par **au moins une antenne compatible**.

La contrainte principale est :

```opl
forall(pAcouvrir in points)
    sum(
        a in antennes,
        pa in points :
            (visibilite[pAcouvrir][pa] == 1)
            &&
            (Distances[pAcouvrir][pa] <= a.portee)
    )
    SELECT[a][pa] >= 1;
```

Pour chaque point `pAcouvrir`, seules les installations possibles respectant les deux conditions suivantes sont considérées :

```text
visibilité = 1
```

et

```text
distance ≤ portée de l'antenne
```

Parmi ces possibilités, **au moins une antenne doit être sélectionnée**.

---

## Fonctionnement général

Le fonctionnement du modèle peut être résumé ainsi :

```text
                  Données
                    │
        ┌───────────┼───────────┐
        │           │           │
      Points     Antennes   Visibilité
        │           │           │
        └───────────┼───────────┘
                    │
                    ▼
             Matrice des
               distances
                    │
                    ▼
          Variables de décision
             SELECT[a][p]
                    │
                    ▼
              Modèle OPL
                    │
                    ▼
             Solveur CPLEX
                    │
             ┌──────┴──────┐
             │             │
        Couverture       Coût
        obligatoire    minimal
             │             │
             └──────┬──────┘
                    ▼
              Solution optimale
                    │
                    ▼
          Antennes sélectionnées
             + coût total
```

---

## Exemple de sélection

Le tableau `SELECT` représente les différentes possibilités :

```text
                  Positions
             P1    P2    P3    P4
           ┌──────────────────────
Antenne 1  │ 0     1     0     0
Antenne 2  │ 0     0     0     1
Antenne 3  │ 0     0     1     0
```

Cela signifie par exemple que le solveur a choisi :

```text
Antenne 1 → P2
Antenne 2 → P4
Antenne 3 → P3
```

Le coût de cette configuration est calculé automatiquement par la fonction objectif.

---

## Affichage de la solution

Après l'optimisation, les antennes sélectionnées sont affichées :

```opl
execute {
    for(var a in antennes)
        for(var p in points)
            if (SELECT[a][p] == 1)
                writeln(
                    a.portee,
                    " : ",
                    p.x,
                    "@",
                    p.y
                );
}
```

Le résultat permet notamment d'obtenir la **portée de l'antenne sélectionnée ainsi que sa position**.

---

## Architecture du modèle

Le modèle peut être organisé autour de deux éléments principaux :

```text
antenna-coverage-optimization/
│
├── README.md
│
├── model/
│   └── coverage.mod
│
└── data/
    └── coverage.dat
```

### `coverage.mod`

Contient le modèle d'optimisation :

* définition des tuples ;
* définition des ensembles ;
* matrice de visibilité ;
* calcul des distances ;
* variables de décision ;
* fonction objectif ;
* contraintes de couverture ;
* affichage des résultats.

### `coverage.dat`

Contient les données de l'instance :

* points ;
* types d'antennes ;
* portée ;
* prix ;
* matrice de visibilité.

Cette séparation permet de conserver **le modèle d'optimisation indépendant des données utilisées pour une instance donnée**.

---

## Technologies

* **OPL — Optimization Programming Language**
* **IBM ILOG CPLEX Optimization Studio**
* **Programmation linéaire en nombres entiers**
* **Variables de décision booléennes**
* **Optimisation combinatoire**
* **Modélisation mathématique**
* **Matrices**
* **Géométrie / distance euclidienne**

---

## Concepts étudiés

Ce projet permet notamment de mettre en pratique :

* la modélisation d'un problème réel sous forme mathématique ;
* la définition de structures de données avec les `tuple` OPL ;
* la modélisation de décisions binaires ;
* la construction d'une matrice de distances ;
* l'utilisation d'une matrice de visibilité ;
* la formulation de contraintes de couverture ;
* la minimisation d'une fonction de coût ;
* l'utilisation d'un solveur d'optimisation ;
* la séparation entre **modèle** et **données d'entrée**.

---

## Contexte académique

**Université de Bretagne Occidentale (UBO)**
**Licence 3 Informatique — IFA**
**Algorithmique avancée**

Le projet porte sur un problème de **couverture réseau et de positionnement d'antennes GSM**.

L'objectif pédagogique est de transformer un problème de positionnement comportant des contraintes géométriques et économiques en un **modèle d'optimisation mathématique**, puis de confier sa résolution à un solveur.

---

## Résumé

Le problème peut finalement être résumé par trois éléments :

```text
             Décisions
                 │
                 ▼
        SELECT[a][pa] ∈ {0,1}
                 │
        ┌────────┴────────┐
        ▼                 ▼
   Couvrir tous      Minimiser le
     les points          coût
        │                 │
        └────────┬────────┘
                 ▼
          CPLEX / OPL
                 │
                 ▼
        Configuration optimale
```

**Le modèle cherche donc une configuration d'antennes qui satisfait les contraintes de couverture tout en minimisant le coût total d'installation.**


## Licence MIT
