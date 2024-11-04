#pragma once

#include <gurobi_c++.h>
#include "cli.hpp"
#include "mdarray.hpp"
#include "instance.hpp"
#include "mipresult.hpp"

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
        std::vector<Instance> subinsts(Instance const& inst) const;
};