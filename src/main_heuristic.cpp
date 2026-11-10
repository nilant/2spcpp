// Entry point `matheuristic`: shelves MIP + pairing matheuristic.
// Usage: matheuristic <instance.json> [--timelimit s ...]

#include <fmt/core.h>

#include "gurobi_c++.h"
#include "heuresult.hpp"
#include "heuristic.hpp"


int main(int argc, char* argv[]) {

    try {
        Args args{argc, argv};

        GRBEnv env{};
        #ifdef NDEBUG
            env.set(GRB_IntParam_OutputFlag, 0);
        #else
            env.set(GRB_IntParam_OutputFlag, 1);
        #endif

        Instance inst{args.input_file};
        fmt::print("##### Solving instance {}...\n", inst.name);

        HeurResult res = heuristic(env, inst, args);
        res.print();

    } catch (GRBException& e) {
        fmt::print(stderr, "error code = {}\n", e.getErrorCode());
        fmt::print(stderr, "error message = {}\n", e.getMessage());
    }
    return 0;
}
