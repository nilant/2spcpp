#include "heuristic.hpp"
#include "heuresult.hpp"
#include "shelves.hpp"
#include "coord.hpp"
#include <chrono>


HeurResult heuristic(GRBEnv& env, Instance const& inst, Args const& args) {

    auto t0 = std::chrono::high_resolution_clock::now();

    HeurResult res{"matheuristic"};

    Args shelves_args{args};
    shelves_args.timelimit = 10;

    Shelves shelves{env, inst};
    shelves.optimize(shelves_args);

    auto subs = shelves.subinsts(inst);
    int n = subs.size();

    Args coord_args{shelves_args};
    coord_args.timelimit /= (n * (n-1) / 2);

    mdarray<int, 2> combs{n, n};

    for (int i = 0; i < n; ++i) {
        for (int j = i+1; j < n; ++j) {
            Instance new_inst = merge(subs[i], subs[j]);
            Coord coord{env, new_inst, new_inst.ub};
            auto coord_res = coord.optimize(coord_args);
            combs(i, j) = coord_res.obj;
        }
    }

    auto t1 = std::chrono::high_resolution_clock::now();

    res.runtime = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() / 1000.0;
    return res;
}