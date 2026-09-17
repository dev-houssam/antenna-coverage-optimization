# antenna-coverage-optimization

## Couverture et positionnement d'antennes GSM

> Projet universitaire d'algorithmique avancée portant sur la modélisation et la résolution d'un problème de couverture réseau par optimisation.

## Présentation

Ce projet consiste à résoudre un problème de **positionnement d'antennes GSM** sur un territoire représenté par un ensemble de points.

L'objectif est de déterminer **où implanter les antennes et quels types d'antennes utiliser** afin de couvrir les points demandés tout en **minimisant le coût global d'implantation**.

Le problème prend notamment en compte :

- les coordonnées des points du territoire ;
- plusieurs catégories d'antennes ;
- la portée et le coût de chaque type d'antenne ;
- la distance entre les points ;
- les obstacles présents sur le territoire ;
- la visibilité entre une antenne et un point à couvrir ;
- les contraintes d'implantation et de couverture ;
- la faisabilité de la solution.

Le problème est ensuite **modélisé sous forme d'un programme linéaire en variables booléennes** et confié à un solveur d'optimisation.

---

## Problématique

On dispose d'un ensemble de points représentant un territoire.

Chaque antenne potentielle est caractérisée par :

- une position ;
- un type ;
- une portée ;
- un coût d'installation.

Une antenne peut couvrir un point si deux conditions sont satisfaites :

1. la distance entre l'antenne et le point est inférieure ou égale à la portée de l'antenne ;
2. le point est visible depuis l'antenne, c'est-à-dire que le segment reliant les deux points ne traverse aucun obstacle.

L'objectif est alors de sélectionner un ensemble d'antennes permettant de satisfaire les contraintes de couverture avec un **coût total minimal**.

---

## Modélisation du problème

### Points

Le territoire est représenté par un ensemble de points :

```text
P = {p1, p2, ..., pn}
```

Chaque point possède des coordonnées permettant notamment de calculer les distances et les intersections avec les obstacles.

Dans la configuration de base, les emplacements potentiels des antennes correspondent aux points du territoire.

---

### Types d'antennes

Chaque type d'antenne possède plusieurs caractéristiques :

```text
Antenne
├── portée
└── coût
```

Par exemple :

| Type | Portée | Coût |
|---|---:|---:|
| A1 | P1 | C1 |
| A2 | P2 | C2 |
| A3 | P3 | C3 |

Les valeurs dépendent des données fournies au programme.

---

## Calcul des distances

Une matrice de distances est pré-calculée entre les différents points :

```text
Distance[p1][p2]
```

Elle permet de déterminer rapidement si une antenne située en `pa` possède une portée suffisante pour couvrir `pb`.

La condition de portée est :

```text
Distance(pa, pb) <= Portee[antenne]
```

Le pré-calcul évite ainsi de recalculer les distances pendant les différentes étapes de résolution.

---

## Gestion des obstacles

La couverture ne dépend pas uniquement de la distance.

Le territoire peut également contenir des **obstacles représentés par des segments**.

Une antenne `pa` peut couvrir un point `pb` uniquement si le segment :

```text
(pa, pb)
```

n'intersecte aucun segment représentant un obstacle.

On obtient ainsi une matrice de visibilité :

```text
Visibilite[p1][p2]
```

avec une représentation conceptuelle :

```text
Visibilite[p1][p2] = 1
```

si `p1` peut voir `p2`, et :

```text
Visibilite[p1][p2] = 0
```

dans le cas contraire.

Cette étape introduit une composante de **géométrie algorithmique** dans le problème.

---

## Matrice de couverture

À partir des données précédentes, on peut déterminer les possibilités de couverture.

Une ligne correspond à une antenne potentielle :

```text
(type d'antenne, position)
```

Une colonne correspond à un point à couvrir.

On définit alors une matrice de couverture :

```text
C[antenne, position][point]
```

avec :

```text
C[a, pa][pb] = 1
```

si l'antenne de type `a`, positionnée au point `pa`, peut couvrir le point `pb`.

Elle doit satisfaire simultanément les conditions de portée et de visibilité.

Conceptuellement :

```text
C[a, pa][pb] =
    1 si Distance[pa][pb] <= Portee[a]
       et Visibilite[pa][pb] = 1

    0 sinon
```

---

# Optimisation

Le problème est ensuite formulé comme un **programme linéaire en variables booléennes**.

La variable de décision principale est :

```text
SELECT[antenne][localisation]
```

Elle indique si une antenne d'un type donné est sélectionnée à une position donnée.

```text
SELECT[a][p] = 1
```

signifie :

> installer une antenne de type `a` au point `p`.

