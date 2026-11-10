// Entry point `coord2`: run bottom-left+ to get an upper bound, then solve the
// coordinate MIP (repetitions as separate tasks). Usage: coord2 <instance.json>

#include <fmt/core.h>

#include "bottom_left_plus.hpp"
#include "coord2.hpp"
#include "mipresult.hpp"

int main(int argc, char* argv[]) {

    try {
        Args args{argc, argv};

        GRBEnv env{true};
        env.set(GRB_IntParam_OutputFlag, args.verbose ? 1 : 0);
        env.start();
        Instance inst{args.input_file};

        auto res_bl = bottom_left_plus(inst);
        int ub = res_bl.obj;
        
        Coord2 coord2{env, inst, ub};
        MIPResult res = coord2.optimize(args);
        res.print();

    } catch (GRBException& e) {
        fmt::print(stderr, "error code={}\n", e.getErrorCode());
        fmt::print(stderr, "error message={}\n", e.getMessage());
    }

    return 0;
}