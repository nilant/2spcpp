#pragma once

#include <gurobi_c++.h>
#include "cli.hpp"
#include "mdarray.hpp"
#include "instance.hpp"
#include "mipresult.hpp"

class Coord2 {
    std::string name{"coord2"};
    double _buildtime{0};

    GRBModel model;
    mdarray<GRBVar, 3> x;
    GRBVar z;

    public:
        Coord2(GRBEnv& env, Instance const& inst);
        MIPResult optimize(Args const& args, int ub);
        double runtime() { return model.get(GRB_DoubleAttr_Runtime); };
        double buildtime() { return _buildtime; };
};