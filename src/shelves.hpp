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

    public:
        Shelves(GRBEnv& env, Instance const& inst);
        MIPResult optimize(Args const& args, int ub);
        double runtime() {return model.get(GRB_DoubleAttr_Runtime); };
        double buildtime() {return _buildtime; };
};