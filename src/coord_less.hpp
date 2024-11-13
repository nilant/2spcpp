#pragma once

#include <gurobi_c++.h>
#include "cli.hpp"
#include "mdarray.hpp"
#include "instance.hpp"
#include "mipresult.hpp"

class CoordLess{
    std::string name{"coord_less"};
    double _buildtime{0};

    GRBModel model;
    mdarray<GRBVar, 3> x;
    GRBVar z;

    public:
        CoordLess(GRBEnv& env, Instance const& inst, int ub);
        MIPResult optimize(Args const& args, int ub);
        double runtime() { return model.get(GRB_DoubleAttr_Runtime); };
        double buildtime() { return _buildtime; };
        Instance subinst(Instance const& inst) const;
};