#pragma once

#include <gurobi_c++.h>
#include "cli.hpp"
#include "mdarray.hpp"
#include "instance.hpp"
#include "mipresult.hpp"

// Coordinate-based MIP, variant 2: every repetition of a task is a separate
// task (nitems2 items in total), so each one picks its own configuration.
// x(i, p, q) = 1 if item i is placed with its bottom-left corner at (p, q).
// z is the makespan (strip height) to minimize; `ub` bounds the grid height.
class Coord2 {
    std::string name;  // instance name, reported in the result
    double _buildtime{0};

    GRBModel model;
    mdarray<GRBVar, 3> x;
    GRBVar z;

    public:
        Coord2(GRBEnv& env, Instance const& inst, int ub);
        MIPResult optimize(Args const& args);
        double runtime() { return model.get(GRB_DoubleAttr_Runtime); };
        double buildtime() { return _buildtime; };
};