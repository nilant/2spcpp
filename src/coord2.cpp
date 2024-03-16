#include <fmt/core.h>

#include "coord2.hpp"
#include "gurobi_c++.h"
#include "instance.hpp"
#include "mipresult.hpp"


Coord2::Coord2(GRBEnv& env, Instance const& inst, int ub) : model{env}, x{inst.nitems2, inst.w, ub} {

    auto t0 = std::chrono::high_resolution_clock::now();
    
    std::vector<Task> tasks;
    tasks.reserve(inst.tasks.size() * inst.rmax);

    std::vector<Config> items;
    items.reserve(inst.nitems * inst.rmax);

    int new_config_id = 0;
    int new_task_id = 0;
    for (auto const& task : inst.tasks) {
        for (int r = 0; r < task.repeat; ++r) {
            Task new_task(task);
            new_task.id = new_task_id++;
            new_task.repeat = 1;
            
            for (auto& config : new_task.configs) {
                config.id = new_config_id++;
                config.task_id = new_task.id;
            }

            tasks.push_back(new_task);

            items.insert(items.end(), new_task.configs.begin(), new_task.configs.end());
        }
    }

    int nitems = items.size();

    z = model.addVar(0, GRB_INFINITY, 1, GRB_CONTINUOUS); 

    for (auto const& item : items) {
        int i = item.id;
        for (int p = 0; p <= inst.w - item.w; ++p) {
            for (int q = 0; q <= ub - item.h; ++q) {
                x(i, p, q) = model.addVar(0, 1, 0, GRB_BINARY, fmt::format("x_{}_{}_{}", i, p, q));
            }
        }
    }

    //(9)
    for (auto const& task : tasks) {
        GRBLinExpr expr{0}; 
        for (auto const& config : task.configs) {
            int i = config.id;
            for (int p = 0; p <= inst.w - config.w; ++p) {
                for (int q = 0; q <= ub - config.h; ++q) {
                    expr += x(i, p, q);
                }
            }
        } 
        model.addConstr(expr == 1, fmt::format("9_{}", task.id));
    }

    //(10)
    for (int r = 0; r < inst.w; ++r) {
        for (int s = 0; s < ub; ++s) {
            GRBLinExpr expr{0};
            for (auto const& item : items) {
                int i = item.id;
                for (int p = std::max(0, r - item.w + 1); p <= r && p <= inst.w - item.w; ++p) {
                    for (int q = std::max(0, s - item.h + 1); q <= s && q <= ub - item.h; ++q) {
                        expr += x(i, p, q);
                    }
                }
            }
            model.addConstr(expr <= 1, fmt::format("10_{}_{}", r, s));
        }
    }

    //(11)
    for (auto const& task : tasks) {
        GRBLinExpr expr{0}; 
        for (auto const& item : task.configs) {
            int i = item.id;
            for (int p = 0; p <= inst.w - item.w; ++p) {
                for (int q = 0; q <= ub - item.h; ++q) {
                    expr += (q + item.h) * x(i, p, q);
                }
            }
        } 
        model.addConstr(z >= expr, fmt::format("11_{}", task.id));
    }

    //(12)
    GRBLinExpr expr{0};
    for (auto const& item : items) {
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

MIPResult Coord2::optimize(Args const& args) {
    
    auto res = solve(name, model, args);
    res.buildtime = _buildtime;
    return res;
}