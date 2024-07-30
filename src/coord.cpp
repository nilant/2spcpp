#include <fmt/core.h>

#include "coord.hpp"
#include "gurobi_c++.h"
#include "gurobi_c.h"
#include "instance.hpp"
#include "mipresult.hpp"


Coord::Coord(GRBEnv& env, Instance const& inst, int ub) : model{env}, x{inst.nitems, inst.w, ub} {

    auto t0 = std::chrono::high_resolution_clock::now();

    z = model.addVar(0, GRB_INFINITY, 1, GRB_CONTINUOUS, "z"); 

    for (auto const& item : inst.items) {
        int i = item.id;
        for (int p = 0; p <= inst.w - item.w; ++p) {
            for (int q = 0; q <= ub - item.h; ++q) {
                x(i, p, q) = model.addVar(0, 1, 0, GRB_BINARY, fmt::format("x_{}_{}_{}", i, p, q));
            }
        }
    }

    //(4)
    for (auto const& task : inst.tasks) {
        if (task.id == -1) continue; 
        GRBLinExpr expr{0}; 
        for (auto const& item : task.configs) {
            int i = item.id;
            for (int p = 0; p <= inst.w - item.w; ++p) {
                for (int q = 0; q <= ub - item.h; ++q) {
                    expr += x(i, p, q);
                }
            }
        } 
        model.addConstr(expr == task.repeat, fmt::format("4_{}", task.id));
    }

    //(5)
    for (int r = 0; r < inst.w; ++r) {
        for (int s = 0; s < ub; ++s) {
            GRBLinExpr expr{0};
            for (auto const& item : inst.items) {
                int i = item.id;
                for (int p = std::max(0, r - item.w + 1); p <= r && p <= inst.w - item.w; ++p) {
                    for (int q = std::max(0, s - item.h + 1); q <= s && q <= ub - item.h; ++q) {
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
            for (int q = 0; q <= ub - item.h; ++q) {
                model.addConstr((q + item.h) * x(i, p, q) <= z, fmt::format("6_{}_{}_{}", i, p, q));
            }
        }
    }

    //(7)
    GRBLinExpr expr{0};
    for (auto const& item : inst.items) {
        int i = item.id;
        for (int p = 0; p <= inst.w - item.w; ++p) {
            for (int q = 0; q <= ub - item.h; ++q) {
                expr += (item.h * item.w) * x(i, p, q);
            }
        }
    }
    model.addConstr((1.0 / inst.w) * expr <= z, "7");


    auto t1 = std::chrono::high_resolution_clock::now();
	_buildtime = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() / 1000.0; 
};

MIPResult Coord::optimize(Args const& args, int ub) {
    
    auto res = solve(name, model, args);
    if (res.obj == -1) {
        res.obj = ub;
    }
    res.buildtime = _buildtime;

    return res;
}

Solution Coord::costruct_solution(Instance& inst) {

    inst.selected_items.clear();
    for (auto const& item : inst.items) {
        int i = item.id;
        for (int p = 0; p <= inst.w - item.w; ++p) {
            for (int q = 0; q <= inst.ub - item.h; ++q) {
                if (std::lrint(x(i, p, q).get(GRB_DoubleAttr_X)) == 1) {
                    Config item2 = item;
                    item2.x = p;
                    item2.y = q;
                    inst.selected_items.push_back(item2);
                }
            }
        }
    }

    Solution sol;
    sol.name = "coord";
    sol.items = inst.selected_items;
    sol.obj = static_cast<int>(std::lrint(z.get(GRB_DoubleAttr_X)));

    return sol;
}

Instance Coord::subinst(Instance const& inst) const {
    
    Instance sub;
    sub.name = inst.name;
    sub.rmax = inst.rmax;
    sub.wmax = sub.wmax;
    sub.w = inst.w;
    sub.seed = inst.seed;
    sub.alpha = inst.alpha;
    sub.tasks = inst.tasks;
    sub.items = inst.items;
    sub.reff = inst.reff;
    
    sub.ub = val(z);

    for (auto& item : inst.items) {
        int i = item.id;
        for (int p = 0; p <= inst.w - item.w; ++p) {
            for (int q = 0; q <= sub.ub - item.h; ++q) {
                if (val(x(i, p, q)) == 1) {
                    sub.selected_items.push_back(item);
                    sub.selected_items.back().x = p;
                    sub.selected_items.back().y = q;
                }
            }
        }
    }

    assert(sub.selected_items.size() == sub.reff);

    return sub;
}