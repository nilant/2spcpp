#pragma once

#include <gurobi_c++.h>
#include "cli.hpp"
#include "mdarray.hpp"
#include "instance.hpp"
#include "mipresult.hpp"

// Shelf-based MIP. Items are sorted by decreasing height; y(i, r) = 1 if the
// r-th copy of item i opens a shelf (its height is the shelf height) and
// x(k, i, r) is the number of copies of item k placed in that shelf. The
// solution is a quick feasible packing used as starting point.
class Shelves {
    std::string name{"shelves"};
    double _buildtime{0};

    GRBModel model;
    mdarray<GRBVar, 2> y;
    mdarray<GRBVar, 3> x;
    std::vector<Config> sorted_items;

    public:
        Shelves(GRBEnv& env, Instance const& inst);
        MIPResult optimize(Args const& args);
        double runtime() {return model.get(GRB_DoubleAttr_Runtime); };
        double buildtime() {return _buildtime; };
        // One sub-instance per opened shelf, containing the items on that shelf.
        std::vector<Instance> subinsts(Instance const& inst) const;
};