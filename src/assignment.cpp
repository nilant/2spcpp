#include "assignment.hpp"
#include "gurobi_c++.h"
#include "gurobi_c.h"
#include "mipresult.hpp"
#include <fmt/core.h>


Assignment::Assignment(GRBEnv& env, mdarray<int, 2> const& combs) : model{env}, x{combs.dimension(0), combs.dimension(1)} {

    auto t0 = std::chrono::high_resolution_clock::now();
    int n = x.dimension(0);

    for (int i = 0; i < n; ++i) {
        for (int j = i+1; j < n; ++j) {
            x(i, j) = model.addVar(0, 1, combs(i, j), GRB_BINARY, fmt::format("x_{}_{}", i, j));
        }
    }

    for (int i = 0; i < n; ++i) {
        GRBLinExpr expr{0};
        for (int j = i+1; j < n; ++j) {
            expr += x(i, j);
        }

        for (int k = 0; k < i; ++k) {
            expr += x(k, i);
        }

        model.addConstr(expr == 1, fmt::format("ass_{}", i));
    }

    auto t1 = std::chrono::high_resolution_clock::now();
	_buildtime = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() / 1000.0; 
}

MIPResult Assignment::optimize(Args const& args) {

    auto res = solve(name, model, args);
    fmt::print("\nobj={}, runtime={}, buildtime={}\n", res.obj, res.runtime, _buildtime);
    return res;
}

std::vector<Instance> Assignment::select(mdarray<Instance, 2> const& instances) {

    std::vector<Instance> insts;

    int n = x.dimension(0);

    for (int i = 0; i < n; ++i) {
        for (int j = i+1; j < n; ++j) {
            if (std::lrint(x(i, j).get(GRB_DoubleAttr_X)) == 1) {
                insts.push_back(instances(i, j));
            }
        }
    }

    return insts;
}