Inversement :

```text
SELECT[a][p] = 0
```

signifie que cette combinaison n'est pas sélectionnée.

---

## Fonction objectif

Le solveur cherche à minimiser le coût total d'installation :

```text
Minimiser :

Σ coût[a] × SELECT[a][p]
```

pour l'ensemble des types d'antennes et des positions possibles.

L'objectif est donc de trouver une configuration de réseau satisfaisant les contraintes avec un coût minimal.

---

## Contraintes de couverture

Chaque point devant être couvert doit être couvert par au moins une antenne sélectionnée.

La couverture d'un point dépend de la matrice `C`.

Conceptuellement :

```text
Pour chaque point pb :

Σ C[a][pa][pb] × SELECT[a][pa] >= 1
```

La sélection finale doit donc permettre de couvrir l'ensemble des points requis.

---

## Contraintes supplémentaires

Le modèle peut également prendre en compte différentes restrictions sur le problème :

- emplacements autorisés pour les antennes ;
- types d'antennes disponibles selon les emplacements ;
- points devant obligatoirement être couverts ;
- zones devant être couvertes ;
- zones ne devant pas être couvertes ;
- zones dans lesquelles les antennes peuvent être implantées ;
- obstacles ;
- contraintes géométriques.

Cette modélisation permet d'adapter le problème à différents scénarios de couverture.

---

# Architecture générale

Le traitement peut être représenté de la manière suivante :

```text
                    DONNÉES D'ENTRÉE
                           │
                           ▼
                ┌─────────────────────┐
                │      Points         │
                │  Coordonnées        │
                └──────────┬──────────┘
                           │
                           ▼
                ┌─────────────────────┐
                │ Calcul des distances│
                │ Distance[p1][p2]    │
                └──────────┬──────────┘
                           │
                           ▼
                ┌─────────────────────┐
                │ Gestion des obstacles│
                │ Visibilité[p1][p2]  │
                └──────────┬──────────┘
                           │
                           ▼
                ┌─────────────────────┐
                │ Matrice de couverture│
                │ C[a,pa][pb]         │
                └──────────┬──────────┘
                           │
                           ▼
                ┌─────────────────────┐
                │ Modèle d'optimisation│
                │ SELECT[a][p]        │
                └──────────┬──────────┘
                           │
                           ▼
                ┌─────────────────────┐
                │       CPLEX         │
                │ Solveur             │
                └──────────┬──────────┘
                           │
                           ▼
                    RÉSULTAT FINAL
                           │
             ┌─────────────┼─────────────┐
             ▼             ▼             ▼
        Antennes       Coût total    Faisabilité
        sélectionnées
```

---

# Organisation du traitement

## 1. Lecture des données

Le programme commence par récupérer les différentes informations nécessaires :

- points du territoire ;
- coordonnées ;
- types d'antennes ;
- portée ;
- coût ;
- contraintes ;
- obstacles.

Un format de données est défini afin de permettre leur transmission au programme d'optimisation.

---

## 2. Pré-calcul des distances

Les distances entre les différents points sont calculées et stockées dans une matrice :

```text
Distance[p1][p2]
```

Cette structure permet de réutiliser directement les distances lors de la construction du problème de couverture.

---

## 3. Construction de la visibilité

Les obstacles sont utilisés pour déterminer les couples de points mutuellement visibles.

Pour chaque paire de points :

```text
p1 → p2
```

on vérifie si le segment reliant les deux points intersecte un obstacle.

Le résultat est stocké dans :

```text
Visibilite[p1][p2]
```

---

## 4. Construction de la couverture

Les caractéristiques des antennes sont ensuite combinées avec les matrices de distance et de visibilité.

Pour chaque combinaison :

```text
(type d'antenne, position, point)
```

on détermine si le point peut être couvert.

Le résultat constitue la matrice :

```text
C[a, pa][pb]
```

---

## 5. Construction du modèle d'optimisation

Les possibilités de couverture sont transformées en contraintes du programme linéaire.

Les variables booléennes `SELECT` représentent les décisions d'installation.

Le solveur doit alors déterminer :

```text
Quelles antennes installer ?
Où les installer ?
Quel type choisir ?
Quel est le coût minimal ?
La couverture demandée est-elle réalisable ?
```

---

## 6. Résolution

Les données sont transmises au solveur **IBM ILOG CPLEX**.

Le solveur recherche une configuration respectant les contraintes du problème tout en minimisant le coût d'installation.

---

## 7. Exploitation du résultat

Après résolution, le programme analyse les valeurs de :

```text
SELECT[antenne][localisation]
```

afin d'identifier les antennes retenues.

