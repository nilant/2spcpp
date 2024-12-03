#include "heuristic.hpp"
#include "assignment.hpp"
#include "heuresult.hpp"
#include "instance.hpp"
#include "shelves.hpp"
#include "coord.hpp"
#include "bottom_left_plus.hpp"

#include <chrono>
#include <fmt/core.h>
#include <algorithm>


std::pair<std::vector<Instance>, double> solve_level(GRBEnv& env, std::vector<Instance>& subs, Instance const& inst, Args const& args) {

    int n = subs.size();
    if (n % 2 != 0) {
        Instance empty{};
        subs.push_back(empty);
        n++;
    }

    Args coord_args{args};
    int npairs = (n * (n-1) / 2);
    int solved_pairs = 0;
    coord_args.timelimit /= npairs;

    mdarray<int, 2> combs{n, n};
    mdarray<Instance, 2> instances{n, n};

    double runtime = 0.0;
    for (int i = 0; i < n; ++i) {
        for (int j = i+1; j < n; ++j) {
            Instance new_inst = merge(subs[i], subs[j]);

            fmt::print("pair ({},{}): merge={} ", i, j, new_inst.ub);
            Coord coord(env, new_inst, new_inst.ub);
            auto coord_res = coord.optimize(coord_args, new_inst.ub);
            fmt::print("coord={} runtime={:.2f}({:.2f})\n", coord_res.obj, coord_res.runtime, coord_args.timelimit);
            runtime += coord_res.runtime;
            solved_pairs++;

            if (coord_args.timelimit - coord_res.runtime > 1e-1) {
                coord_args.timelimit = coord_args.timelimit + (coord_args.timelimit - coord_res.runtime) / (npairs - solved_pairs); 
            }

            if (coord_res.obj != -1 && coord_res.obj <= new_inst.ub) {
                auto sub = coord.subinst(new_inst);
                instances(i, j) = sub;
                combs(i, j) = sub.ub;
            } else {
                instances(i, j) = new_inst;
                combs(i, j) = new_inst.ub;
            }
        }
    }

    Assignment ass{env, combs};
    ass.optimize(args);
    auto new_subs = ass.select(instances);

    return {new_subs, runtime};
}

std::vector<Instance> filter_subs(std::vector<Instance>& subs, double percentage) {
    auto it = std::partition(subs.begin(), subs.end(), [percentage](auto const& sub) {return sub.fill_ratio <= percentage; });
    std::vector<Instance> full_instances{it, subs.end()};
    subs.erase(it, subs.end());
    
    return full_instances;
}

HeurResult heuristic(GRBEnv& env, Instance const& inst, Args const& args) {

    fmt::print("Optimizing {}...\n", inst.name);

    double filter_area_percent = 1;

    auto t0 = std::chrono::high_resolution_clock::now();

    HeurResult res{args.exec_name};

    Args shelves_args{args};
    shelves_args.timelimit = 10;

    Shelves shelves{env, inst};
    auto shelves_sol = shelves.optimize(shelves_args);
    res.start_sol = shelves_sol.obj;
    
    fmt::print("starting solution={}\n", res.start_sol);

    auto subs = shelves.subinsts(inst);
    auto full_insts = filter_subs(subs, filter_area_percent);

    int n = subs.size();

    if (n % 2 != 0) {
        Instance empty{};
        subs.push_back(empty);
        n++;
    }

    int nlevels = std::ceil(std::log2(n));
    int solved_levels = 0;
    assert(nlevels > 0);

    Args coord_args{args};
    coord_args.timelimit /= nlevels;
    fmt::print("n={}, pairs={}, nlevels={}\n", n, (n * (n-1) / 2), nlevels);

    int i = 0;
    while (n >= 2) {
        auto solve_res = solve_level(env, subs, inst, coord_args);
        solved_levels++;
        subs = solve_res.first;
        auto runtime_level = solve_res.second;

        if (coord_args.timelimit - runtime_level > 1) {
            coord_args.timelimit = coord_args.timelimit + (coord_args.timelimit - runtime_level) / (nlevels - solved_levels);
        }

        auto full = filter_subs(subs, filter_area_percent);
        full_insts.insert(full_insts.end(), full.begin(), full.end());
        n = subs.size();

        int obj = 0;
        for (auto const& sub : subs) {
            obj += sub.ub;
        }
        fmt::print("level={}, obj={}\n", i, obj);
        i++;
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    res.runtime = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() / 1000.0;
    res.obj = 0;
    for (auto const& inst : full_insts) {
        res.obj += inst.ub;
    }
    if (n == 1) {
        res.obj += subs[0].ub;
    }
    return res;
}
