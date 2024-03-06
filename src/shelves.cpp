#include <algorithm>
#include <fmt/core.h>

#include "shelves.hpp"
#include "gurobi_c++.h"
#include "gurobi_c.h"
#include "instance.hpp"


Shelves::Shelves(GRBEnv& env, Instance const& inst) : model{env}, y{inst.nitems, inst.rmax}, x{inst.nitems, inst.nitems, inst.rmax} {

    auto t0 = std::chrono::high_resolution_clock::now();

    std::vector<Config> sorted_items(inst.nitems);
    std::partial_sort_copy(inst.items.begin(), inst.items.end(),
                            sorted_items.begin(), sorted_items.end(), 
                            [] (auto const& a, auto const& b) { return a.h > b.h; });

    for (int i = 0; i < inst.nitems; ++i) {
        for (int r = 0; r < sorted_items[i].repeat; ++r) {
            y(i, r) = model.addVar(0, 1, sorted_items[i].h, GRB_BINARY, fmt::format("y_{}_{}", i, r));
            for (int k = i; k < inst.nitems; ++k) {
                x(k, i, r) = model.addVar(0, sorted_items[k].repeat, 0, GRB_INTEGER, fmt::format("x_{}_{}_{}", k, i, r));
            }
        }
    }
    
    //(19)
    for (auto const& task : inst.tasks) {
        GRBLinExpr expr{0};
        for (int i = 0; i < inst.nitems; ++i) {
            if (sorted_items[i].task_id == task.id) {
                for (int r = 0; r < sorted_items[i].repeat; ++r) {
                    expr += y(i, r);
                }
            }
        }

        for (int k = 0; k < inst.nitems; ++k) {
            if (sorted_items[k].task_id == task.id) {
                for (int i = 0; i <= k; ++i) {
                    for (int r = 0; r < sorted_items[i].repeat; ++r) {
                        expr += x(k, i, r);
                    }
                }
            }
        }

        model.addConstr(expr == task.repeat, fmt::format("19_{}", task.id));
    }

    //(20)
    for (int i = 0; i < inst.nitems; ++i) {
        for (int r = 0; r < sorted_items[i].repeat; ++r) {
            GRBLinExpr expr{0};
            for (int k = i; k < inst.nitems; ++k) {
                expr += sorted_items[k].w * x(k, i, r);
            }

            model.addConstr(expr <= (inst.w - sorted_items[i].w) * y(i, r), fmt::format("20_{}_{}", i, r));
        }
    }

    auto t1 = std::chrono::high_resolution_clock::now();
	_buildtime = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() / 1000.0; 
}


MIPResult Shelves::optimize(Args const& args, int ub) {
    
    auto res = solve(name, model, args, ub);
    fmt::print("\nobj={}, runtime={}, buildtime={}\n", res.obj, res.runtime, _buildtime);
    return res;
}