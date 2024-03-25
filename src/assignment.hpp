#pragma once

#include <gurobi_c++.h>

#include "mdarray.hpp"
#include "mipresult.hpp"
#include "cli.hpp"


class Assignment {
    std::string name{"assigment"};
    double _buildtime{0};

    GRBModel model;
    mdarray<GRBVar, 2> x;

    public:
        Assignment(GRBEnv& env, mdarray<int, 2> const& combs);
        MIPResult optimize(Args const& args);
        double runtime() { return model.get(GRB_DoubleAttr_Runtime); };
        double buildtime() { return _buildtime; };
};