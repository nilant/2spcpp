#include "heuristic.hpp"
#include "assignment.hpp"
#include "bottom_left.hpp"
#include "heuresult.hpp"
#include "instance.hpp"
#include "shelves.hpp"
#include "coord.hpp"

#include <chrono>
#include <fmt/core.h>


std::vector<Instance> solve_level(GRBEnv& env, std::vector<Instance>& subs, Instance const& inst, Args const& args) {

    if (subs.size() % 2 == 1) {
        subs.push_back(Instance{});
    }
    int n = subs.size();

    Args coord_args{args};
    coord_args.timelimit /= (n);

    mdarray<int, 2> combs{n, n};
    mdarray<Instance, 2> instances{n, n};

    for (int i = 0; i < n; ++i) {
        for (int j = i+1; j < n; ++j) {
            Instance new_inst = merge(subs[i], subs[j]);
            
            auto shelves_obj = new_inst.ub;

            auto bl_res = bottom_left(new_inst); 
            if ((i == 0 && j == 1) || (i == 0 && j == 2) || (i == 2 && j == 3))
                print_solution(new_inst.selected_items.begin(), new_inst.selected_items.end(), bl_res.obj, std::format("data/{}_{}_{}_{}.json", inst.name, "bl", i, j), "bl");

            auto bl_obj = bl_res.obj;
            fmt::print("Optimize ({}, {}): pre={}, post={}\n", i, j, new_inst.ub, bl_res.obj);

            new_inst.ub = bl_obj;
            combs(i, j) = bl_obj;
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
        sub.ub = std::min(static_cast<int>(std::lrint(coord_res.obj)), sub.ub);
    }

    return new_subs;
}

HeurResult heuristic(GRBEnv& env, Instance const& inst, Args const& args) {

    fmt::print("Optimizing {}...\n", inst.name);

    auto t0 = std::chrono::high_resolution_clock::now();

    HeurResult res{"matheuristic"};

    Args shelves_args{args};
    shelves_args.timelimit = 10;

    Shelves shelves{env, inst};
    auto shelves_sol = shelves.optimize(shelves_args);
    res.start_sol = shelves_sol.obj;
    
    fmt::print("starting solution={}\n", res.start_sol);

    auto subs = shelves.subinsts(inst);

    int nlevels = std::lrint(std::log2(subs.size()));

    Args coord_args{args};
    coord_args.timelimit = (args.timelimit - 10) / nlevels;

    int n = subs.size();
    fmt::print("n={}, pairs={}, nlevels={}\n", n, (n * (n-1) / 2), nlevels);
    int i = 0;
    while (subs.size() >= 2) {
        subs = solve_level(env, subs, inst, coord_args);
        int obj = 0;
        for (auto const& sub : subs) {
            obj += sub.ub;
        }
        fmt::print("level={}, obj={}\n", i, obj);
        i++;
        std::exit(1);
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    res.runtime = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() / 1000.0;
    res.obj = subs[0].ub;

    return res;
}
