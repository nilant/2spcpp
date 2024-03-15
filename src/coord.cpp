#include <fmt/core.h>

#include "coord.hpp"
#include "gurobi_c++.h"
#include "gurobi_c.h"
#include "mipresult.hpp"


Coord::Coord(GRBEnv& env, Instance const& inst) : model{env}, x{inst.nitems, inst.w, inst.ub} {

    auto t0 = std::chrono::high_resolution_clock::now();

    z = model.addVar(0, GRB_INFINITY, 1, GRB_CONTINUOUS, "z"); 

    for (auto const& item : inst.items) {
        int i = item.id;
        for (int p = 0; p <= inst.w - item.w; ++p) {
            for (int q = 0; q <= inst.ub - item.h; ++q) {
                x(i, p, q) = model.addVar(0, 1, 0, GRB_BINARY, fmt::format("x_{}_{}_{}", i, p, q));
            }
        }
    }

    //(4)
    for (auto const& task : inst.tasks) {
        GRBLinExpr expr{0}; 
        for (auto const& item : task.configs) {
            int i = item.id;
            for (int p = 0; p <= inst.w - item.w; ++p) {
                for (int q = 0; q <= inst.ub - item.h; ++q) {
                    expr += x(i, p, q);
                }
            }
        } 
        model.addConstr(expr == task.repeat, fmt::format("4_{}", task.id));
    }

    //(5)
    for (int r = 0; r < inst.w; ++r) {
        for (int s = 0; s < inst.ub; ++s) {
            GRBLinExpr expr{0};
            for (auto const& item : inst.items) {
                int i = item.id;
                for (int p = std::max(0, r - item.w + 1); p <= r && p <= inst.w - item.w; ++p) {
                    for (int q = std::max(0, s - item.h + 1); q <= s && q <= inst.ub - item.h; ++q) {
                        expr += x(i, p, q);
                    }
                }
            }
            model.addConstr(expr <= 1, fmt::format("5_{}_{}", r, s));
        }
    }

    //(6)
    for (auto const& item : inst.items) {
        int i = item.id;
        for (int p = 0; p <= inst.w - item.w; ++p) {
            for (int q = 0; q <= inst.ub - item.h; ++q) {
                model.addConstr((q + item.h) * x(i, p, q) <= z, fmt::format("6_{}_{}_{}", i, p, q));
            }
        }
    }

    auto t1 = std::chrono::high_resolution_clock::now();
	_buildtime = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() / 1000.0; 
};

MIPResult Coord::optimize(Args const& args, int ub) {
    
    auto res = solve(name, model, args, ub);
    res.buildtime = _buildtime;
    return res;
}