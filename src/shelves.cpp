#include <algorithm>
#include <fmt/core.h>

#include "shelves.hpp"
#include "gurobi_c++.h"
#include "gurobi_c.h"
#include "instance.hpp"


Shelves::Shelves(GRBEnv& env, Instance const& inst) : model{env}, sorted_items{inst.items}, y{inst.nitems, inst.rmax}, x{inst.nitems, inst.nitems, inst.rmax} {

    auto t0 = std::chrono::high_resolution_clock::now();

    std::sort(sorted_items.begin(), sorted_items.end(), 
                [] (auto const& a, auto const& b) { 
                    return a.h > b.h; }
                );

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


MIPResult Shelves::optimize(Args const& args) {
    
    auto res = solve(name, model, args);
    return res;
}

std::vector<Instance> Shelves::subinsts(Instance const& inst) const {

    std::vector<Instance> subs;

    for (int i = 0; i < inst.nitems; ++i) {
        for (int r = 0; r < sorted_items[i].repeat; ++r) {
            if (val(y(i, r)) == 1) {
                Instance sub{};
                sub.name = inst.name;
                sub.rmax = inst.rmax;
                sub.wmax = inst.wmax;
                sub.w = inst.w;
                sub.seed = inst.seed;
                sub.alpha = inst.alpha;
                sub.ntasks = inst.ntasks;
                sub.nitems = inst.nitems;

                sub.ub = sorted_items[i].h;
                sub.selected_items.push_back(sorted_items[i]);

                sub.tasks = std::vector<Task>(sub.ntasks);
                sub.reff = 0;
                sub.tasks[sorted_items[i].task_id] = (inst.tasks[sorted_items[i].task_id]);
                sub.tasks[sorted_items[i].task_id].repeat = 1;
                if (val(x(i, i, r)) >= 1) {
                    sub.tasks[sorted_items[i].task_id].repeat += val(x(i, i, r));
                    for (int rr = 0; rr < val(x(i, i, r)); ++rr) {
                        sub.selected_items.push_back(sorted_items[i]);
                    }
                }
                sub.reff += sub.tasks[sorted_items[i].task_id].repeat;

                for (int k = i+1; k < inst.nitems; ++k) {
                    if (val(x(k, i, r)) >= 1) {
                        sub.tasks[sorted_items[k].task_id] = (inst.tasks[sorted_items[k].task_id]);
                        sub.tasks[sorted_items[k].task_id].repeat = val(x(k, i, r));
                        for (int rr = 0; rr < val(x(k, i, r)); ++rr) {
                            sub.selected_items.push_back(sorted_items[k]);
                        }
                        sub.reff += sub.tasks[sorted_items[k].task_id].repeat;
                    }
                }

                for (auto& task : sub.tasks) {
                    for (auto& item : task.configs) {
                        item.repeat = task.repeat;
                        sub.items.push_back(item);
                    }
                }

                assert(sub.selected_items.size() == sub.reff);

                sub.area();
                subs.push_back(sub);
            }
        }
    }

    return subs;
}