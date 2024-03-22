#include "heuristic.hpp"
#include "heuresult.hpp"
#include "shelves.hpp"


HeurResult heuristic(GRBEnv& env, Instance const& inst, Args const& args) {

    HeurResult res{"matheuristic"};

    Args shelves_args{args};
    shelves_args.timelimit = 10;

    Shelves shelves{env, inst};
    shelves.optimize(shelves_args);

    auto subs = shelves.subinsts(inst);

    return res;
}