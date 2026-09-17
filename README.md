# antenna-coverage-optimization

## Antenna Coverage Optimization

## Overview

This project models and solves an **antenna placement optimization problem** using **IBM ILOG CPLEX Optimization Studio and OPL**.

The objective is to determine which antennas should be installed, and where, in order to **cover all required points of a territory while minimizing the total installation cost**.

The model takes into account:

* possible antenna locations;
* different antenna types;
* antenna range and installation cost;
* visibility between points;
* Euclidean distances;
* coverage constraints;
* binary decision variables;
* total installation cost.

The optimization problem is formulated in **OPL** and solved using **CPLEX**.

---

## Problem

A territory is represented by a set of points:

```text
P = {p₁, p₂, ..., pₙ}
```

Each point has Cartesian coordinates:

```text
(x, y)
```

Antennas can be installed at these points.

Several antenna types are available, each characterized by:

* a **range** (`portee`);
* an **installation price** (`prix`).

An antenna installed at position `pa` can cover a point `p` when:

1. `pa` is visible from `p`;
2. the distance between `p` and `pa` does not exceed the antenna's range.

The objective is therefore:

> **Find a feasible antenna configuration covering every required point while minimizing the total installation cost.**

---

## Mathematical Model

### Points

Points are represented using an OPL tuple:

```opl
tuple Tpoint {
    int x;
    int y;
};

{Tpoint} points = ...;
```

Each point therefore contains its coordinates.

---

### Antenna types

The available antenna types are represented by:

```opl
tuple Tantenne {
    float portee;
    int prix;
};

{Tantenne} antennes = ...;
```

Each antenna type has:

| Attribute | Description            |
| --------- | ---------------------- |
| `portee`  | Maximum coverage range |
| `prix`    | Installation cost      |

---

## Visibility Matrix

The model uses a visibility matrix:

```opl
int visibilite[points][points] = ...;
```

The value:

```text
visibilite[p1][p2] = 1
```

indicates that `p1` and `p2` are visible to each other.

This allows the model to take visibility constraints into account when determining whether an antenna can cover a point.

The matrix can therefore represent restrictions such as obstacles between two locations.

---

## Distance Matrix

Distances between all pairs of points are precomputed:

```opl
float Distances[points][points];
```

The Euclidean distance is calculated with:

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

This produces a distance matrix:

```text
             p1      p2      p3
        ┌────────────────────────
p1      │   0      d12     d13
p2      │  d21      0      d23
p3      │  d31     d32      0
```

Precomputing the distances avoids recalculating them during the optimization model.

---

## Decision Variables

The main decision variable is:

```opl
dvar boolean SELECT[antennes][points];
```

`SELECT[a][pa]` indicates whether an antenna of type `a` is installed at point `pa`.

```text
SELECT[a][pa] = 1
```

means:

> Install antenna type `a` at position `pa`.

Whereas:

```text
SELECT[a][pa] = 0
```

means:

> Do not install this antenna at this position.

The optimization solver determines these values.

---

## Objective Function

The objective is to minimize the total installation cost:

```opl
minimize
    sum(a in antennes, p in points)
        a.prix * SELECT[a][p];
```

The total cost is therefore:

```text
Σ price(a) × SELECT(a,p)
```

Only selected antennas contribute to the final cost.

---

## Coverage Constraints

Every point must be covered by at least one selected antenna.

The model expresses this with:

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

For a point `pAcouvrir`, the model considers only antenna installations satisfying both conditions:

```text
visibility[pAcouvrir][pa] = 1
```

and

```text
distance(pAcouvrir, pa) <= antenna.range
```

At least one of these possible installations must therefore be selected.

Conceptually:

```text
                  Point to cover
                        │
             ┌──────────┴──────────┐
             │                     │
        Is position visible?   Is range sufficient?
             │                     │
             └──────────┬──────────┘
                        │
                       YES
                        │
                        ▼
                Candidate antenna
                        │
                        ▼
                  SELECT[a][pa]
                        │
                        ▼
               At least one = 1
```

---

## Optimization Process

The complete optimization process can be summarized as:

```text
              Input data
                  │
       ┌──────────┼──────────┐
       │          │          │
     Points    Antennas   Visibility
       │          │          │
       └──────────┼──────────┘
                  │
                  ▼
           Distance Matrix
                  │
                  ▼
        Binary Decision Variables
             SELECT[a][p]
                  │
                  ▼
             CPLEX Solver
                  │
        ┌─────────┴─────────┐
        │                   │
   Coverage constraints   Cost function
        │                   │
        └─────────┬─────────┘
                  ▼
          Optimized solution
                  │
                  ▼
       Selected antenna positions
          + total installation cost
```

---

## Output

Once CPLEX has optimized the model, the selected antennas are displayed with their range, price and position:

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

An output can therefore look like:

```text
2.5 : 10@15
5.0 : 20@30
2.5 : 35@10
```

Each line corresponds to an antenna selected by the optimizer.

---

## Technologies

* **OPL (Optimization Programming Language)**
* **IBM ILOG CPLEX Optimization Studio**
* **Mixed-Integer Linear Programming (MILP)**
* Boolean decision variables
* Mathematical optimization
* Matrix modeling
* Euclidean distance
* Combinatorial optimization

---

## Project Structure

A possible organization of the project is:

```text
antenna-coverage-optimization/
│
├── README.md
│
├── model/
│   └── coverage.mod
│
├── data/
│   └── coverage.dat
│
└── examples/
    └── example.dat
```

### `coverage.mod`

Contains the optimization model:

* data structures;
* distance computation;
* decision variables;
* objective function;
* coverage constraints;
* result display.

### `coverage.dat`

Contains the instance data:

* territory points;
* antenna types;
* visibility matrix.

---

## Example Data Model

The model separates the **optimization logic** from the **input instance**.

Conceptually, the data file provides:

```text
Points
 ├── coordinates
 │
Antennas
 ├── range
 └── price
 │
Visibility
 └── visibility between points
```

The OPL model then uses these data to construct and solve the optimization problem.

---

## Key Concept

The central idea of the project is the binary matrix:

```opl
SELECT[antennes][points]
```

It represents all possible combinations of:

```text
        Antenna type
             ×
        Installation point
```

The solver searches for a configuration of these binary variables that satisfies the coverage constraints while minimizing the total cost.

In other words:

```text
        SELECT[a][p] ∈ {0, 1}

        ↓

     Feasible coverage

        +

   Minimum installation cost
```

---

## Academic Context

**Université de Bretagne Occidentale (UBO)**
**Licence 3 Informatique — IFA**
**Algorithmique avancée**

Project based on the **network coverage / GSM antenna positioning** optimization problem.

The project focuses on the formulation of a real-world positioning problem as a mathematical optimization model and its resolution using a dedicated solver.

---

## What This Project Demonstrates

This project demonstrates the ability to:

* translate a real-world problem into a mathematical model;
* structure optimization data using OPL tuples;
* model binary decisions;
* precompute distances;
* represent visibility constraints using matrices;
* formulate coverage constraints;
* define a cost-minimization objective;
* use **CPLEX** to search for an optimal feasible configuration;
* separate input data from the optimization model.

---

## License

This project is intended for educational and portfolio purposes.

