#pragma once

#include <filesystem>
#include <gurobi_c++.h>
#include "cli.hpp"
#include "mdarray.hpp"
#include "instance.hpp"
#include "mipresult.hpp"

// Coordinate-based MIP: x(i, p, q) = 1 if item (task configuration) i is placed
// with its bottom-left corner at (p, q), z is the strip height to minimize.
// A task with `repeat` > 1 must be placed `repeat` times in total, possibly
// with different configurations. `ub` bounds the height of the grid.
class Coord {
    std::string name{"coord"};
    double _buildtime{0};

    GRBModel model;
    mdarray<GRBVar, 3> x;
    GRBVar z;

    public:
        Coord(GRBEnv& env, Instance const& inst, int ub);
        MIPResult optimize(Args const& args, int ub);
        // Write the Gurobi model/solution next to the instance as <name>_sol.json.
        void write_sol(fs::path const& file_path);
        // Extract the placed items (with x, y) from the optimal variables.
        Solution costruct_solution(Instance& inst);
        double runtime() { return model.get(GRB_DoubleAttr_Runtime); };
        double buildtime() { return _buildtime; };
        // Build an instance holding the placed items of the current solution
        // (used by the matheuristic to continue from the coord solution).
        Instance subinst(Instance const& inst) const;
};