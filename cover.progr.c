// ============================================================
// Antenna Coverage Optimization
// IBM ILOG CPLEX Optimization Studio - OPL
// ============================================================

tuple Tpoint {
    int x;
    int y;
};

tuple Tantenne {
    float portee;
    int prix;
};

{Tpoint} points = ...;
{Tantenne} antennes = ...;

// 1 if two points have direct visibility, 0 otherwise
int visibilite[points][points] = ...;

// ------------------------------------------------------------
// Distance matrix
// ------------------------------------------------------------

float Distances[points][points];

execute {
    for (var p1 in points)
        for (var p2 in points)
            Distances[p1][p2] =
                Opl.sqrt(
                    (p1.x - p2.x) * (p1.x - p2.x)
                  + (p1.y - p2.y) * (p1.y - p2.y)
                );
}

// ------------------------------------------------------------
// Decision variables
//
// SELECT[a][pa] = 1
// if an antenna of type 'a' is installed at point 'pa'.
// ------------------------------------------------------------

dvar boolean SELECT[antennes][points];

// ------------------------------------------------------------
// Objective
//
// Minimize the total installation cost.
// ------------------------------------------------------------

minimize
    sum(a in antennes, pa in points)
        a.prix * SELECT[a][pa];

// ------------------------------------------------------------
// Coverage constraints
//
// Every point must be covered by at least one selected antenna.
//
// An antenna can cover a point if:
//   1. the antenna position is visible from the point;
//   2. the distance is within the antenna's range.
// ------------------------------------------------------------

subject to {

    forall(pAcouvrir in points)

        sum(
            a in antennes,
            pa in points :
                visibilite[pAcouvrir][pa] == 1
                &&
                Distances[pAcouvrir][pa] <= a.portee
        )
        SELECT[a][pa] >= 1;
}

// ------------------------------------------------------------
// Display selected antennas
// ------------------------------------------------------------

execute {

    writeln("Selected antennas:");

    for (var a in antennes)
        for (var p in points)
            if (SELECT[a][p] == 1)
                writeln(
                    "Range: ", a.portee,
                    " | Price: ", a.prix,
                    " | Position: ", p.x, "@", p.y
                );
}
