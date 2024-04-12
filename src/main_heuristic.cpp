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

        HeurResult res = heuristic(env, inst, args);
        res.write(args.input_file);
        res.print();

    } catch (GRBException& e) {
        fmt::print("error code = {}\n", e.getErrorCode());
        fmt::print("error message = {}\n", e.getMessage());
    }
    return 0;
}