Le résultat permet notamment d'obtenir :

- les antennes sélectionnées ;
- leurs positions ;
- leur type ;
- le coût global ;
- la faisabilité de la solution.

---

# Exemple conceptuel

Supposons un territoire composé de plusieurs points :

```text
P1 ─── P2 ─── P3
│             │
│   obstacle  │
│             │
P4 ─── P5 ─── P6
```

Plusieurs antennes peuvent être installées sur certains points.

Pour une antenne située en `P1` :

```text
Distance(P1, P5) <= portée
```

peut être vrai.

Cependant, si un obstacle coupe le segment :

```text
P1 ───────── P5
```

alors :

```text
Visibilite[P1][P5] = 0
```

et `P5` ne peut pas être couvert depuis `P1`.

Le modèle d'optimisation doit donc tenir compte simultanément :

```text
distance
    +
visibilité
    +
type d'antenne
    +
coût
    +
contraintes
```

---

# Structures de données

Le projet manipule principalement des structures permettant de représenter :

### Points

```text
Point
├── x
└── y
```

### Antennes

```text
Antenne
├── type
├── portée
└── coût
```

### Obstacles

```text
Segment
├── point de départ
└── point d'arrivée
```

### Matrices

```text
Distance[p1][p2]
Visibilite[p1][p2]
Couverture[antenne][position][point]
```

### Variables de décision

```text
SELECT[antenne][position]
```

---

# Aspects algorithmiques

Ce projet met en œuvre plusieurs problématiques classiques d'algorithmique avancée.

### Pré-calcul

La matrice des distances permet de transformer des calculs répétés en accès directs à des valeurs déjà calculées.

### Géométrie algorithmique

La gestion des obstacles nécessite de déterminer si des segments se croisent.

### Matrices

Les relations entre points, antennes et possibilités de couverture sont représentées par différentes matrices.

### Optimisation combinatoire

Le nombre de configurations possibles augmente rapidement avec le nombre de points, d'antennes et de types disponibles.

Le recours à un solveur permet d'explorer efficacement cet espace de solutions sous contraintes.

### Programmation linéaire

Le problème est exprimé sous forme d'un modèle mathématique composé :

- de variables de décision ;
- de contraintes ;
- d'une fonction objectif.

---

# Technologies et outils

- **C**
- **CPLEX**
- **Programmation linéaire**
- **Optimisation combinatoire**
- **Algorithmique avancée**
- **Matrices**
- **Géométrie algorithmique**
- **Structures de données**
- **Linux / environnement Unix**
- **Fichiers de données**

---

# Objectifs pédagogiques

Le projet permettait notamment de travailler sur :

- la conception d'un format de données ;
- la représentation de données structurées ;
- la manipulation de matrices ;
- le pré-calcul algorithmique ;
- la géométrie computationnelle ;
- la modélisation d'un problème réel ;
- la formulation d'un programme linéaire ;
- l'utilisation d'un solveur d'optimisation ;
- l'interprétation des résultats d'un modèle mathématique ;
- la gestion de contraintes ;
- l'analyse de solutions.

---

# Contexte universitaire

**Université de Bretagne Occidentale — UBO**  
**Licence 3 Informatique / IFA**  
**Module : Algorithmique avancée**  
**TP 2 : Couverture réseau**

Le sujet retrouvé dans les archives correspond à l'année universitaire **2018/2019**.

Le projet présenté ici reprend le problème de **couverture et positionnement d'antennes GSM** décrit dans cet enseignement.

---

# Résultats attendus

Le programme doit être capable de produire une solution permettant notamment de déterminer :

```text
+--------------------------------------+
| Solution de couverture               |
+--------------------------------------+
| Antennes sélectionnées               |
| Position des antennes                |
| Type des antennes                    |
| Coût global d'implantation           |
| Faisabilité de la solution           |
+--------------------------------------+
```

L'objectif n'est donc pas simplement de déterminer quelles antennes couvrent quels points, mais de **rechercher une configuration optimale respectant les contraintes du territoire**.

---

# Schéma simplifié du modèle

```text
                     TERRITOIRE
                         │
          ┌──────────────┼──────────────┐
          │              │              │
          ▼              ▼              ▼
       Points        Antennes        Obstacles
          │              │              │
          ▼              ▼              ▼
      Distance       Portée/Coût    Visibilité
          │              │              │
          └──────────────┼──────────────┘
                         ▼
                Matrice de couverture
                         │
                         ▼
                  Variables SELECT
                         │
                         ▼
                Programme linéaire
                         │
                         ▼
                       CPLEX
                         │
                         ▼
                 Solution optimale
```

---

## Licence MIT
