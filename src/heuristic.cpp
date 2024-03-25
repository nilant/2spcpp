#include "heuristic.hpp"
#include "assignment.hpp"
#include "heuresult.hpp"
#include "shelves.hpp"
#include "coord.hpp"
#include <chrono>


std::vector<Instance> solve_level(GRBEnv& env, std::vector<Instance> const& subs, Args const& args) {

    int n = subs.size();

    Args coord_args{args};
    coord_args.timelimit /= (n * (n-1) / 2);

    mdarray<int, 2> combs{n, n};
    mdarray<Instance, 2> instances{n, n};

    for (int i = 0; i < n; ++i) {
        for (int j = i+1; j < n; ++j) {
            Instance new_inst = merge(subs[i], subs[j]);
            Coord coord{env, new_inst, new_inst.ub};
            auto coord_res = coord.optimize(coord_args, new_inst.ub);
            new_inst.ub = coord_res.obj;
            combs(i, j) = coord_res.obj;
            instances(i, j) = new_inst;
        }
    }

    Assignment ass{env, combs};
    ass.optimize(args);
    return ass.select(instances);
}

HeurResult heuristic(GRBEnv& env, Instance const& inst, Args const& args) {

    auto t0 = std::chrono::high_resolution_clock::now();

    HeurResult res{"matheuristic"};

    Args shelves_args{args};
    shelves_args.timelimit = 10;

    Shelves shelves{env, inst};
    shelves.optimize(shelves_args);

    auto subs = shelves.subinsts(inst);

    int nlevels = std::lrint(std::log2(subs.size()));

    Args coord_args{args};
    coord_args.timelimit = (args.timelimit - 10) / nlevels;

    while (subs.size() >= 2) {
        subs = solve_level(env, subs, args);
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    res.runtime = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() / 1000.0;
    res.obj = subs[0].ub;

    return res;
}