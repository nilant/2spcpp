#include "heuristic.hpp"
#include "assignment.hpp"
#include "coord_less.hpp"
#include "coord.hpp"
#include "heuresult.hpp"
#include "instance.hpp"
#include "shelves.hpp"
#include "coord.hpp"
#include "bottom_left_plus.hpp"

#include <chrono>
#include <fmt/core.h>
#include <algorithm>


std::vector<Instance> solve_level(GRBEnv& env, std::vector<Instance>& subs, Instance const& inst, Args const& args) {

    int n = subs.size();
    if (n % 2 != 0) {
        Instance empty{};
        subs.push_back(empty);
        n++;
    }

    Args coord_args{args};
    coord_args.timelimit /= n;

    Args less_args{args};
    less_args.timelimit /= (n * (n - 1) / 2);

    mdarray<int, 2> combs{n, n};
    mdarray<Instance, 2> instances{n, n};

    for (int i = 0; i < n; ++i) {
        for (int j = i+1; j < n; ++j) {
            Instance new_inst = merge(subs[i], subs[j]);
            Solution merge_sol;
            merge_sol.name = "shelves";
            merge_sol.items = new_inst.selected_items;
            merge_sol.obj = new_inst.ub;

            auto shelves_obj = new_inst.ub;

            CoordLess coord_less(env, new_inst, new_inst.ub);
            auto less_res = coord_less.optimize(less_args, new_inst.ub);

            int probe_obj = less_res.obj;

            char star = ' ';
            int obj = probe_obj; 
            if (probe_obj == -1 || probe_obj > shelves_obj) {
                star = '*';
                obj = shelves_obj;
            }
            fmt::print("Optimize ({}, {}): pre={}, post={} runtime={:.2f}({}) {}\n", i, j, shelves_obj, probe_obj, less_res.runtime, coord_args.timelimit, star);

            new_inst.ub = obj;
            combs(i, j) = obj;
            instances(i, j) = new_inst;
        }
    }

    Assignment ass{env, combs};
    ass.optimize(args);
    auto new_subs = ass.select(instances);
    for (auto& sub : new_subs) {
        Coord coord(env, sub, sub.ub);
        auto coord_res = coord.optimize(coord_args, sub.ub);
        fmt::print("coord pre={}, post={}\n", sub.ub, coord_res.obj);

        if (coord_res.obj != -1) {
            sub = coord.subinst(sub);
        }
    }

    return new_subs;
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

    int nlevels = std::lrint(std::log2(n));

    Args coord_args{args};
    if (nlevels > 0) {
        coord_args.timelimit = (args.timelimit - 10) / nlevels;
    } else {
        coord_args.timelimit = args.timelimit - 10;
    }

    fmt::print("n={}, pairs={}, nlevels={}\n", n, (n * (n-1) / 2), nlevels);
    int i = 0;
    while (n >= 2) {
        subs = solve_level(env, subs, inst, coord_args);
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
