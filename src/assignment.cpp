#include "assignment.hpp"
#include "gurobi_c.h"
#include "mipresult.hpp"
#include <fmt/core.h>


Assignment::Assignment(GRBEnv& env, mdarray<int, 2> const& combs) : model{env}, x{combs.dimension(0), combs.dimension(1)} {

    int n = x.dimension(0);

    for (int i = 0; i < n; ++i) {
        for (int j = i+1; j < n; ++j) {
            x(i, j) = model.addVar(0, 1, combs(i, j), GRB_BINARY, fmt::format("x_{}_{}", i, j));
        }
    }

    for (int i = 0; i < n; ++i) {
        GRBLinExpr expr{0};
        for (int j = i; j < n; ++j) {
            expr += x(i, j);
        }

        for (int k = 0; k < i; ++k) {
            expr += x(k, i);
        }

        model.addConstr(expr == 1, fmt::format("ass_{}", i));
    }
}

MIPResult Assignment::optimize(Args const& args) {

    auto res = solve(name, model, args);
    fmt::print("\nobj={}, runtime={}, buildtime={}\n", res.obj, res.runtime, _buildtime);
    return res;
}